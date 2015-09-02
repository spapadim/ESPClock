##  Droid Sans
otf2bdf -p 14 DroidSans.ttf > DroidSans_14.bdf
bdf2u8g -b 32 -e 126 DroidSans_14.bdf DroidSans14 DroidSans14.h

otf2bdf -p 16 DroidSans.ttf > DroidSans_16.bdf
bdf2u8g -b 32 -e 126 DroidSans_16.bdf DroidSans16 DroidSans16.h


##  Droid Mono
otf2bdf -p 14 DroidSansMono.ttf > DroidMono_14.bdf
bdf2u8g -b 32 -e 126 DroidMono_14.bdf DroidMono14 DroidMono14.h

otf2bdf -p 16 DroidSansMono.ttf > DroidMono_16.bdf
bdf2u8g -b 32 -e 126 DroidMono_16.bdf DroidMono16 DroidMono16.h

otf2bdf -p 18 DroidSansMono.ttf > DroidMono_18.bdf
bdf2u8g -b 32 -e 126 DroidMono_18.bdf DroidMono18 DroidMono18.h


##  Droid Sans & Mono -- large, numbers only
# NOTE: The bitmap for each glyph must fit within 255 bytes, so
#   sizes > 50 are too big for the U8G format.
otf2bdf -p 50 DroidSansMono.ttf > DroidMono_50.bdf
bdf2u8g -b 48 -e 58 DroidMono_50.bdf DroidMono50 DroidMono50.h

otf2bdf -p 50 DroidSans.ttf > DroidSans_50.bdf
bdf2u8g -b 48 -e 58 DroidSans_50.bdf DroidSans50 DroidSans50.h

