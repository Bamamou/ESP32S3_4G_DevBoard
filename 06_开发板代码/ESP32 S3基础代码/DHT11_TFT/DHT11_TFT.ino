/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 It uses the time of compile/upload to set the time
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo...

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################

 Based on clock sketch by Gilchrist 6/2/2014 1.0

A few colour codes:

code  color
0x0000  Black
0xFFFF  White
0xBDF7  Light Gray
0x7BEF  Dark Gray
0xF800  Red
0xFFE0  Yellow
0xFBE0  Orange
0x79E0  Brown
0x7E0 Green
0x7FF Cyan
0x1F  Blue
0xF81F  Pink

 */

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "hanzi.h"
#include "DHT.h"
#define  LED_RED  8
#define  LED_BLUE 21
#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define TFT_GREY 0x5AEB
DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

void setup(void) {
  //Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  //drawHanzi(32, 8, "我", TFT_YELLOW);
   tft.drawBitmap(10, 24, mo, 64, 64, TFT_RED);
   tft.drawBitmap(90, 24, zi, 64, 64, TFT_RED);
   tft.drawBitmap(170, 24, hao, 64, 64, TFT_RED);
   tft.drawBitmap(10, 114, wen, 32, 32, TFT_YELLOW);
   tft.drawBitmap(50, 114, du, 32, 32, TFT_YELLOW); 
   tft.drawBitmap(10, 174, shi, 32, 32, TFT_YELLOW);
   tft.drawBitmap(50, 174, du, 32, 32, TFT_YELLOW); 
   tft.drawBitmap(200, 114, C, 32, 32, TFT_YELLOW); 
   tft.drawBitmap(200, 174, PRE, 16, 32, TFT_YELLOW); 
   //tft.drawChar(':', 180, 174, 6);
   pinMode(LED_RED, OUTPUT);
   pinMode(LED_BLUE, OUTPUT);
   dht.begin();
   
}

void loop() {
  digitalWrite(LED_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);
  digitalWrite(LED_RED, LOW);    // turn the LED off by making the voltage LOW
  delay(200);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
     tft.drawFloat(t, 1, 100,114,6);
     tft.setTextColor(TFT_GREEN, TFT_BLACK);    // Set colour back to yellow
     tft.drawFloat(h, 1, 100,174,6);
     tft.setTextColor(TFT_GREEN, TFT_BLACK);    // Set colour back to yellow
  
}
