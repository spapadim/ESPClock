#ifndef ESPClockWebServer_h
#define ESPClockWebServer_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

class WebServerClass {  // This is really a namespace, just in odd Arduino-style
public:
  void begin();
  void loop();

private:
  static void handleNotFound();

  static void handleBasicSetup();
  static void handleNetConfig();

  static void handleSettingsHtml();
  static void handleApiSettings();
  static void handleApiUpdate();

  static ESP8266WebServer _server;
};

extern WebServerClass WebServer;

#endif  // ESPClockWebServer_h

