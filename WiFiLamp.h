#ifndef ESPClock_WiFiLamp_h
#define ESPClock_WiFiLamp_h

#include "Settings.h"

class WiFiLampClass {
public:
  static const uint16_t TCP_PORT = 5577;  // Port on which lamp modules listen

  WiFiLampClass() { }

  void begin();

  bool lampOn(uint8_t i);
  bool lampOff(uint8_t i);
  bool lampRGBW(uint8_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0x00);
  bool lampPreset(uint8_t i);

private:
  // We want connections as fast as possible, so cache IPs
  IPAddress _dev_ip[SettingsClass::N_DEVICES];

  bool _sendCommand(uint8_t i, const uint8_t *data, size_t len);
};

extern WiFiLampClass WiFiLamp;

#endif  // ESPClock_WiFiLamp_h

