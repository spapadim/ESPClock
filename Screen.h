#ifndef ESPClock_Touch_h
#define ESPClock_Touch_h

//#include <functional>  // TODO max() conflicts

#include <Arduino.h>
#include <Digole.h>

extern void lcdDebugPrintf(int16_t y, const char *fmt, ...) __attribute__((format (printf, 2, 3)));;

class Touchscreen {
public:

#if defined(ESP8266)
  static const uint8_t INT_PIN = 12;
#else
  static const uint8_t INT_PIN = 0;  // int 0 -> pin 2 on Uno
#endif
  static const uint32_t DEBOUNCE_MS = 150;
  static const int16_t MOVE_THRESHOLD = 1;  // Do not report move event unless *both* coordinates change more than this

  typedef void (*touch_cb_t)(uint16_t x, uint16_t y);
  //typedef std::function<void(uint16_t, uint16_t)> touch_cb_t;

  Touchscreen();

  void begin(touch_cb_t down_cb, touch_cb_t move_cb, touch_cb_t up_cb);
  void loop();
  
private:
  static void _touchInterrupt();  // TODO: Can't use closure, but any other way to pass non-static? Probably not..

  enum touch_state_t : uint8_t {
    STATE_IDLE,     // Waiting for first touch; then transition to TOUCHING
    STATE_TOUCHING, // Waiting for further touches; if finger goes up, transition to RELEASED
    STATE_RELEASED  // If another touch registers
  };

  touch_cb_t _down_cb, _move_cb, _up_cb;
  uint16_t _last_x, _last_y;
  volatile touch_state_t _state;
  uint16_t _last_release_time;  // based on millis()
};

extern Touchscreen Touch;  // Singleton

#if defined(DIGOLE_I2C) && DIGOLE_I2C
extern Digole::DigoleI2C LCD;
#elif defined(DIGOLE_SERIAL) && DIGOLE_SERIAL
extern Digole::DigoleSerial LCD;
#elif defined(DIGOLE_SPI) && DIGOLE_SPI
#if defined(ESP8266)
//extern Digole::DigoleSPI LCD;
extern Digole::DigoleSoftSPI LCD;
#else
extern Digole::DigoleSPI LCD;
//extern Digole::DigoleSoftSPI LCD;
#endif
#else
#error "Digole screen not configured!"
#endif

#endif  // ESPClock_Touch_h

