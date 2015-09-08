ESP Clock
=========

*Touchscreen clock and light controller with WiFi*

[![ESPClock video](http://i.imgur.com/RyLw0kk.png)](https://youtu.be/hKBLLkvEEXE)

This was a week-long hack, to build a simple touchscreen clock, with the following features:
* Graphical UI with touch (no buttons)
* Clock synchronization over NTP
* Ability to control WiFi-connected LED lamps
* Web-based configuration UI

This project was partly inspired by the Chumby (remember that?) and by our old X10 light controller (remember *those*!?).  It probably costs the same as a used Chumby (and does less), but it's more fun this way. :)  In retrospect, the cost could also be taken down to $13.


#### Dependencies
* [My rewrite](https://github.com/spapadim/Digole) of the Digole library
* [ArduinoJson](http://github.com/bblanchon/ArduinoJson)
* [Time](http://www.pjrc.com/teensy/td_libs_Time.html)
* Arduino ESP built-in libraries (WiFi, WebServer)

Also, for flashing fonts, you will need the original DigoleSerial library (haven't tested/ported the font upload stuff in my rewrite) and, if you want to substitute your own fonts, the font conversion tools from u8glib/ucglib.

#### BOM
* [Sparkfun Thing](https://www.sparkfun.com/products/13231)
* [Digole 2.4" IPS display](http://digole.com/index.php?productID=1208) (in UART mode, with wire soldered to reset pad)
* 500mAh LiPo (optional, but recommended)
* Depending how fancy you want to get with print finishing: XTC-3D resin and/or Bondo putty, spray paint and primer

More details can be found in the [making-of post](http://bitquill.net/blog/esp8266-light-controller-clock).

#### TODO
This is a tentative list of ideas that could be implemented, time permitting:
* Extend web UI to control lights as well
* Sort out the AP->AP+STA->STA transitions and make initial setup work more smoothly (now it's either AP or STA, until reboot)
* Weather info display (e.g., via OpenWeatherMap, which returns a reasonable-size response?)
* Expose more settings in web UI (?)
* OTA reflash and more modular architecture

