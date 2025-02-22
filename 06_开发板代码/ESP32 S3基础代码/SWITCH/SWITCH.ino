#include "Arduino.h"
#define  SWITCH  46
#define  LED_BLUE 21
// the setup function runs once when you press reset or power the board
/*
本代码实现对蜂鸣器开关以及LED灯开关
*/
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(SWITCH, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(SWITCH, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BLUE, LOW);   // turn the LED off (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(SWITCH, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
}
