#include <Arduino.h>.
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
//////阿里云三元素//////////////////////
#define ProductKey     "a1ZMkwhevG9"             //产品KEY
#define DeviceName     "mzh001"      //设备名称
#define DeviceSecret   "cc96840980ac49570acc47e2c0195b5c"  //密钥
#define ALINK_TOPIC_PROP_POST     "/sys/" ProductKey "/" DeviceName "/thing/event/property/post"
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

void EC800NALiyun_init(void)//初始化MQTT连接
{
  char QMTCFG[] = "AT+QMTCFG=\"aliauth\",0,\"%s\",\"%s\",\"%s\"\r\n"; //配置三元素
  char jdata[200];
  sprintf(jdata, QMTCFG, ProductKey, DeviceName, DeviceSecret); //配置三元素
  // prints title with ending line break
  connect4G("AT", "OK");
  Serial2.println("ATE0&W");//关闭回显
  delay(300);
  Serial2.println("AT+QMTDISC=0");//关闭上一次MQTT连接
  delay(300);
  connect4G("AT+CPIN?", "+CPIN: READY"); //返+CPIN:READY，表明识别到卡了
  connect4G("AT+CGATT?", "+CGATT: 1"); //返+CGACT: 1,就能正常工作了
  Serial2.println("AT+QIACT=1");//对设备激活，获取IP地址
  delay(300);
  Serial.println(jdata);//打印三元素值址
  connect4G(jdata, "OK");
  connect4G("AT+QMTOPEN=0,\"iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883", "+QMTOPEN: 0,0"); //连接到服务器
  connect4G("AT+QMTCONN=0,\"clientExample\"", "+QMTCONN: 0,0,0");//登录到服务器
}

void AliyunMQTT_SENDData(void)//发布数据到阿里云
{
  char json_data[200];
  char jsonBuf[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"CurrentTemperature\":{\"value\":%d},\"CurrentHumidity\":{\"value\":%d}},\"method\":\"thing.event.property.post\"}";
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
  Serial.println(json_data);//串口将发送的数据打印出来
  Serial2.print("AT+QMTPUBEX=0,0,0,0,");
  Serial2.print(ALINK_TOPIC_PROP_POST);//先发布主题
  Serial2.print(",");
  Serial2.println(String(datalen));
  delay(300);              // wait for a second
  //Serial2.println(json_data);
  connect4G(json_data, "+QMTPUBEX: 0,0,0"); //查看是否发送成功

}
void ACKdata(char *dat)
{
  int datalen=strlen(dat);
  Serial2.print("AT+QMTPUBEX=0,0,0,0,");
  Serial2.print(ALINK_TOPIC_PROP_POST);//先发布主题
  Serial2.print(",");
  Serial2.println(String(datalen));
  delay(300);              // wait for a second
  //Serial2.println(json_data);
  connect4G(dat, "+QMTPUBEX: 0,0,0"); //查看是否发送成功
 }
void Aliyun_RECData(void)//收取服务器下发数据
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
      index = recstr.indexOf("{"); //找到JOSN的地方
      Jsontempdata = recstr.substring(index);
      endof = Jsontempdata.length() - 3;
      //Serial.println(endof);
      Jsondata = Jsontempdata.substring(0, endof); //去掉最后一个双引号.
      Serial.println(Jsondata);
      deserializeJson(json, Jsondata);
      JsonObject obj = json.as<JsonObject>();//将json内容各个解析
      String  Params = obj["params"];//解析出里面包含的json数据，进行二次解析
      Serial.println(Params);
      deserializeJson(json, Params);
      obj = json.as<JsonObject>();//将json内容各个解析
      if (Params.indexOf("NightLightSwitch") != -1) //说明存在这个参数
      {
        int LEDstatus = obj["NightLightSwitch"];//解析出LED灯,控制继电器
        if (LEDstatus)
        
          digitalWrite(LED_BLUE, LOW); //输出低电平
        else
          digitalWrite(LED_BLUE, HIGH); //输出高电平
      }
      else if (Params.indexOf("Buzzer") != -1) //说明存在这个参数
      {
        int BEEPstatus = obj["Buzzer"];//解析出LED灯,控制继电器
        if (BEEPstatus)
        {
          char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"Buzzer\":1},\"method\":\"thing.event.property.post\"}";
          digitalWrite(BEEP, HIGH); //GPIO2输出高电平
          ACKdata(Readjson);//上报状态值给阿里云
        }
        else
        {
          char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"Buzzer\":0},\"method\":\"thing.event.property.post\"}";
          digitalWrite(BEEP, LOW); //GPIO2输出低电平
          ACKdata(Readjson);//上报状态值给阿里云
        }
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
  EC800NALiyun_init();//初始化EC800N对接阿里云

}

void loop() {
  
  AliyunMQTT_SENDData();//发数据
  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  Aliyun_RECData();//收数据
  digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}
