#ifndef ESPClock_Config_h
#define ESPClock_Config_h

#if defined(ESP8266)
static const uint8_t LED_PIN = BUILTIN_LED;
static const uint8_t DISPLAY_RESET_PIN = 13;
#else
static const uint8_t LED_PIN = 13;
static const uint8_t DISPLAY_RESET_PIN = 0xff;  // Disabled
#endif

static const uint8_t BACKLIGHT_ON = 70;  // for 3.3V VCC, 70 seems ok; for 5V could go lower
static const uint8_t BACKLIGHT_OFF = 5;
static const uint8_t BACKLIGHT_TIMEOUT = 20;  // in seconds

static const uint8_t ID_TIME = 0;
static const uint8_t ID_DATE = 1;
static const uint8_t ID_SPLASH = 2;

static const uint8_t ID_ON = 100;
static const uint8_t ID_PRESET = 101;
static const uint8_t ID_OFF = 102;

// Buttons for device selection must have the highest IDs;
// the IDs must be contiguous and have the same order as device indices
static const uint8_t ID_DEV0 = 200;
static const uint8_t ID_DEV1 = 201;
static const uint8_t ID_DEV2 = 202;



#endif  // ESPClock_Config_h
