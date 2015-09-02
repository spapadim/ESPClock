#ifndef ESPClock_NTPClient_h
#define ESPClock_NTPClient_h

#include <IPAddress.h>
#include <WiFiUdp.h>
#include <Time.h>

class NTPClientClass {
public:
  static const uint16_t NTP_PORT = 123;
  static const uint16_t LOCAL_PORT = 2390;
  static const uint8_t NTP_TIMEOUT = 20;  // in seconds

  typedef void (*ntp_cb_t)(time_t t);

  NTPClientClass() : _cb(0), _state(STATE_IDLE), _req_time(0) { }

  void begin(ntp_cb_t cb);
  void loop();

  void startRequest();
  
private:
  static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  uint8_t _buf[NTP_PACKET_SIZE];

  enum client_state_t : uint8_t {
    STATE_IDLE,     // No request pending
    STATE_PENDING   // Waiting for UDP response
  };

  WiFiUDP _udp;
  ntp_cb_t _cb;
  client_state_t _state;
  time_t _req_time;
};

extern NTPClientClass NTPClient;

#endif  // ESPClock_NTPClient_h

