#ifndef ESPClock_Widget_h
#define ESPClock_Widget_h

#include <Digole.h>
#include "Font.h"

using Digole::Color;

#define _DIGOLE_KLUDGE 1

extern Color BLACK, WHITE;
extern Color DARK_GRAY, DARK_GRAY2, LIGHT_GRAY, GRAY;
extern Color LIGHT_RED, RED, LIGHT_GREEN, GREEN, LIGHT_BLUE, BLUE;

class Widget {
public:
  Widget (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  inline uint8_t getId() const { return _id; }
  inline uint16_t getX() const { return _x; }
  inline uint16_t getY() const { return _y; }
  inline uint16_t getWidth() const { return _w; }
  inline uint16_t getHeight() const { return _h; }

  inline bool contains(uint16_t x, uint16_t y) const {
    return (x >= _x) && (x < _x + _w) && (y >= _y) && (y < _y + _h);
  }

  virtual void draw() = 0;
  
private:
  uint8_t _id;
  uint16_t _x, _y, _w, _h;
};

class Label : public Widget {
public:

  static const uint8_t DEFAULT_MARGIN = 3;

  enum justification_t : uint8_t {
    LEFT, CENTER, RIGHT
  };
  enum alignment_t : uint8_t {
    TOP, MIDDLE, BOTTOM
  };

  Label (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String &label);

  void draw() override;

  inline const String& getLabel() const { return _label; }
  inline void setLabel(const String &label) { 
    _label = label; 
    _label_w = _metric->getStringWidth(label);
    _label_h = _metric->getStringHeight(label);
  }

  inline uint8_t getFont() const { return _font; }
  inline const FontMetric& getFontMetric() const { return *_metric; }
  inline void setFont(uint8_t font, const FontMetric *metric) {
    _font = font;
    _metric = metric;
    _label_w = metric->getStringWidth(_label);
    _label_h = metric->getStringHeight(_label);
  }
  
  inline const Color& getFgColor() const { return _fg; }
  inline const Color& getBgColor() const { return _bg; }
  inline void setFgColor(const Color &fg) { _fg = fg; }
  inline void setBgColor(const Color &bg) { _bg = bg; }
  inline void setColor(const Color &fg, const Color &bg) { _fg = fg; _bg = bg; }

  inline justification_t getJustification() const { return _just; }
  inline void setJustification(justification_t just) { _just = just; }
  inline alignment_t getAlignment() const { return _align; }
  inline void setAlignment(alignment_t align) { _align = align; }

  inline uint8_t getMargin() const { return _margin; }
  inline void setMargin(uint8_t margin) { _margin = margin; }

#if defined(_DIGOLE_KLUDGE) && _DIGOLE_KLUDGE
  // Kludge for Digole's shoddy API
  inline void setDrawMode(Digole::draw_mode_t draw_mode) { _draw_mode = draw_mode; }
#endif

protected:
  void _drawLabel(const String &label, const Color &fg, const Color &bg);
  
private:
  String _label;
  uint16_t _label_w;
  uint8_t _label_h;
  uint8_t _font;
  const FontMetric *_metric;
  Color _bg, _fg;  // TODO FIXME -- mangle for XOR draw mode (and adjust getter/setter methods)
#if defined(_DIGOLE_KLUDGE) && _DIGOLE_KLUDGE
  Digole::draw_mode_t _draw_mode = Digole::MODE_AND; // Workaround for Digole's shoddy API
#endif
  justification_t _just;
  alignment_t _align;
  uint8_t _margin;
};

class Button : public Label{
public:
  Button (uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String &label);

  void draw() override;

  // Pressed: permanently pressed state
  inline bool getPressed() const { return _pressed; }
  inline void setPressed(bool pressed) { _pressed = pressed; }

  // Active: when finger is hovering over button
  inline bool getActive() const { return _active; }
  inline void setActive(bool active) { _active = active; }

  inline const Color& getFgPressed() const { return _fg_pressed; }
  inline const Color& getBgPressed() const { return _bg_pressed; }
  inline void setFgPressed(const Color &fg) { _fg_pressed = fg; }
  inline void setBgPressed(const Color &bg) { _bg_pressed = bg; }
  inline void setPressedColor(const Color &fg, const Color &bg) { _fg_pressed = fg; _bg_pressed = bg; }

  inline const Color& getFgActive() const { return _fg_active; }
  inline const Color& getBgActive() const { return _bg_active; }
  inline void setFgActive(const Color &fg) { _fg_active = fg; }
  inline void setBgActive(const Color &bg) { _bg_active = bg; }
  inline void setActiveColor(const Color &fg, const Color &bg) { _fg_active = fg; _bg_active = bg; }

private:
  bool _pressed, _active;
  Color _bg_pressed, _fg_pressed;
  Color _bg_active, _fg_active;
};

#endif  // ESPClock_Widget_h

