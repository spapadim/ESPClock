#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

#include <errno.h>

#include "Settings.h"
#include "WebServer.h"

ESP8266WebServer WebServerClass::_server;

WebServerClass WebServer;

#include "settings_html.h"  // Not really a header file, but Arduino build barfs with other extensions
#include "basicsetup_html.h"  // Ditto

void WebServerClass::handleNotFound() {
  // TODO Make it pretty
  _server.send(404, FPSTR("text/plain"), "Invalid webpage");
}

void WebServerClass::handleBasicSetup() {
  _server.send_P(200, "text/html", (const char*)basicsetup_html);
}

void WebServerClass::handleNetConfig() {
  if (!_server.hasArg("ssid") || !_server.hasArg("password")) {
    _server.send(400, "text/plain", "Missing fields");
    return;
  }

  String ssid = _server.arg("ssid"),
         password = _server.arg("password");
  if (ssid.length() > sizeof(Settings.ssid) - 1 ||
      password.length() > sizeof(Settings.password) - 1) {
    _server.send(400, "text/plain", "Fields too long");
    return;
  }

  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  
  Settings.save();
  _server.send(200, "text/plain", "Settings stored; please power cycle.");
}

void WebServerClass::handleSettingsHtml() {
  _server.send_P(200, "text/html", (const char*)settings_html);
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
  s["r"] = Settings.preset_r;
  s["g"] = Settings.preset_g;
  s["b"] = Settings.preset_b;
  s["w"] = Settings.preset_w;

  // printTo(String) does not seem to work?
  //String data;
  //s.printTo(data);
  char json_str[512];
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
    _server.send(400, "text/plain", "Missing JSON payload");
    return;
  }

  StaticJsonBuffer<2048> _json;  // Size 512 failed, although it was sufficient to construct object above
  JsonObject &s = _json.parseObject(_server.arg("plain"));

  if (!s.success()) {
    _server.send(400, "text/plain", "JSON parse error");
    return;
  }

  // Check that number arguments are valid
  int utc_offset = s["utcOffset"].as<int>(),
      r = s["r"].as<int>(),
      g = s["g"].as<int>(),
      b = s["b"].as<int>(),
      w = s["w"].as<int>();
  if (utc_offset < -12*3600 || utc_offset > 12*3600) {
    _server.send(400, "text/plain", "Invalid UTC offset value");
    return;
  }
  if (r < 0 || g < 0 || b < 0 || w < 0 || \
      r > 255 || g > 255 || b > 255) {
    _server.send(400, "text/plain", "Color values not within 0..255");
    return;
  }

  // Check number of devices
  JsonArray &dev = s["dev"].asArray();
  // TODO: Check if array is JsonArray::invalid()
  if (dev.size() > SettingsClass::N_DEVICES) {
    _server.send(400, "text/plain", "Too many devices");
    return;
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
      _server.send(400, "text/plain", "Argument too long");
      return;
   }
   // Now check device hostnames
   String dev_hostname[dev.size()];
   for (int i = 0;  i < dev.size();  i++) {  // TODO Use iterator, it's a linked list, this is O(N^2)
      dev_hostname[i] = dev[i].as<String>();
      if (dev_hostname[i].length() > sizeof(Settings.dev_hostname[0]) - 1) {
        _server.send(400, "text/plain", "Device hostname too long");  // Might as well be specific
        return;
      }
   }
         
  // Update and save settings
  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  strcpy(Settings.hostname, hostname.c_str());
  Settings.utc_offset = (int16_t)utc_offset;
  strcpy(Settings.zip, zip.c_str());
  for (int i = 0;  i < dev.size();  i++) {  // TODO Iterator here too
    strcpy(Settings.dev_hostname[i], dev_hostname[i].c_str());
  }
  
  Settings.save();
  _server.send(200, "application/json", "{ \"success\": true }");
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

