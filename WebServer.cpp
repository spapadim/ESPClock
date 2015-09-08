#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

#include <errno.h>

#include "Settings.h"
#include "WebServer.h"

ESP8266WebServer WebServerClass::_server;

const char TEXT_PLAIN[] PROGMEM = "text/plain";
const char TEXT_HTML[] PROGMEM = "text/html";
const char APPLICATION_JSON[] PROGMEM = "application/json";

WebServerClass WebServer;

#include "settings_html.h"  // Not really a header file, but Arduino build barfs with other extensions
#include "basicsetup_html.h"  // Ditto

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

  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  
  Settings.save();
  _server.send_P(200, TEXT_PLAIN, PSTR("Settings stored; please power cycle."));
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
         zip = s["zip"].as<String>();
   if (ssid.length() > sizeof(Settings.ssid) - 1 ||
       password.length() > sizeof(Settings.password) - 1 ||
       hostname.length() > sizeof(Settings.hostname) - 1 ||
       zip.length() > sizeof(Settings.zip) - 1 ) {
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
  _server.send_P(200, APPLICATION_JSON, PSTR("{ \"success\": true }"));
}

void WebServerClass::begin() {
  if (WiFi.status() == WL_CONNECTED) {
    _server.on("/", handleSettingsHtml);
    _server.on("/api/settings", HTTP_GET, handleApiSettings);
    _server.on("/api/update", HTTP_POST, handleApiUpdate);
  } else {
    // Internet is not available, cannot get Bootstrap, etc
    // so fall back to a very basic webpage
    _server.on("/", handleBasicSetup);
    _server.on("/netconfig", handleNetConfig);    
  }
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

