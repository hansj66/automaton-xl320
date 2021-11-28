#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include "display.h"
#include "qrcode.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Display::Display()
{

}

void Display::Begin()
{
//    Wire.setPins(21, 22);
     // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
}

char displaybuffer[512];

void Display::DisplayQRCode(char * buf)
{

  display.clearDisplay();
  QRCode qrcode;
  uint8_t qrcodeBytes[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeBytes, 3, 0, buf);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
              display.drawRect(x*2+32, y*2, 2, 2, 1);
        } else {
              display.drawRect(x*2+32, y*2, 2, 2, 0);
        }
    }
  }
  display.display();
}

void Display::DisplayText(const char * text, int waitms) 
{
  // strtok will crash if the text argument is stored in flash
  strcpy(displaybuffer, text);

  display.clearDisplay();
  display.setTextColor(WHITE);
  int y = 0;

  const char separator[2] = "\n";
  char *token;
   
  token = strtok(displaybuffer, separator);
   
  while( token != NULL ) 
  {
    display.setCursor(0, y);
    display.println(token);
    y+=10;
    token = strtok(NULL, separator);
  }
  display.display(); 
  delay(waitms);
}
