#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

#include <errno.h>
#include <stdio.h>

#include "Settings.h"
#include "WebServer.h"
#include "WiFiLamp.h"

ESP8266WebServer WebServerClass::_server;

const char TEXT_PLAIN[] PROGMEM = "text/plain";
const char TEXT_HTML[] PROGMEM = "text/html";
const char APPLICATION_JSON[] PROGMEM = "application/json";

WebServerClass WebServer;

static inline uint8_t hexvalue(char c) {
  c = toupper(c);
  if (c >= 'A') {
    return c - 'A' + 10;
  } else {
    return c - '0';
  }
}

void WebServerClass::urlDecode(char *decoded, const char *encoded, size_t n) {
  const char *p = encoded;
  char *q = decoded;
  while (*p != '\0') {
    // Check length limit first, make sure it's zero-terminated
    if ((q - decoded >= n) && (n > 0)) {
      q[-1] = '\0';
      return;
    }
    if (*p == '+') {
      *q = ' ';
    } else if (*p == '%' && isxdigit(p[1]) && isxdigit(p[2])) {
      *q = (char)(hexvalue(p[1])*16 + hexvalue(p[2]));
      p += 2;  // Additional chars to skip
    } else {
      *q = *p;
    }
    ++p; ++q;
  }
  *q = '\0';
}

#include "settings_html.h"  // Not really a header file, but Arduino build barfs with other extensions
#include "basicsetup_html.h"  // Ditto
#include "update_html.h"

void WebServerClass::_sendJsonStatus(bool success) {
  if (success) {
    _server.send_P(200, APPLICATION_JSON, PSTR("{ \"success\": true }"));    
  } else {
    _server.send_P(200, APPLICATION_JSON, PSTR("{ \"success\": true }"));        
  }  
}

void WebServerClass::_sendConnectionHeader() {
  _server.sendHeader("Connection", "close");
}

void WebServerClass::handleNotFound() {
  // TODO Make it pretty
  _server.send_P(404, TEXT_PLAIN, PSTR("Invalid webpage"));
}

void WebServerClass::handleBasicSetup() {
  _server.send_P(200, TEXT_HTML, (const char*)basicsetup_html);
}

void WebServerClass::handleNetConfig() {
  if (!_server.hasArg("ssid") || !_server.hasArg("password")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing fields"));
    return;
  }

  String ssid = _server.arg("ssid"),
         password = _server.arg("password");
  if (ssid.length() > sizeof(Settings.ssid) - 1 ||
      password.length() > sizeof(Settings.password) - 1) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Fields too long"));
    return;
  }

  char dec_ssid[32], dec_password[64];  // decoded
  urlDecode(dec_ssid, ssid.c_str(), sizeof(dec_ssid));
  urlDecode(dec_password, password.c_str(), sizeof(dec_password));
  strcpy(Settings.ssid, dec_ssid);
  strcpy(Settings.password, dec_password);
  
  Settings.save();
  _server.send_P(200, TEXT_PLAIN, PSTR("Settings stored; please power cycle."));
}

void WebServerClass::handleUpdate() {
  _server.send_P(200, TEXT_HTML, (const char*)update_html);
}

void WebServerClass::handleDoUpdate() {
  // From WebUpdate example sketch in esp8266/Arduino
  _sendConnectionHeader();  // TODO Send this in all responses
  char buf[128];
  if (Update.hasError()) {
    snprintf_P(buf, sizeof(buf), PSTR("Firmware update failed with error code %d"), Update.getError());
  } else {
    strncpy_P(buf, PSTR("Firmware update successful; rebooting!"), sizeof(buf));    
  }
  _server.send_P(200, TEXT_PLAIN, buf);
  delay(1000);
  ESP.restart();
}

void WebServerClass::handleFileUpload() {
  // From WebUpdate example sketch in esp8266/Arduino
  if (_server.uri() != "/doupdate") {
    return;
  }
  if (Update.hasError()) {
    return;  // Ignore any (further) received data
  }
  HTTPUpload &upload = _server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    WiFiUDP::stopAll();
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Update.begin(maxSketchSpace);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Update.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    Update.end(true); // true to set the size to the current progress
  }
  yield();
}

void WebServerClass::handleControlHtml() {
  // TODO
}

void WebServerClass::handleApiDevOn() {
  if (!_server.hasArg("d")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing device parameter"));
    return;
  }

  uint8_t dev = (uint8_t)strtol(_server.arg("d").c_str(), NULL, 10);  // No sscanf in stdlibs?
  _sendJsonStatus(WiFiLamp.lampOn(dev));
}

void WebServerClass::handleApiDevOff() {
  if (!_server.hasArg("d")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing device parameter"));
    return;
  }

  uint8_t dev = (uint8_t)strtol(_server.arg("d").c_str(), NULL, 10);
  _sendJsonStatus(WiFiLamp.lampOn(dev));
}

void WebServerClass::handleApiDevPreset() {
  if (!_server.hasArg("d") || !_server.hasArg("p")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing parameter(s)"));
    return;
  }

  uint8_t dev = (uint8_t)strtol(_server.arg("d").c_str(), NULL, 10);
  uint8_t pre = (uint8_t)strtol(_server.arg("p").c_str(), NULL, 10);
  _sendJsonStatus(WiFiLamp.lampPreset(dev, pre));
}

void WebServerClass::handleApiDevRGBW() {
  if (!_server.hasArg("d") || 
      !_server.hasArg("r") || !_server.hasArg("g") || !_server.hasArg("b") || 
      !_server.hasArg("w")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing parameter(s)"));
    return;
  }

  // TODO: Allow either RGB or W
  uint8_t dev = (uint8_t)strtol(_server.arg("d").c_str(), NULL, 10);
  uint8_t r = (uint8_t)strtol(_server.arg("r").c_str(), NULL, 10);
  uint8_t g = (uint8_t)strtol(_server.arg("g").c_str(), NULL, 10);
  uint8_t b = (uint8_t)strtol(_server.arg("b").c_str(), NULL, 10);
  uint8_t w = (uint8_t)strtol(_server.arg("w").c_str(), NULL, 10);
  _sendJsonStatus(WiFiLamp.lampRGBW(dev, r, g, b, w));
}

void WebServerClass::handleApiDevParams() {
  StaticJsonBuffer<512> _json;
  JsonObject &p = _json.createObject();

  p["numDevices"] = SettingsClass::N_DEVICES;
  p["numPresets"] = SettingsClass::N_PRESETS;

  char json_str[512];
  p.printTo(json_str, sizeof(json_str));
  _server.send(200, "application/json", json_str);
}

void WebServerClass::handleSettingsHtml() {
  _server.send_P(200, TEXT_HTML, (const char*)settings_html);
}

void WebServerClass::handleApiSettings() {
  StaticJsonBuffer<1024> _json;
  JsonObject &s = _json.createObject();

  s["ssid"] = Settings.ssid;
  s["password"] = Settings.password;
  s["hostname"] = Settings.hostname;
  s["ntpHostname"] = Settings.ntp_hostname;
  s["utcOffset"] = Settings.utc_offset;
  s["zip"] = Settings.zip;
  s["owmApiKey"] = Settings.owm_apikey;
  JsonArray &dev = s.createNestedArray("dev");
  for (int i = 0;  i < SettingsClass::N_DEVICES;  i++) {
    dev.add(Settings.dev_hostname[i]);
  }
  JsonArray &pre = s.createNestedArray("presets");
  for (int i = 0;  i< SettingsClass::N_PRESETS;  i++) {
    JsonObject &p = pre.createNestedObject();
    p["r"] = Settings.preset_r[i];
    p["g"] = Settings.preset_g[i];
    p["b"] = Settings.preset_b[i];
    p["w"] = Settings.preset_w[i];
  }

  // printTo(String) does not seem to work?
  //String data;
  //s.printTo(data);
  char json_str[1024];
  s.printTo(json_str, sizeof(json_str));
  _server.send(200, "application/json", json_str);
}

#if 0
static bool validateNumberArgValue(int &val, const String &value, int min, int max) {
  long l_val;
  char *endptr;
  strtol(value.c_str(), &endptr, 10);
  if ((errno == ERANGE) || (*endptr != 0) ||
      (l_val < min) || (l_val > max))
    return false;
  return (int)l_val;
}
#endif

// TODO: Instead of HTTP 400, send 200 with JSON error message?
void WebServerClass::handleApiUpdate() {
  // Check if JSON payload is present
  if (!_server.hasArg("plain")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing JSON payload"));
    return;
  }

  //StaticJsonBuffer<2048> _json;  // Size 512 failed, although it was sufficient to construct object above
  DynamicJsonBuffer _json;
  JsonObject &s = _json.parseObject(_server.arg("plain"));

  if (!s.success()) {
    _server.send_P(400, TEXT_PLAIN, PSTR("JSON parse error"));
    return;
  }

  // Check that UTC offset is valid
  int utc_offset = s["utcOffset"].as<int>();
  if (utc_offset < -12*3600 || utc_offset > 12*3600) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Invalid UTC offset value"));
    return;
  }

  // Check number of devices
  JsonArray &dev = s["dev"].asArray();
  // TODO: Check if array is JsonArray::invalid()
  if (dev.size() > SettingsClass::N_DEVICES) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Too many devices"));
    return;
  }

  // Check number of presets
  JsonArray &pre = s["presets"].asArray();
  if (pre.size() > SettingsClass::N_PRESETS) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Too many color presets"));
  }
  // Check that color values are valid
  int pre_r[pre.size()], pre_g[pre.size()], pre_b[pre.size()], pre_w[pre.size()];
  for (int i = 0;  i < pre.size();  i++) {  // TODO iterator
    JsonObject &pre_i = pre[i].asObject();
    pre_r[i] = pre_i["r"].as<int>();
    pre_g[i] = pre_i["g"].as<int>();
    pre_b[i] = pre_i["b"].as<int>();
    pre_w[i] = pre_i["w"].as<int>();
    if (pre_r[i] < 0 || pre_g[i] < 0 || pre_b[i] < 0 || pre_w[i] < 0 || \
        pre_r[i] > 255 || pre_g[i] > 255 || pre_b[i] > 255 || pre_w[i] > 255) {
      _server.send_P(400, TEXT_PLAIN, PSTR("Color values not within 0..255"));
      return;
    }
  }

  // Check length of string arguments (except device hostnames)
  String ssid = s["ssid"].as<String>(),
         password = s["password"].as<String>(),
         hostname = s["hostname"].as<String>(),
         zip = s["zip"].as<String>(),
         owm_apikey = s["owmApiKey"].as<String>();
   if (ssid.length() > sizeof(Settings.ssid) - 1 ||
       password.length() > sizeof(Settings.password) - 1 ||
       hostname.length() > sizeof(Settings.hostname) - 1 ||
       zip.length() > sizeof(Settings.zip) - 1 ||
       owm_apikey.length() > sizeof(Settings.owm_apikey) - 1) {
      _server.send_P(400, TEXT_PLAIN, PSTR("Argument too long"));
      return;
   }
   // Now check device hostnames
   String dev_hostname[dev.size()];
   for (int i = 0;  i < dev.size();  i++) {  // TODO Use iterator, it's a linked list, this is O(N^2)
      dev_hostname[i] = dev[i].as<String>();
      if (dev_hostname[i].length() > sizeof(Settings.dev_hostname[0]) - 1) {
        _server.send_P(400, TEXT_PLAIN, PSTR("Device hostname too long"));  // Might as well be specific
        return;
      }
   }
         
  // Update and save settings
  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  strcpy(Settings.hostname, hostname.c_str());
  Settings.utc_offset = (int16_t)utc_offset;
  strcpy(Settings.zip, zip.c_str());
  strcpy(Settings.owm_apikey, owm_apikey.c_str());
  for (int i = 0;  i < dev.size();  i++) {
    strcpy(Settings.dev_hostname[i], dev_hostname[i].c_str());
  }
  for (int i = 0;  i < pre.size();  i++) {
    Settings.preset_r[i] = (uint8_t)pre_r[i];
    Settings.preset_g[i] = (uint8_t)pre_g[i];
    Settings.preset_b[i] = (uint8_t)pre_b[i];
    Settings.preset_w[i] = (uint8_t)pre_w[i];
  }
  
  Settings.save();
  _sendJsonStatus(true);
}

void WebServerClass::begin() {
  if (WiFi.status() == WL_CONNECTED) {
    _server.on("/", handleSettingsHtml);
    _server.on("/api/settings", HTTP_GET, handleApiSettings);
    _server.on("/api/update", HTTP_POST, handleApiUpdate);
    _server.on("/control", handleControlHtml);
    _server.on("/api/dev/on", handleApiDevOn);
    _server.on("/api/dev/off", handleApiDevOff);
    _server.on("/api/dev/preset", handleApiDevPreset);
    _server.on("/api/dev/rgbw", handleApiDevRGBW);
    _server.on("/api/dev/params", handleApiDevParams);
  } else {
    // Internet is not available, cannot get Bootstrap, etc
    // so fall back to a very basic webpage
    _server.on("/", HTTP_GET, handleBasicSetup);
    _server.on("/netconfig", HTTP_POST, handleNetConfig);    
  }
  _server.on("/update", HTTP_GET, handleUpdate);
  _server.on("/doupdate", HTTP_POST, handleDoUpdate);
  _server.onFileUpload(handleFileUpload);
  _server.onNotFound(handleNotFound);

  _server.begin();

  // Start mDNS responder and register web interface
  if (WiFi.status() == WL_CONNECTED && MDNS.begin(Settings.hostname)) {
    MDNS.addService("http", "tcp", 80);
  }
}

void WebServerClass::loop() {
  _server.handleClient();
}

