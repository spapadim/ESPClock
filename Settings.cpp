#include <Arduino.h>
#include <EEPROM.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "Settings.h"

SettingsClass Settings;

static const size_t EEPROM_SIZE = 512;

void SettingsClass::begin() {
#if defined(ESP8266)
  EEPROM.begin(EEPROM_SIZE);
#else
  EEPROM.begin();
#endif
  EEPROM.get(0, *this);

  // Populate defaults if flash magic is unexpected
  if (magic != FLASH_MAGIC) {
    if (magic != FLASH_MAGIC_R1) {
      bzero(this, sizeof(*this));
#if defined(ESP8266)
      uint8_t mac[WL_MAC_ADDR_LENGTH];
      WiFi.macAddress(mac);
      sprintf(hostname, "espclock%02x", mac[WL_MAC_ADDR_LENGTH-1]);
#else
      strcpy(hostname, "espclock");
#endif
      strcpy(ntp_hostname, "us.pool.ntp.org");
    }
    memset(preset_r, 0xff, sizeof(preset_r));
    memset(preset_g, 0xff, sizeof(preset_g));
    memset(preset_b, 0xff, sizeof(preset_b));
    memset(preset_w, 0xff, sizeof(preset_w));

    magic = FLASH_MAGIC;
    save();  // Is this really necessary?
  }
}

void SettingsClass::save() {
  EEPROM.put(0, *this);
#if defined(ESP8266)
  EEPROM.commit();
#endif
}

