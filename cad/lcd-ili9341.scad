inch = 25.4;

// ILI9341 2.4" generic touch TFT (TJCTM24024-SPI) dimensions
lcd_model="ili9341";
// Caliper measurements:
// lcd_standoff_pos = [ [ 0.1, 0.1], [ 0.1, 1.55],
//                      [2.75, 0.1], [2.75, 1.55] ] * inch;  // M3
// From http://datasheets.gpio.dk/dl/2.4inch-tft-touch/ :
lcd_standoff_pos = [ [ 3.0, 3.0], [ 3.0, 39.7],
                     [70.3, 3.0], [70.3, 39.7] ];  // M3
lcd_xlen = 77.15;
lcd_ylen = 43.0;
lcd_scr_xlen = 60.5 + 0; // +2 slack for ribbon cable moved to separate cutout
lcd_scr_ylen = 43; // 42.5 ?  (match with _yofs)
lcd_scr_xofs = 6.4;
lcd_scr_yofs = 0; // 0.25 ?  (match with _ylen; also fix zero assumptions in code)
lcd_touch_xlen = 50.0;
lcd_touch_ylen = 37.6;
lcd_touch_xofs = 8.8;
lcd_touch_yofs = 2.7;
lcd_scr_h = 3.85;  // TODO - double-check

// Ribbon cable cutout
lcd_ribbon_yofs = 8.3;
lcd_ribbon_ylen = 11;
lcd_ribbon_xlen = 2;

lcd_header_yofs = 0.15 * inch;  // approximate?
lcd_header_ylen = 1.4 * inch;
lcd_header_xlen = 0.1 * inch + 0.5;  // +0.5 to avoid overhang
lcd_header_scr_xofs = lcd_scr_xlen + 7 + lcd_header_xlen;  // Relative to screen top
lcd_header_h = 1.8;

lcd_yext = 6.25;  // yext==6.25mm leaves ~3.2mm for cap, when h==85mm
lcd_xext_top = 3;
lcd_xext_bottom = 0;

screw_scr_r = 2.9/2;  // M3 tap
nut_scr_r = 6.1/2;  // M3 corner-to-corner
nut_scr_h = 2.25;  // M3 normal

