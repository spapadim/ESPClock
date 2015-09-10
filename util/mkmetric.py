#! /usr/bin/env python2
#
# Simple script to parse bdf2u8g output and create font metric
# tables (needed to compute text dimensions, which the Digole
# display doesn't support)

import sys

font_name = sys.argv[1]

print "static uint8_t _data_%s[] = {" % font_name

ic = 0
for l in sys.stdin:
  l = l.strip()
  if l.startswith("bbx "):
    l = l[len("bbx "):l.find(" encoding")]
    w, h, dx, dy = [int(i) for i in l.split()]
    print ("%3d, %2d, %2d," % (w, h+dy, dx)),
    ic += 1
    if (ic % 4 == 0):
      print
  elif l.startswith("encoding range "):
    l = l[len("encoding range "):]
    start, end = [int(i) for i in l.split("..")]
print "};"

print
print "static FontMetric metric_%s(_data, %d, %d)" % (font_name, (end-start+1), start)

