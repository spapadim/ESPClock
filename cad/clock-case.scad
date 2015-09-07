
part = "case";  // [case,endcaps|caps,ribs,clip]

thk = 2.5;

r = 10;
side = 55 - 2*r;
h = 85;

curved = true;
curv_ofs = 25;  // larger: less curved

rib_thk = 2.5;  // height of separate glue-on corner ribs

inch = 25.4;

screw_r = 2.9/2;  // M3 tap
nut_r = 6.1/2;  // M3 corner-to-corner
nut_h = 2.25;  // M3 normal
//nut_hjam = 1.8;  // M3 jam nut
screw_scr_r = 1.8/2;  // M2 tap
nut_scr_r = 4.35/2; // M2 corner-to-corner
nut_scr_h = 1.6; // M2 normal
standoff_thk = 1.4; //1.6;
standoff_rtc_thk = 1;
standoff_h = 4;

foot_h = 1;
foot_d = 5;

cap_thk = thk;
cap_slack = 0.2; // PLA/Kossel: -0.1;
cap_notch_w = 6;  // 7;
cap_notch_d = 2.5;  // 2.5;
cap_notch_thk = 2*thk/3;

// SparkFun Thing dimensions

thing_standoff_pos =  [ [0.475, 0.1],  [0.475, 0.9],
                        [1.725, 0.1],  [1.725, 0.9] ] * inch;
thing_xlen = 2.2 * inch;
thing_ylen = 1.0 * inch;
thing_usb_mid_yofs = 0.3 * inch;
thing_usb_edge_xofs = 0;
thing_pcb_thk = 1.55;

// Digole 2.4" IPS LCD dimensions
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
lcd_header_xlen = 3;
lcd_header_h = lcd_scr_h - 2.1;

// RTC dimensions
rtc_standoff_pos = [ [0.350, 0.075], [1.350, 0.075], [0.350, 0.775] ] * inch; // M2
rtc_xlen = 1.5 * inch;
rtc_ylen = 0.85 * inch;


// LiPo battery dimensions (500mAh, Adafruit or eBay)
bat_w = 29.8 + 0.6;
bat_thk = 5 + 0.25;
bat_l = 35.5;  // not needed
bat_clip_w = 7.5;

usb_micro_thk = 0.5;
usb_slack = 0.35;
module usb_micro_inner_profile() {
  // 0.75 is eyeballed
  polygon([[-5.40/2, 0], [5.40/2, 0], [6.90/2, 0.75],
           [6.90/2, 1.85], [-6.90/2, 1.85], [-6.90/2, 0.75]]);
}

module usb_micro(h, slack = 0) {
  translate([0, usb_micro_thk, 0])
    linear_extrude(height = h) minkowski() {
      usb_micro_inner_profile();
      circle(r = usb_micro_thk + slack, $fn=12);
    }
}


module shell(side, r, h) {
  hull() {
    // Radius and angle of curvature arc
    curv_r = sqrt(side*side + curv_ofs*curv_ofs + sqrt(3)*side*curv_ofs) + r;
    curv_arc_angle = atan(side/(sqrt(3)*side + 2*curv_ofs));
    
    // Curvature circle center (x0, y0)
    x0 = -side/2 - curv_ofs*cos(30);
    y0 = -curv_ofs*sin(30);
    
    // Slice coordinates
    x1 = x0 + curv_r*cos(30-curv_arc_angle);
    y1 = y0 + curv_r*sin(30-curv_arc_angle);
    x2 = x0 + curv_r*cos(30+curv_arc_angle);
    y2 = y0 + curv_r*sin(30+curv_arc_angle);
    
    
    translate([-side/2, 0, 0]) cylinder(r = r, h = h, $fn = 56);
    translate([side/2, 0, 0]) cylinder(r = r, h = h, $fn = 56);
    translate([0, side*cos(30), 0]) cylinder(r = r, h = h, $fn = 56);
    if (curved)
      intersection () {
        translate([x0, y0, 0]) 
          cylinder (r = curv_r, h = h, $fn=36*7);
        linear_extrude(h)
          polygon([[-side/2, 0], [2*x1 + side/2, 2*y1], [2*x2 + side/2, 2*y2]]);
      }
  }
}

//rotate([90, 0, 0]) shell(side, r, h);
//mirror([-1, 0, 0]) shell();

// Virtual jig:
*linear_extrude(120)
  polygon([[-side/2, 0], [side/2, 0], [0, side*cos(30)]]);

// Bring an object to screen position
module scr_position(width) {  // TODO: add rotational displacement?
  curv_r = sqrt(side*side + curv_ofs*curv_ofs + sqrt(3)*side*curv_ofs) + (r + thk);
  chord_dist = sqrt(curv_r*curv_r - width*width/4);
  rofs_adj = chord_dist - curv_ofs - sqrt(3)*side/2 - (r + thk);
  
  zofs = (h - lcd_touch_xlen) / 2 - lcd_touch_xofs;
  rofs = tan(30)*side/2 + r + thk + (curved ? rofs_adj : 0);
  
  angle = 30;  // FIXME: Not properly/consistently used below?
  
  translate([cos(angle)*rofs, sin(angle)*rofs, 0])
    translate([0, tan(30)*side/2, 0])  // move center to epicenter
      rotate([0, 0, angle])  // bring parallel to side
        translate([0, 0, zofs])  // center touch area on face
          children(0);
}

module standoff_cylinder(r1, r2, h, truncated = false) {
  rmax = max(r1,r2);
  rmin = min(r1,r2);
  difference() {
    cylinder(r1 = r1, r2 = r2, h = h);
    if (truncated)  // TODO: Fix nut slightly poking through, if truncated
      translate([-rmax-1, rmin, -1])
        cube([2*rmax+2, rmax+1, h+2]);
  }
}

// TODO(no; will have visible holes on back): Add thing standoffs for mounting USB on other side, as well
module case(feet = false, rtc = true, ribs = true) {
  face_thk = 1.6; //0.8;

  lcd_yext = 6.25;  // yext==6.25mm leaves ~3.2mm for cap, when h==85mm
  lcd_xext = 3;
  
  // Wall
  difference() {
    union() {
      // Main wall
      difference() {
        shell(side, r + thk, h);
        translate([0, 0, -1]) shell(side, r, h+2);
      }
      // Screen mount fill-in
      intersection() {
        shell(side, r + thk, h);
        scr_position(width = lcd_ylen)
           translate([-lcd_scr_h - face_thk, -lcd_ylen/2-lcd_yext/2, -lcd_xext/2])  // move center to origin
            cube([lcd_scr_h + face_thk + 20, lcd_ylen+lcd_yext, lcd_xlen+lcd_xext]); // TODO: properly adj x for curved
      }
      // Thing standoff positives
      for (pos_xy = thing_standoff_pos)
        translate([pos_xy[1] - thing_ylen/2, -r + standoff_h, thk + pos_xy[0]])
          rotate([90, 0, 0])
            standoff_cylinder(r1 = screw_r + standoff_thk, 
                     r2 = screw_r + standoff_thk + standoff_h, // 45deg slope
                     h = standoff_h, $fn=24);
      // RTC standoff positives
      if (rtc) {
        translate([-cos(30)*(tan(30)*side/2+r), tan(30)*side/2 + sin(30)*(tan(30)*side/2+r), 0]) rotate([0, 0, -120])  // Move to back face; ugly last-minute hack!
        for (pos_xy = rtc_standoff_pos)  
          translate([rtc_ylen/2 - pos_xy[1], standoff_h, h - (2*thk + pos_xy[0])])
            rotate([90, 0, 0])
              standoff_cylinder(r1 = screw_scr_r + standoff_rtc_thk, 
                       r2 = screw_scr_r + standoff_rtc_thk + standoff_h, // 45deg slope
                       h = standoff_h, $fn=24);          
      }
      
      // Integrated ribs (optional) to support endcaps
      if (ribs) {
        for (a = [0, 120, 240])
          translate([0, sqrt(3)*side/6, h/2])
            rotate([0, 0, a])
              translate([0, sqrt(3)*side/3 + r - rib_thk, 0])
                rotate([-90, 90, 0])
                  rib();
      }
      
    }
      
    // Screen cutout and standoffs
    scr_position(width = lcd_ylen) union() {
      // Full coutout for touch area
      translate([-lcd_scr_h - face_thk - 1, -lcd_touch_ylen/2, lcd_touch_xofs])
        cube([lcd_scr_h + face_thk + thk + 20, lcd_touch_ylen, lcd_touch_xlen]);  // TODO: properly adj x
      // Partial cutout for screen module
      translate([-lcd_scr_h - face_thk - 1, -lcd_scr_ylen/2, lcd_scr_xofs]) {
        cube([lcd_scr_h + 1, lcd_scr_ylen, lcd_scr_xlen]);
        // Ribbon cable extra cutout
        translate([0, lcd_ribbon_yofs, 0]) 
          cube([lcd_scr_h + 1, lcd_ribbon_ylen, lcd_scr_xlen + lcd_ribbon_xlen]);
        // MCU pin header extra cutout
        translate([0, lcd_header_yofs, -lcd_header_xlen]) 
          cube([lcd_header_h + 1, lcd_header_ylen, lcd_scr_xlen + lcd_header_xlen]);
        // PenIRQ pin extra cutout; TODO: name constants 3 and 3.5 below
        translate([0, lcd_ribbon_yofs, 0]) 
          cube([lcd_header_h + 1, 3, lcd_scr_xlen + lcd_ribbon_xlen + 3.5]);
      }
    }
    // Screen mounting holes
    scr_position(width = lcd_ylen)
      for (pos_xy = lcd_standoff_pos)
        translate([-lcd_scr_h-face_thk-1, -lcd_ylen/2 + pos_xy[1], pos_xy[0]])
          rotate([0, 90, 0]) {
            cylinder(r = screw_scr_r, h = lcd_scr_h + face_thk + 1, $fn=24);
            translate([0, 0, lcd_scr_h + face_thk + 1 - 0.05 - nut_scr_h])
              rotate([0, 0, 30])  
                cylinder(r = nut_scr_r, h = 10, $fn=6);
          }
    // Thing standoff negatives
    for (pos_xy = thing_standoff_pos)

      translate([pos_xy[1] - thing_ylen/2, -r + standoff_h, cap_thk + pos_xy[0]])
        rotate([90, 0, 0]) {
          translate([0, 0, -1]) cylinder(r = screw_r, h = standoff_h+thk+2, $fn=24);
          translate([0, 0, standoff_h+thk-(nut_h + 0.2)]) rotate([0, 0, 30]) cylinder(r = nut_r, h = (nut_h+0.2)+1, $fn=6);  // TODO: name 0.2 (see also below)

        }
    // RTC standoff negatives
    if (rtc) {
        translate([-cos(30)*(tan(30)*side/2+r), tan(30)*side/2 + sin(30)*(tan(30)*side/2+r), 0]) rotate([0, 0, -120])  // Move to back face; ugly last-minute hack!
      for (pos_xy = rtc_standoff_pos)  
        translate([rtc_ylen/2 - pos_xy[1], standoff_h, h - (2*thk + pos_xy[0])])
          rotate([90, 0, 0]){
            translate([0, 0, -1]) cylinder(r = screw_scr_r, h = standoff_h+thk+2, $fn=24);
            translate([0, 0, standoff_h+thk-(nut_scr_h+0.6)]) rotate([0, 0, 30]) cylinder(r = nut_scr_r, h = (nut_scr_h+0.6)+1, $fn=6);  // TODO: name 0.6 constant (extra sink to allow covering nuts with putty)
          }
      }

  }
  
  if (feet) {
    for (s = [-1,+1], t = [-1,+1])
      translate([s*(side/2 - foot_d/2 - thk), -r-thk-foot_h+0.01, h/2 + t*(h/2 - foot_d/2 - thk)])
        rotate([-90, 0, 0])
          cylinder(r1 = foot_d/2, r2=foot_d/2 + foot_h, h = foot_h, $fn=24);
  }
            
  // Side rings; TODO: Slant-in & Move out of the way of LCD mounting holes
  *for (z = [thk, h-2*thk]) 
    translate([0, 0, z]) 
      difference() {
        shell(side, r + 0.1, thk);
        translate([0, 0, -1]) shell(side, r - thk/2, thk+2);
      }
}

module endcap(usb = false, notch = false) {
  difference () {
    shell(side, r - cap_slack, thk);
    if (usb) {
      // USB cutout
      translate([thing_usb_mid_yofs - thing_ylen/2, -r + standoff_h + thing_pcb_thk, -1])
        usb_micro(thk+2, slack = usb_slack);
    }
    if (notch) {
      // Finger notch for easier removal
      translate([-cos(30)*(tan(30)*side/2 + r - cap_notch_d), tan(30)*side/2 + sin(30)*(tan(30)*side/2 + r - cap_notch_d), 0])
        rotate([0, 0, 60])
          translate([-cap_notch_w/2, 0, -1])
            cube([cap_notch_w, cap_notch_d + 1, cap_notch_thk + 1]);
    }
  }
}

module rib() {
  intersection() {
    translate([0, 0, -r + rib_thk]) 
      rotate([0, 90, 0])
        cylinder(r = r, h = h - 2*thk, center = true, $fn=36+18);
      translate([-h/2, -r-1, 0])
        cube([h, 2*r+2, r+1]);
  }
}

module fillet(h, r) {
  difference() {
     cube([h, r, r]);
     translate([-0.1, r, r]) rotate([0, 90, 0]) cylinder(h = h+.2, r = r);
  }
}


module battery_clip() {
  toe_l = 0.66*thk;  // Anything more may collide with ribs
  cr = 1;
  difference() {
    union() {
      translate([0, -bat_w/2 - thk]) 
        cube([bat_thk + thk, bat_w + 2*thk, bat_clip_w]);
      translate([0, -bat_w/2 - thk - toe_l])
        cube([thk, bat_w + 2*thk + 2*toe_l, bat_clip_w]);
    }
    translate([-1, -bat_w/2, -1])
      cube([bat_thk + 1, bat_w, bat_clip_w + 2]);
    for (i = [-1,+1]) {
      translate([bat_thk + thk + 0.01, i*(bat_w/2 + thk + 0.01), -1])
        rotate([0, -90, (i+1)*45])
          fillet(r = cr + thk, h = bat_clip_w + 2, $fn=32);
      translate([-0.01, i*(bat_w/2 - 0.01), -1])
        rotate([0, -90, (i+1)*45 + 180])
          fillet(r = cr, h = bat_clip_w + 2, $fn=16);
    }
  }
  for (i = [-1,+1]) {
    translate([bat_thk + 0.01, i*(bat_w/2 + 0.01), 0])
      rotate([0, -90, (i+1)*45])
        fillet(r = cr, h = bat_clip_w, $fn=16);
    translate([thk - 0.01, i*(bat_w/2 + thk - 0.01), 0])
      rotate([0, -90, (i+1)*45 + 180])
        fillet(r = 0.4, h = bat_clip_w, $fn=16);
  }
}

if (part == "case") {
  case(feet = false, ribs = false);
} else if (part == "endcaps" || part == "caps") {
  //translate([+0.5*side, 0, 0]) case();
  translate([+0.5*side, 0, thk]) rotate([0, 180, 120]) endcap(usb = true, notch = true);
  translate([-0.75*side, 0.9*side, 0]) rotate([0, 0, 180]) endcap();
} else if (part == "ribs") {
  for (i = [-1,0,+1])
    translate([0, i*(2*sqrt(rib_thk*(2*r-rib_thk)) + thk), 0])
      rib();
} else if (part == "clip") {
  battery_clip();
}
