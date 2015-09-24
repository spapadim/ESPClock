#ifndef ESPClock_OWMClient_h
#define ESPClock_OWMClient_h

#include <Arduino.h>

// OpenWeatherMap client
class OWMClient {
public:
  static const char API_HOST[];  // We pass this to WiFiClient::open, so can't be PROGMEM
  static const char API_ENDPOINT_ZIP[] PROGMEM;
  static const char API_APIKEY_FIELD[] PROGMEM;

  struct WeatherInfo {
    time_t time;
    char location_name[24];
    char condition[16];
    float temperature;
    float humidity;
    float pressure;
    float temp_min, temp_max;
    float wind_speed, wind_deg;
  };

  typedef void (*owm_cb_t)(const WeatherInfo &conditions);

  OWMClient() : _cb(0), _state(IDLE) { }

  void begin(owm_cb_t cb);
  void loop();

  bool startUpdate();
  bool updating() const { return _state != IDLE; }
  const WeatherInfo &getConditions() const { return _conditions; }
  
private:
  owm_cb_t _cb;
  WiFiClient _client;
  WeatherInfo _conditions;
  enum : uint8_t { IDLE, WAITING_RESPONSE, READING_HEADERS, READING_BODY } _state;
};

extern OWMClient OWM;

#endif  // ESPClock_OWMClient_h
