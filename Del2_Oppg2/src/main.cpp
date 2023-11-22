// This program is a modified version of MAS245 PlatformIO Example, written by K. M. Knausgård 2023-10-21
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include "MPU9250.h"


namespace{
  CAN_message_t msg;
  FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
}

void setup() {
  Serial.begin(9600);
  can1.begin();
  can1.setBaudRate(250000);
}

void sendCan() {
  msg.len = 1;
  msg.id = 0x021;
  can1.write(msg);
}

void loop() {

  CAN_message_t com;

  if (can1.read(com)) {
    msg.buf[0] =  com.buf[0];
    sendCan();
  }
}
