#include "Arduino.h"
#define  LED_RED  8
#define  LED_BLUE 21
// the setup function runs once when you press reset or power the board
/*
本代码实现对LED 2个灯交替闪烁
*/
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BLUE, LOW);   // turn the LED off (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LED_RED, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
}
