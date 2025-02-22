#include <Arduino.h>.
#include <Wire.h>
#include <HardwareSerial.h>

HardwareSerial mySerial1(1);//设置串口1

void setup() {

  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
  mySerial1.begin(115200, SERIAL_8N1, 39, 40); //RX1 39 TX1 40
  pinMode(38, OUTPUT);//设置RE端口

}

void loop() {
  digitalWrite(38, HIGH);//设置发数据
  Serial.println("RS485_TEST");
  mySerial1.println("RS485_TEST");
  delay(100);
  digitalWrite(38, LOW);//设置收数据
  delay(500);
  while (mySerial1.available() > 0)
  {
    String recstr = "";
    recstr = mySerial1.readString();
    Serial.println(recstr);
  }
  delay(500);
}
