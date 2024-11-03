#include <Arduino.h>
#include "GEA3.h"

static GEA3 gea3;

void setup()
{
  Serial.begin(115200);

  pinMode(LED_HEARTBEAT, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_MQTT, OUTPUT);

  Serial1.begin(GEA3::baud, SERIAL_8N1, D7, D6);

  gea3.begin(Serial1);
  gea3.send(GEA3::Packet(0xE4, 0xFF, (uint8_t[]){ 0x01 }));
  gea3.send(GEA3::Packet(0xE4, 0xFF, (uint8_t[]){ 0x02 }));
  gea3.send(GEA3::Packet(0xE4, 0xFF, (uint8_t[]){ 0x03 }));
  gea3.send(GEA3::Packet(0xE4, 0xFF, { 0x05 }));
  gea3.send(GEA3::Packet(0xE4, 0xFF, { 0x06 }));
  gea3.send(GEA3::Packet(0xE4, 0xFF, { 0x07, 8, 9, 10 }));

  gea3.readERD(0xC0, 0x1234);
  gea3.writeERD(0xC0, 0x1234, (uint16_t)0xABCD);
}

void loop()
{
  gea3.loop();
  digitalWrite(LED_HEARTBEAT, millis() % 1000 < 500);
}
