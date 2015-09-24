#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Digole.h>

#include "Settings.h"
#include "Config.h"

#include "Font.h"
#include "Widget.h"
#include "Screen.h"

#include "NTPClient.h"
#include "WiFiLamp.h"
#include "WebServer.h"
#include "OWMClient.h"


static const char DEFAULT_AP_PASSWORD[] = "esp";
static const char WIFI_CONNECT_TIMEOUT = 10;  // in seconds

static const time_t NTP_SYNC_INTERVAL = 3600;  // in seconds
static const time_t OWM_UPDATE_INTERVAL = 1800;  // in seconds

extern Label lbl_time, lbl_date, lbl_splash_top, lbl_splash_bottom;
extern Button btn_on, btn_preset, btn_off;
extern Button btn_dev0, btn_dev1, btn_dev2;
extern Button *buttons[];

static Button *active_button = 0;  // Last active button

static Button *find_button(Button **buttons, uint16_t x, uint16_t y) {
  Button **b;
  for (b = buttons; (*b) != 0; b++) {
    if ((*b)->contains(x, y))
      break;
  }
  return *b;
}

static inline void update_backlight(bool on, time_t time_now = 0) {
  static time_t last_on = 0;

  if (time_now == 0)
    time_now = now();

  if (on) {
#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
    //Serial.println("BL WAKE");  // DEBUG
#endif
    last_on = time_now;
    LCD.setBacklight(BACKLIGHT_ON);
  } else if (last_on > 0 && time_now > last_on + BACKLIGHT_TIMEOUT) {
#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
    //Serial.println("BL SLEEP");  // DEBUG
#endif
    last_on = 0;
    LCD.setBacklight(BACKLIGHT_OFF);
    LCD.setBacklight(BACKLIGHT_OFF);  // Digole bug: needs second command; not sure with turning on is ok with one
  }
}

static uint8_t selected_dev = 0;
static uint8_t cur_preset = 0;

static void touch_down_or_move_cb(uint16_t x, uint16_t y) {
  update_backlight(true);

  Button *b = find_button(buttons, x, y);
  if (b != active_button) {
    if (active_button != 0) {
      active_button->setActive(false);
      active_button->draw();
      active_button = 0;
    }
    if (b != 0) {
      b->setActive(true);
      b->draw();
      active_button = b;
    }
  }
}

static void touch_up_cb(uint16_t x, uint16_t y) {
  Button *b = find_button(buttons, x, y);

#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
  if (b != 0) {
    Serial.print("Button press: "); Serial.println(b->getLabel()); Serial.flush();   // DEBUG
  }
#endif

  if (active_button != 0) {
    active_button->setActive(false);
    active_button->draw();
    active_button = 0;
  }

  if (b != 0) {
    uint8_t b_id = b->getId();
    if (b_id >= ID_DEV0) {
      // Update selected button
      uint8_t i_dev = b_id - ID_DEV0;
      buttons[3 + selected_dev]->setPressed(false);
      buttons[3 + selected_dev]->draw();
      buttons[3 + i_dev]->setPressed(true);
      buttons[3 + i_dev]->draw();
      selected_dev = i_dev;
    } else if (b_id == ID_OFF) {
      bool res = WiFiLamp.lampOff(selected_dev);
      //lcdDebugPrintf(0, "OFF(%d) = %d", (int)selected_dev, (res ? 1 : 0));
    } else if (b_id == ID_ON) {
      bool res = WiFiLamp.lampOn(selected_dev);
      //lcdDebugPrintf(0, "ON(%d) = %d", (int)selected_dev, (res ? 1 : 0));
    } else if (b_id == ID_PRESET) {
      WiFiLamp.lampPreset(selected_dev, cur_preset);
      cur_preset = (cur_preset + 1) % SettingsClass::N_PRESETS;
    }
  }
}

static void update_time(bool force = false) {
  static time_t last_update = 0;
  static char buf[12];
  static char led_state = LOW;

  time_t time_now = now();

  update_backlight(false, time_now);

  if ((time_now != last_update && (time_now % 60) == 0) || force) {
    int hr = hour();
    sprintf(buf, "%02d:%02d", hr, minute());
    lbl_time.setLabel(buf);
    lbl_time.draw();
#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
    Serial.print("New time: "); Serial.println(buf);  Serial.flush();  // DEBUG
#endif

    if (hr == 0 || force) {
      // Need to break up into two sprintf() because Time library uses static buffer
      sprintf(buf, "%3s", dayShortStr(weekday()));
      sprintf(buf + 3, ", %3s %2d", monthShortStr(month()), day());
      lbl_date.setLabel(buf);
      lbl_date.draw();
    }

    last_update = time_now;
  }
}

static void connect_wifi() {
  if (Settings.ssid[0] != '\0') {
    WiFi.hostname(Settings.hostname);
    WiFi.begin(Settings.ssid, Settings.password);
    int s = 0;
    while ((WiFi.status() != WL_CONNECTED) && (s < 2*WIFI_CONNECT_TIMEOUT)) {
      delay(500);  // Must match while condition above
      ++s;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Fall back to soft-AP mode
    WiFi.softAP(Settings.hostname, DEFAULT_AP_PASSWORD);
  }
}

static void time_cb(time_t time) {
  setTime(time);
  update_time(true);
}

static void weather_cb(const OWMClient::WeatherInfo &conditions) {
  // TODO
}


void setup() {
  char buf[40];
  
#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
  Serial.begin(115200);
  Serial.println("**** START ****");
#endif
  
  Settings.begin();

  LCD.begin();
  Touch.begin(touch_down_or_move_cb, touch_down_or_move_cb, touch_up_cb);

  LCD.setDisplayConfig(false);
  LCD.clearScreen();
  LCD.setRotation(Digole::ROT270);

  lbl_splash_top.setAlignment(Label::BOTTOM);
  lbl_splash_bottom.setAlignment(Label::TOP);
  lbl_time.setFont(201, &metric_DroidMono50);
  lbl_time.setJustification(Label::LEFT);
  lbl_time.setMargin(0);
  lbl_date.setFgColor(GRAY);
  lbl_date.setJustification(Label::LEFT);
  lbl_date.setMargin(12);
#if defined(_DIGOLE_KLUDGE) && _DIGOLE_KLUDGE
  lbl_time.setDrawMode(Digole::MODE_COPY);
  lbl_date.setDrawMode(Digole::MODE_COPY);
  lbl_splash_top.setDrawMode(Digole::MODE_COPY);
  lbl_splash_bottom.setDrawMode(Digole::MODE_COPY);
#endif

  lbl_splash_top.draw();
  if (Settings.hostname[0] != '\0') {
    lbl_splash_bottom.setLabel(Settings.hostname);
    lbl_splash_bottom.draw();
    delay(1000);
  }
  // otherwise, lbl_splash_bottom is just "" initially
  
  update_backlight(true);

  btn_dev0.setPressed(true);
  selected_dev = 0;

  delay(200);

  snprintf(buf, sizeof(buf), "%s..", Settings.ssid);
  lbl_splash_top.setLabel("Connecting to");
  lbl_splash_bottom.setLabel(buf);
  lbl_splash_top.draw();
  lbl_splash_bottom.draw();
  connect_wifi();

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.begin(time_cb);
    OWM.begin(weather_cb);
    WiFiLamp.begin();

    // Clear splashscreen and show main UI
    LCD.clearScreen();
    LCD.setRotation(Digole::ROT270);  // clearScreen resets orientation??
    lbl_time.draw();
    lbl_date.draw();

    // Draw all buttons
    for (Button **b = buttons; (*b) != 0; b++)
      (*b)->draw();

    NTPClient.startRequest();
    OWM.startUpdate();
  } else {
    // Display brief configuration instructions
    lbl_splash_top.setLabel("Please connect to ESP_xxx");
    lbl_splash_bottom.setLabel("wifi to configure.");
    lbl_splash_top.draw();
    lbl_splash_bottom.draw();
  }
  WebServer.begin();
}

void loop() {
  WebServer.loop();
  Touch.loop();
  update_time();

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.loop();
    OWM.loop();

    static time_t last_ntp = 0, last_owm = 0;
    time_t t_now = now();
    if (t_now - last_ntp > NTP_SYNC_INTERVAL) {
      NTPClient.startRequest();
      last_ntp = t_now;
    }
    if (t_now - last_ntp > OWM_UPDATE_INTERVAL) {
      OWM.startUpdate();
      last_owm = t_now;
    }
  }
}

