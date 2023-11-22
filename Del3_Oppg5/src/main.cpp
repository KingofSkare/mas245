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
uint32_t outData;

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
  msg.buf[0] = outData;
  can1.write(msg);
}

void loop() {

  display.clearDisplay(); 
  delay(500);
  
  if (imu.update()) {

    float imuData = imu.getLinearAccZ();
    outData = (uint32_t)(abs(imuData*10000));
    sendCan();

    CAN_message_t com;

    float outData2 = (can1.read(com));
    outData2 = msg.buf[0];
    outData2 = outData2/10000; 
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
  
    String textLine2 = "Sending: " + String(imu.getLinearAccZ());
    String textLine3 = "Receiving: " + String(outData2);

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
    display.display();
  }
}