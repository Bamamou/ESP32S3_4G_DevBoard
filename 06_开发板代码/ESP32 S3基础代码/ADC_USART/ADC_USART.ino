#include "Arduino.h"
#define  LED_RED  8
#define  LED_BLUE 21
int sensorPin = A1;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
/*
本代码实现对光敏电阻的电压值采集，并通过串口打印出来
*/
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);//串口配置115200
}

// the loop function runs over and over again forever
void loop() {
  sensorValue = analogRead(sensorPin);//采集电压值
  Serial.println(sensorValue);//打印出来数据
  float voltage= sensorValue * (3.3 / 4096.0);//获取到电压值
  Serial.print(voltage);//打印出来数据
  Serial.println("V");
  digitalWrite(LED_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LED_RED, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                       // wait for a second
}
