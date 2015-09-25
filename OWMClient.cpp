#include <Arduino.h>
#include <Time.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "OWMClient.h"
#include "Settings.h"

OWMClient OWM;

const char OWMClient::API_HOST[] = "api.openweathermap.org";
const char OWMClient::API_ENDPOINT_ZIP[] PROGMEM = "/data/2.5/weather?zip=";
const char OWMClient::API_APIKEY_FIELD[] PROGMEM = ",us&units=metric&&APPID=";


void OWMClient::begin(owm_cb_t cb) {
  _cb = cb;
  _conditions.time = 0;
  // TODO(?) Do hostname -> IP lookup here?
}

void OWMClient::loop() {
  if (_state == IDLE || !_client.connected()) {  // TODO simplify
    return;
  }

  while (_client.available()) {
    String line = _client.readStringUntil('\n');
    switch(_state) {
      case WAITING_RESPONSE: {
        // Read status code
        int status = -1;
        if (line.startsWith(F("HTTP/1."))) {  // strlen == 9
          status = line.substring(9, line.indexOf(' ', 9)).toInt();
        }
        if (status != 200) {
          // Something went wrong (invalid response or HTTP error); terminate
          _client.stop();
          _state = IDLE;
          return;
        }
        _state = READING_HEADERS;
        break;
      }
      case READING_HEADERS: {
        if (line == "\r") {
          _state = READING_BODY;
        }
        break;
      }
      case READING_BODY: {  // Assumes body is a single line of JSON
        // Read and parse payload
        StaticJsonBuffer<3072> _json;  // TODO - CHECK (use DynamicJsonBuffer instead?)
        //DynamicJsonBuffer _json;
        JsonObject &s = _json.parseObject(line);  // TODO line.c_str() instead? (avoids copy but..??)
        if (!s.success()) {
          // Parse error, or out of memory; terminate
          _client.stop();
          _state = IDLE;
          return;
        }
        // Update _conditions
        _conditions.time = now();
        strncpy(_conditions.location_name, s["name"].as<String>().c_str(), sizeof(_conditions.location_name));
        strncpy(_conditions.condition, s["weather"][0]["main"].as<String>().c_str(), sizeof(_conditions.condition));
        JsonObject &main = s["main"].asObject();
        _conditions.temperature = main["temp"].as<float>();
        _conditions.temp_min = main["temp_min"].as<float>();
        _conditions.temp_max = main["temp_max"].as<float>();
        _conditions.humidity = main["humidity"].as<float>();
        _conditions.pressure = main["pressure"].as<float>();
        JsonObject &wind = s["wind"].asObject();
        _conditions.wind_speed = wind["speed"].as<float>();
        _conditions.wind_deg = wind["deg"].as<float>();
        // Terminate connection
        _client.stop();
        _state = IDLE;
        // Invoke callback
        if (_cb) {
          _cb(_conditions);
        }
        break;
      }
    }
    yield();
  }
}

bool OWMClient::startUpdate() {
  if (_state != IDLE || _client.connected()) {  // TODO simplify
    return false;
  }
  if (Settings.owm_apikey[0] == '\0') {
    return false;
  }
  if (!_client.connect(API_HOST, 80)) {
    return false;
  }
  // Print GET request line
  //   HTTP/1.0 seems to prevent chunked transfer encoding,
  //   even though server responds with HTTP/1.1
  _client.print(F("GET "));
  _client.print(FPSTR(API_ENDPOINT_ZIP)); _client.print(Settings.zip); 
  _client.print(FPSTR(API_APIKEY_FIELD)); _client.print(Settings.owm_apikey);
  _client.println(F(" HTTP/1.0"));
  // Print other headers
  _client.print(F("Host: ")); _client.println(FPSTR(API_HOST));
  _client.println(F("User-Agent: BuildFailureDetectorESP8266"));
  _client.println(F("Connection: close"));
  // End request headers
  _client.println();
  
  _state = WAITING_RESPONSE;
  
  return true;
}

