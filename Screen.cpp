#include <Arduino.h>
#include <Wire.h>
#include <Digole.h>

#include "Config.h"
#include "Screen.h"
#include "Widget.h"  // for WHITE

#if defined(DIGOLE_I2C) && DIGOLE_I2C
Digole::DigoleI2C LCD(Wire, 0x27);
#elif defined(DIGOLE_SERIAL) && DIGOLE_SERIAL
Digole::DigoleSerial LCD(Serial, 115200, DISPLAY_RESET_PIN);
#elif defined(DIGOLE_SPI) && DIGOLE_SPI
#  if defined(ESP8266)
Digole::DigoleSoftSPI LCD(4, 13, 12, 14);
//Digole::DigoleSPI LCD(4, 13);
#  else
//Digole::DigoleSoftSPI LCD(10, 12, 11, 13);
Digole::DigoleSPI LCD(10, 12);
#  endif
#endif

Touchscreen Touch;

// We can't use Serial in UART mode, might as well use screen itself
void lcdDebugPrintf(int16_t y, const char *fmt, ...) {
  va_list arg;
  int res;
  char buf[100];

  va_start(arg, fmt);
  vsnprintf(buf, sizeof(buf), fmt, arg);
  va_end(arg);
  
  LCD.setFont(200);
  LCD.setTextPosition(0, y);
  LCD.setDrawMode(Digole::MODE_COPY);
  LCD.setColor(WHITE);
  LCD.print(buf);
}

Touchscreen::Touchscreen()
: _down_cb(0), _move_cb(0), _up_cb(0), 
  _last_x(0xffff), _last_y(0xffff), 
  _state(STATE_IDLE), _last_release_time(0) {
  
}

void Touchscreen::begin(touch_cb_t down_cb, touch_cb_t move_cb, touch_cb_t up_cb) {
  _down_cb = down_cb;
  _move_cb = move_cb;
  _up_cb = up_cb;

  attachInterrupt(INT_PIN, _touchInterrupt, FALLING);
}

void Touchscreen::loop() {
  if (_state == STATE_IDLE)
    return;

  uint16_t t_now = millis();
  uint16_t x, y;
  LCD.readTouchscreen(x, y, Digole::TOUCH_DOWN_NONBLOCKING);

#if !defined(DIGOLE_SERIAL) || !DIGOLE_SERIAL
  Serial.println(); Serial.print("POS "); Serial.print(x); Serial.print(','); Serial.println(y);  // DEBUG
#endif

  // CAUTION: Docs claim no-touch returns 0xffff, 0xffff,
  //   but that does not seem to be the case
  
  switch (_state) {
  case STATE_RELEASED:
    if (x > 0xff00 || y > 0xff00) {
      if (t_now - _last_release_time > DEBOUNCE_MS) {
        if (_up_cb)
          _up_cb(_last_x, _last_y);
        _state = STATE_IDLE;
        _last_x = _last_y = 0xffff;
      }
      break; 
    } else {
      _state = STATE_TOUCHING;
      // Do not break; fall through next case
    }
  case STATE_TOUCHING:
    if (x> 0xff00 || y > 0xff00) {
      _state = STATE_RELEASED;
      _last_release_time = t_now;
    } else {
      if (_last_x == 0xffff && _last_y == 0xffff) {
        if (_down_cb)
          _down_cb(x, y);
      } else if (_last_x != x || _last_y != y) {
        if (_move_cb && 
            (abs((int16_t)x - (int16_t)_last_x) > MOVE_THRESHOLD && 
             abs((int16_t)y - (int16_t)_last_y) > MOVE_THRESHOLD))
          _move_cb(x,y);
      }
      _last_x = x;
      _last_y = y;
    }
    break;
  }
}

void Touchscreen::_touchInterrupt() {
  //Serial.print('T');  // DEBUG
  if (Touch._state != STATE_IDLE)
    return;
  Touch._state = STATE_TOUCHING;
}

