#include <Arduino.h>

#include "Font.h"



FontMetric::FontMetric (const uint8_t *data, uint8_t len, uint8_t ofs)
: _data(data), _len(len), _ofs(ofs) {
}


uint16_t FontMetric::getStringWidth(const char *s) const {
  if (!*s)
    return 0;

  int16_t w = 0;
  int8_t gap = -getCharXOffset(*s);
  for (; *s; ++s) {
    w += getCharXOffset(*s) + getCharWidth(*s);
  }
  return (uint16_t)w;
}

uint8_t FontMetric::getStringHeight(const char *s) const {
  uint8_t h = 0;
  for (; *s; ++s) {
    h = max(h, getCharHeight(*s));
  }
  return (uint16_t)h;
}

