#include "Font.h"
#include "Widget.h"

#include "Config.h"


/**************************************
 * Colors
 */

Color BLACK(0x00, 0x00, 0x00);
Color WHITE(0xff, 0xff, 0xff);

Color GRAY(0x99, 0x99, 0x99);
Color DARK_GRAY(0x4f, 0x4f, 0x4f);
Color LIGHT_GRAY(0xbf, 0xbf, 0xbf);
Color LIGHT_RED(0xf2, 0x7a, 0x7a);
Color RED(0xcc, 0x00, 0x00);
Color LIGHT_GREEN(0x8d, 0xeb, 0x8d);
Color GREEN(0x00, 0x99, 0x00);
Color LIGHT_BLUE(0x91, 0xba, 0xf2);
Color BLUE(0x0f, 0x3b, 0x99);


/**************************************
 * Font metrics
 */


#ifdef _USE_PROGMEM
static const uint8_t _data_DroidMono18[] PROGMEM = {
#else
static const uint8_t _data_DroidMono18[] = {
#endif
   0,  0,  0,   3, 18,  6,   9, 18,  3,  13, 18,  1,
  11, 19,  2,  15, 18,  0,  14, 18,  1,   3, 18,  6,
   7, 18,  4,   7, 18,  4,  11, 19,  2,  12, 15,  2,
   4,  3,  5,   9,  8,  3,   4,  4,  6,   9, 18,  3,
  11, 18,  2,   6, 18,  3,  11, 18,  2,  11, 18,  2,
  13, 18,  1,  11, 18,  2,  11, 18,  2,  11, 18,  2,
  11, 18,  2,  11, 18,  2,   3, 13,  6,   4, 13,  5,
  11, 15,  2,  11, 12,  2,  11, 15,  2,  11, 18,  2,
  15, 18,  0,  15, 18,  0,  12, 18,  2,  12, 18,  2,
  12, 18,  2,  10, 18,  3,  10, 18,  3,  12, 18,  1,
  11, 18,  2,  10, 18,  2,  10, 18,  2,  12, 18,  3,
  10, 18,  3,  13, 18,  1,  11, 18,  2,  13, 18,  1,
  11, 18,  2,  13, 18,  1,  12, 18,  2,  11, 18,  2,
  12, 18,  1,  11, 18,  2,  15, 18,  0,  15, 18,  0,
  13, 18,  1,  14, 18,  0,  13, 18,  1,   6, 18,  5,
   9, 18,  3,   6, 18,  4,  13, 18,  1,  15,  0,  0,
   5, 19,  5,  11, 13,  2,  11, 19,  2,  11, 13,  2,
  11, 19,  2,  11, 13,  2,  12, 19,  2,  13, 13,  1,
  11, 19,  2,  11, 19,  3,   8, 19,  2,  11, 19,  3,
  11, 19,  3,  13, 13,  1,  11, 13,  2,  13, 13,  1,
  11, 13,  2,  11, 13,  2,  10, 13,  3,  10, 13,  3,
  10, 17,  2,  11, 13,  2,  13, 13,  1,  15, 13,  0,
  13, 13,  1,  13, 13,  1,  11, 13,  2,   9, 18,  3,
   2, 19,  7,   9, 18,  3,  11, 10,  2
};

FontMetric metric_DroidMono18(_data_DroidMono18, 95, 32);

#ifdef _USE_PROGMEM
static const uint8_t _data_DroidMono50[] PROGMEM = {
#else
static const uint8_t _data_DroidMono50[] = {
#endif
  31, 50,  5,  18, 49,  7,  31, 50,  5,  31, 50,  4,
  35, 49,  2,  29, 49,  6,  31, 50,  5,  31, 49,  5,
  31, 50,  5,  31, 50,  5,   9, 38, 16
};

FontMetric metric_DroidMono50(_data_DroidMono50, 11, 48);


/**************************************
 * Application-specific
 */

Label lbl_time(ID_TIME, 4, 76 + 32, 206, 108 - 32, "--:--");
Label lbl_date(ID_DATE, 4, 76 + 108, 206, 160-108, "---, --- --");

Label lbl_splash_top(ID_SPLASH, 4, 4, 312, 116, "Hello!");
Label lbl_splash_bottom(ID_SPLASH, 4, 120, 312, 116, "");

Button btn_on(ID_ON, 4, 4, 100, 64, "On");
Button btn_preset(ID_PRESET, 110, 4, 100, 64, "Scene");
Button btn_off(ID_OFF, 216, 4, 100, 64, "Off");

Button btn_dev0(ID_DEV0, 216, 76, 100, 48, "1");
Button btn_dev1(ID_DEV1, 216, 132, 100, 48, "2");
Button btn_dev2(ID_DEV2, 216, 188, 100, 48, "3");

Button *buttons[] = { &btn_on, &btn_preset, &btn_off, &btn_dev0, &btn_dev1, &btn_dev2, 0 };

 

