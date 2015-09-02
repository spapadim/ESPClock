#include <Arduino.h>
#include <Digole.h>

#include "Widget.h"
#include "Screen.h"

Widget::Widget (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
: _id(id), _x(x), _y(y), _w(w), _h(h) { 
}

Label::Label (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String &label)
: Widget(id, x, y, w, h), _label(label), 
  _fg(WHITE), _bg(BLACK), _font(200), _metric(&metric_DroidMono18),
  _just(CENTER), _align(MIDDLE), _margin(DEFAULT_MARGIN) {
  _label_w = _metric->getStringWidth(label);
  _label_h = _metric->getStringHeight(label);
}


void Label::draw() {
  _drawLabel(_label, _fg, _bg);
}

void Label::_drawLabel(const String &label, const Color &fg, const Color &bg) {
  LCD.setDrawMode(Digole::MODE_COPY);
  LCD.setColor(bg);
  LCD.drawRect(getX(), getY(), getWidth(), getHeight(), true);
  uint16_t lbl_x;
  switch(_just) {
  case LEFT:
    lbl_x = getX() + _margin;
    break;
  case RIGHT:
    lbl_x = getX() + getWidth() - _label_w - _margin;
    break;
  case CENTER:
  default:
    lbl_x = getX() + (getWidth() - _label_w)/2;
    break;
  }
  uint16_t lbl_y;
  switch (_align) {
  case TOP:
    lbl_y = getY() + _label_h + _margin;
    break;
  case BOTTOM:
    lbl_y = getY() + getHeight() - _margin;
    break;
  case MIDDLE:
  default:
    lbl_y = getY() + (getHeight() + _label_h)/2;
    break;
  }
  LCD.setTextPosition(max(0, lbl_x), max(0, lbl_y), Digole::PIXEL);
  LCD.setFont(_font);
#if defined(_DIGOLE_KLUDGE) && _DIGOLE_KLUDGE
  LCD.setDrawMode(_draw_mode); // Digole API sucks!
#else
  LCD.setBackgroundColor();
#endif
  LCD.setColor(fg);
  LCD.print(_label);
}

Button::Button (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String &label) 
: Label(id, x, y, w, h, label), _pressed(false), _active(false) { 
  setColor(BLACK, DARK_GRAY);
  setPressedColor(BLACK, GRAY);
  setActiveColor(BLACK, LIGHT_GRAY);
}

void Button::draw() {
  if (_active) {
    _drawLabel(getLabel(), getFgActive(), getBgActive());
  } else if (_pressed) {
    _drawLabel(getLabel(), getFgPressed(), getBgPressed());
  } else {
    _drawLabel(getLabel(), getFgColor(), getBgColor());    
  }
}


