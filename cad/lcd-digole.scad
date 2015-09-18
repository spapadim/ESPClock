inch = 25.4;

// Digole 2.4" IPS LCD dimensions
lcd_model = "digole";
lcd_standoff_pos = [ [ 2.0, 3.7], [ 2.0, 39.3],
                     [67.8, 3.7], [67.8, 39.3] ];  // M2
lcd_xlen = 70.0;
lcd_ylen = 43.0;
lcd_scr_xlen = 60 + 0; // +2 slack for ribbon cable moved to separate cutout
lcd_scr_ylen = 43; // 42.5 ?  (match with _yofs)
lcd_scr_xofs = 5;
lcd_scr_yofs = 0; // 0.25 ?  (match with _ylen; also fix zero assumptions in code)
lcd_touch_xlen = 49.0;
lcd_touch_ylen = 36.8;
lcd_touch_xofs = 7.8;
lcd_touch_yofs = 3.4;
lcd_scr_h = 3.35;

// Ribbon cable cutout
lcd_ribbon_yofs = 24.5;
lcd_ribbon_ylen = 11;
lcd_ribbon_xlen = 2;

lcd_header_yofs = 0.6 * inch;  // approximate?
lcd_header_ylen = 0.6 * inch;
lcd_header_xlen = 3 + 0.1;
lcd_header_scr_xofs = 0.1;
lcd_header_h = lcd_scr_h - 2.1;

lcd_yext = 6.25;  // yext==6.25mm leaves ~3.2mm for cap, when h==85mm
lcd_xext_top = 2.5;
lcd_xext_bottom = 1.5;

screw_scr_r = 1.8/2;  // M2 tap
nut_scr_r = 4.35/2; // M2 corner-to-corner
nut_scr_h = 1.6; // M2 normal
