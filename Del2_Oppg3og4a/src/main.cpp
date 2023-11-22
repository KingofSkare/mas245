// This program is a modified version of MAS245 PlatformIO Example, written by K. M. Knausgård 2023-10-21
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include "MPU9250.h"

namespace carrier {
  namespace pin
  {
    constexpr uint8_t oledDcPower{6};
    constexpr uint8_t oledCs{10};
    constexpr uint8_t oledReset{5};
  }

  namespace oled
  {
    constexpr uint8_t screenWidth{128}; // OLED display width in pixels
    constexpr uint8_t screenHeight{64}; // OLED display height in pixels
  }
}

namespace{
  CAN_message_t msg;

  FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

  Adafruit_SSD1306 display( carrier::oled::screenWidth,
                            carrier::oled::screenHeight,
                            &SPI,
                            carrier::pin::oledDcPower,
                            carrier::pin::oledReset,
                            carrier::pin::oledCs);
}

MPU9250 imu;

uint32_t lastID = 0;
int received = 0;

void setup() {
  Serial.begin(9600);
  can1.begin();
  can1.setBaudRate(500000);
 
   if( !display.begin(SSD1306_SWITCHCAPVCC) )
  {
    Serial.println(F("ERROR: display.begin(SSD1306_SWITCHCAPVCC) failed."));
  }

  display.clearDisplay();
  display.display();
  delay(200);

  Wire.begin();
  delay(2000);
  imu.setup(0x68);
}

void sendCan() {
  msg.len = 1;
  msg.id = 0x021;
  msg.buf[0] = imu.getLinearAccZ();
  can1.write(msg);
}

void loop() {

  display.clearDisplay(); 
  delay(500);
  
  if (imu.update()) {
    sendCan();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
  
    String textLine1 = "MAS245 - Gruppe 8";
    String textLine2 = "CAN-statistikk";
    String textLine3 = "Antall mottatt: " + String(received);
    String textLine4 = "Mottok sist ID: " + String(lastID, HEX);
    String textLine5 = "IMU z: " + String(imu.getLinearAccZ()) + String(" m/s^2");

    display.drawRoundRect(0, 0, display.width(), display.height(), 8, WHITE); 
    display.drawCircle(64, -239, 256, WHITE);
    display.setCursor(14, 3);
    display.print(textLine1);
    display.setCursor(8,20);
    display.print(textLine2);

    for (int x = 7; x < 109; x++) {
      display.drawPixel(x, 30, WHITE);
      if (x % 6 == 0) {
        x++;
        continue;
      }
    }

    display.setCursor(8, 33);    
    display.println(textLine3);  
    display.setCursor(8, 41);
    display.println(textLine4);

    for (int x = 7; x < 109; x++) {
      display.drawPixel(x, 51, WHITE);
      if (x % 6 == 0) {
        x++;
        continue;
      }
    }
    display.setCursor(8, 54);
    display.println(textLine5);

    received++;
    lastID = msg.id;

    display.display();
  }
}
