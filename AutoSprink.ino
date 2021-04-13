#include <ESP8266.h>
#include <SoftwareSerial.h>
#include "TroykaRTC.h"
#include <SPI.h>

#define SSID     "YOUR_SSID"
#define PASSWORD "YOUR_PASSWORD"
#define HUMIDITY_PIN A0
#define PUMP_PIN A1
#define MOSFET_PIN 4

SoftwareSerial mySerial(13, 10);
ESP8266 wifi(mySerial);
RTC clock;
int humidity;
int hum;
char date[12];
char time[12];
char weekDay[12];
const String ip = "YOUR_IP";

void setup(void) {
  if (wifi.joinAP(SSID, PASSWORD)) {
  }
  clock.begin();
  clock.set(__TIMESTAMP__);
  pinMode(MOSFET_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
}

void loop(void) {
  clock.read();
  digitalWrite(MOSFET_PIN, HIGH);
  humidity = analogRead(HUMIDITY_PIN);
  digitalWrite(MOSFET_PIN, LOW);
  clock.getTimeStamp(time, date, weekDay);
  if (wifi.createTCP(ip, 80)) {
    String data = "GET /push/?";
    data += "date=" + String(date) + "&time="
            + String(time) + "&mouisture=" + String(map(humidity, 0, 1023, 0, 100)) + " HTTP/1.1\r\n";
    data += "Host: http://" + ip + "r\n\r\n";
    wifi.send((const uint8_t*) data.c_str(), data.length());
    wifi.releaseTCP();
  }
  delay(100);
  if (humidity <= 650) {
    digitalWrite(PUMP_PIN, HIGH);
    if (humidity >= 750) {
      digitalWrite(PUMP_PIN, LOW);
    }
    delay(600);
    digitalWrite(PUMP_PIN, LOW);
  }
  delay(1000);
}