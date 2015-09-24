#ifndef ESPClock_Settings_h
#define ESPClock_Settings_h

#include <IPAddress.h>
#include <EEPROM.h>

struct SettingsClass {
  static const uint32_t FLASH_MAGIC_R1 = 0x5aa5e001;  // Before adding multiple presets
  static const uint32_t FLASH_MAGIC_R2 = 0x5aa5e002;  // Before adding OpenWeatherMaps API key
  static const uint32_t FLASH_MAGIC = 0x5aa5e003;

  static const uint8_t N_DEVICES = 3;
  static const uint8_t N_PRESETS = 3;
  
  uint32_t magic;
  char ssid[32];  // ESP seems to have a 31-char limit
  char password[64];
  char hostname[32];  // For mDNS; *not* FQDN
  char ntp_hostname[64];
  int16_t utc_offset;  // in seconds
  char zip[6];
  char dev_hostname[N_DEVICES][64];
  // R, G, B, W
  uint8_t preset_r[N_PRESETS];
  uint8_t preset_g[N_PRESETS];
  uint8_t preset_b[N_PRESETS];
  uint8_t preset_w[N_PRESETS];
  char owm_apikey[36];

  void begin();
  void save();
};

extern SettingsClass Settings;

#endif  // ESPClock_Settings_h

