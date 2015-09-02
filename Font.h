#ifndef ESPClock_Font_h
#define ESPClock_Font_h

#include <Arduino.h>
#include <inttypes.h>

#define _USE_PROGMEM

class FontMetric {
public:
  FontMetric (const uint8_t *data, uint8_t len, uint8_t ofs = 0);

  inline uint8_t getCharWidth(char c) const {
    uint16_t i = (uint16_t)c - (uint16_t)_ofs;
#ifdef _USE_PROGMEM
    return (i < _len) ? pgm_read_byte_near(_data + i*3 + 0) : 0;
#else
    return (i < _len) ? _data[i*3 + 0] : 0;
#endif
  }
  inline uint8_t getCharHeight(char c) const {
    // This is really only the ascender part, to save a bit of space
    uint16_t i = (uint16_t)c - (uint16_t)_ofs;
#ifdef _USE_PROGMEM
    return (i < _len) ? pgm_read_byte_near(_data + i*3 + 1) : 0;
#else
    return (i < _len) ? _data[i*3 + 1] : 0;
#endif
  }
  inline int8_t getCharXOffset(char c) const {
    uint16_t i = (uint16_t)c - (uint16_t)_ofs;
#ifdef _USE_PROGMEM
    return (i < _len) ? pgm_read_byte_near(_data + i*3 + 2) : 0;
#else
    return (i < _len) ? (int8_t)_data[i*3 + 2] : 0;
#endif
  }

  uint16_t getStringWidth(const char *s) const;
  uint8_t getStringHeight(const char *s) const;

  inline uint16_t getStringWidth(const String &s) const { return getStringWidth(s.c_str()); }
  inline uint8_t getStringHeight(const String &s) const { return getStringHeight(s.c_str()); }
  
private:
  uint8_t _ofs, _len;
  const uint8_t *_data;
};

extern FontMetric metric_DroidMono18;
extern FontMetric metric_DroidMono50;

#endif // ESPClock_Font_h

