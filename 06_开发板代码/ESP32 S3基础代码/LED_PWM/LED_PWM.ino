#include "Arduino.h"
#define  LED_RED  8
#define  LED_BLUE 21
int sensorPin = A1;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int freq = 2000;    // 频率
int channel = 0;    // 通道
int resolution = 12;   // 分辨率

/*
  本代码实现光敏来控制LED灯亮度
*/
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);//串口配置115200
  ledcSetup(channel, freq, resolution); // 设置通道0
  ledcAttachPin(LED_RED, channel);  // 将通道与对应的引脚连接LED端口
}

// the loop function runs over and over again forever
void loop() {
  sensorValue = analogRead(sensorPin);//采集电压值
  Serial.println(sensorValue);//打印出来数据
  ledcWrite(channel, sensorValue);  // 输出PWM  由ADC光敏实现
  delay(20);
}
