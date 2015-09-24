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

  static void urlDecode(char *decoded, const char *encoded, size_t n);

private:
  static void _sendJsonStatus(bool success);

  static void handleNotFound();

  static void handleBasicSetup();
  static void handleNetConfig();

  static void handleControlHtml();
  static void handleApiDevOn();
  static void handleApiDevOff();
  static void handleApiDevPreset();
  static void handleApiDevRGBW();
  static void handleApiDevParams();

  static void handleSettingsHtml();
  static void handleApiSettings();
  static void handleApiUpdate();

  static ESP8266WebServer _server;
};

extern WebServerClass WebServer;

#endif  // ESPClockWebServer_h
