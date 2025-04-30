#include "EC800N.h"
//////阿里云三元素//////////////////////
#define ProductKey     "a1ZMkwhevG9"             //产品KEY
#define DeviceName     "mzh001"      //设备名称
#define DeviceSecret   "cc96840980ac49570acc47e2c0195b5c"  //密钥
#define ALINK_TOPIC_PROP_POST     "/sys/" ProductKey "/" DeviceName "/thing/event/property/post"
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
void EC800NAliyun_init(void)
{
  Serial2.begin(115200,SERIAL_8N1,36,37);  // 启动串口通讯，波特率设置为115200,并配置对应端口
  pinMode(35, OUTPUT);//配置EC800N  PWRKEY引脚
  digitalWrite(35, HIGH); //对EC800N开机
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

void AliyunMQTT_SENDData(int value)//发布数据到阿里云
{
  char json_data[200];
  char jsonBuf[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"LineVotage\":{\"value\":%d}},\"method\":\"thing.event.property.post\"}";
  sprintf(json_data, jsonBuf, value);
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
