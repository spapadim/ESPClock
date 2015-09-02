#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <IPAddress.h>

#include "Settings.h"
#include "WiFiLamp.h"
#include "Screen.h"  // for lcdDebugPrintf

WiFiLampClass WiFiLamp;

void WiFiLampClass::begin() {
  for (int i = 0;  i < SettingsClass::N_DEVICES;  i++) {
    if (Settings.dev_hostname[i][0] != '\0') {
      WiFi.hostByName(Settings.dev_hostname[i], _dev_ip[i]);
    }
  }
}

bool WiFiLampClass::lampOn(uint8_t i) {
  uint8_t _cmd[4]  = { 0x71, 0x23, 0x0f, 0xa3 };
  return _sendCommand(i, _cmd, sizeof(_cmd));
}

bool WiFiLampClass::lampOff(uint8_t i) {
  uint8_t _cmd[4] = { 0x71, 0x24, 0x0f, 0xa4 };
  return _sendCommand(i, _cmd, sizeof(_cmd));
}

bool WiFiLampClass::lampRGBW(uint8_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  uint8_t _cmd[8] = { 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00 };
  
  // RGB and W seem to be mutually exclusive (at least from iPad app)
  if (w > 0) {
    _cmd[4] = w;
    _cmd[5] = 0x0f;  // RGB/W mask?
  } else {
    _cmd[1] = r;
    _cmd[2] = g;
    _cmd[3] = b;
    _cmd[5] = 0xf0;  // RGB/W mask?
  }
  
  // Compute checksum
  for (int i = 0;  i < sizeof(_cmd) - 1;  i++)
    _cmd[sizeof(_cmd)-1] += _cmd[i];

  return _sendCommand(i, _cmd, sizeof(_cmd));
}

bool WiFiLampClass::lampPreset(uint8_t i) {
  return lampRGBW(i, Settings.preset_r, Settings.preset_g, Settings.preset_b, Settings.preset_w);
}

bool WiFiLampClass::_sendCommand(uint8_t i, const uint8_t *data, size_t len) {
  if (i < 0 || i >= SettingsClass::N_DEVICES) {
    //lcdDebugPrintf(2, "ERR: BOUND");
    return false;  // Invalid device
  }

  if (Settings.dev_hostname[i][0] == '\0') {
    //lcdDebugPrintf(2, "ERR: NOIP");
    return false;  // Not configured
  }
  
  WiFiClient tcp;

  if (!tcp.connect(_dev_ip[i], TCP_PORT)) {
    //lcdDebugPrintf(2, "ERR: CONN");
    return false;  // Failed to connect
  }

  tcp.write(data, len);
  tcp.flush();
  delay(50);  // TODO: Necessary?? ;  Make sure data gets chance to be sent out
  return true;
}

