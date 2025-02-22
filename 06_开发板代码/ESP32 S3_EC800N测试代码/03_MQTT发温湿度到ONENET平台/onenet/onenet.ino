#include <Arduino.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
//////ONENET三元素//////////////////////
#define ProductID    "31580807"             //产品ID
#define DeviceName     "112650"      //设备名称
#define JianQuan   "201808160002"  //鉴权信息
#define  LED_RED  8
#define  LED_BLUE 21
#define  BEEP     9
DynamicJsonDocument json(200);
//判断返回值是否正确
void connect4G (String cmd, char *res)
{
  while (1)
  {
    Serial2.println(cmd);
    delay(300);
    while (Serial2.available() > 0)
    {
      if (Serial2.find(res))
      {
        digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(100);              // wait for a second
        digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
        delay(100);
        Serial.println(res);
        return;
      }
      else
      {
        Serial.print(cmd);
        Serial.println("  Return ERROR!");
      }
    }
    delay(200);
  }
}

void EC800NOnenet_init(void)//初始化MQTT连接
{
  // prints title with ending line break
  char QMTCOON[] = "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"";
  char jdata[200];
  sprintf(jdata, QMTCOON, ProductID, DeviceName, JianQuan); //配置三元素
  connect4G("AT", "OK");
  Serial2.println("ATE0&W");//关闭回显
  delay(300);
  Serial2.println("AT+QMTDISC=0");//关闭上一次MQTT连接
  delay(300);
  connect4G("AT+CPIN?", "+CPIN: READY"); //返+CPIN:READY，表明识别到卡了
  connect4G("AT+CGATT?", "+CGATT: 1"); //返+CGACT: 1,就能正常工作了
  Serial2.println("AT+QIACT=1");//对设备激活，获取IP地址
  delay(300);
  connect4G("AT+QMTCFG=\"version\",0,4", "OK");//设置下MQTT版本
  connect4G("AT+QMTOPEN=0,\"183.230.40.39\",6002", "+QMTOPEN: 0,0"); //连接到服务器
  connect4G(jdata, "+QMTCONN: 0,0,0");//登录到服务器
}

void OnenetMQTT_SENDData(void)//发布数据到ONENET
{
  char json_data[200];
  char jsonBuf[] = "{\"datastreams\":[{\"id\":\"temp\",\"datapoints\":[{\"value\":%d}]},{\"id\":\"humi\",\"datapoints\":[{\"value\":%d}]}]}";
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
  sprintf(json_data, jsonBuf, (int)t, (int)h);
  int datalen = strlen(json_data); //获取数据长度
  char zero[3] = {0x01, 0x00, 0xFF};
  zero[2] = datalen & 0xFF;
  Serial.println(json_data);//串口将发送的数据打印出来
  Serial2.print("AT+QMTPUBEX=0,0,0,0,\"$dp\",");
  Serial2.println(String(datalen + 3));
  delay(100);
  Serial2.write(zero, 3);
  connect4G(json_data, "+QMTPUBEX: 0,0,0"); //查看是否发送成功
}

void Onenet_RECData(void)//收取服务器下发数据
{
  String recstr = "";
  String Jsontempdata = ""; //暂时的
  String Jsondata = ""; //实际的
  char index, endof;
  while (Serial2.available() > 0)
  {

    if (Serial2.find("+QMTRECV:"))//收到服务器下发的数据
    {
      recstr = Serial2.readString();
      Serial.println(recstr);
      if ( recstr.indexOf("L1") != -1) //灯亮
      {
        digitalWrite(LED_BLUE, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
      } else   if ( recstr.indexOf("L0") != -1) //灯灭
      {
        digitalWrite(LED_BLUE, HIGH ); // Turn the LED off by making the voltage HIGH
      }
      // Switch on the LED if an 1 was received as first character
      if ( recstr.indexOf("S1") != -1) {
        digitalWrite(BEEP, HIGH);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
      } else  if ( recstr.indexOf("S0") != -1)
      {
        digitalWrite(BEEP, LOW ); // Turn the LED off by making the voltage HIGH
      }
      break;
    }
  }
}
void setup() {

  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
 Serial2.begin(115200,SERIAL_8N1,36,37);  // 启动串口通讯，波特率设置为115200,并配置对应端口
  pinMode(35, OUTPUT);//配置EC800N PWRKEY引脚
  digitalWrite(35, HIGH); //对EC800N开机
  pinMode(BEEP, OUTPUT); //配置GPIO9为蜂鸣器端口
  digitalWrite(BEEP, LOW); //GPIO9输出低电平
  pinMode(LED_RED, OUTPUT); //配置GPIO8为LED灯状态
  pinMode(LED_BLUE, OUTPUT); //配置GPIO21为LED灯状态
  delay(500);
  dht.begin();
  EC800NOnenet_init();//初始化EC800N对接ONENET

}

void loop() {

  OnenetMQTT_SENDData();//发数据
  digitalWrite(LED_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  Onenet_RECData();//收数据
  digitalWrite(LED_RED, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}
