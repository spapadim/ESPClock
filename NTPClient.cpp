// Code based on the NTP client example for ESP8266WiFi library

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>

#include "Settings.h"
#include "NTPClient.h"

NTPClientClass NTPClient;

void NTPClientClass::begin(ntp_cb_t cb) {
  _cb = cb;
}

void NTPClientClass::loop() {
  if (_state == STATE_IDLE)
    return;

  if (!_udp.parsePacket()) {
    if (now() - _req_time > NTP_TIMEOUT) {
      _state = STATE_IDLE;
      _req_time = 0;
      _udp.stop();
    }
    return;
  }

  // We got a response
  _udp.read(_buf, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  uint32_t highWord = word(_buf[40], _buf[41]);
  uint32_t lowWord = word(_buf[42], _buf[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  uint32_t secsSince1900 = highWord << 16 | lowWord;
  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  // subtract seventy years:
  time_t utc_epoch = secsSince1900 - 2208988800UL;
  time_t local_epoch = utc_epoch + Settings.utc_offset;

  _udp.stop();
  _req_time = 0;
  _state = STATE_IDLE;

  if (_cb) {
    _cb(local_epoch);
  }
}

void NTPClientClass::startRequest() {
  if (_cb == 0)
    return;  // Don't bother, nobody will get it anyway

  if (_state != STATE_IDLE)
    return;  // Don't submit more than one request at a time

  _udp.begin(LOCAL_PORT);

  memset(_buf, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  _buf[0] = 0b11100011;   // LI, Version, Mode
  _buf[1] = 0;     // Stratum, or type of clock
  _buf[2] = 6;     // Polling Interval
  _buf[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _buf[12]  = 49;
  _buf[13]  = 0x4E;
  _buf[14]  = 49;
  _buf[15]  = 52;

  _udp.beginPacket(Settings.ntp_hostname, NTP_PORT);
  _udp.write(_buf, NTP_PACKET_SIZE);
  _udp.endPacket();

  _req_time = now();
  _state = STATE_PENDING;
}

