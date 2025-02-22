#include "EC800N.h"
//////ONENET三元素//////////////////////
#define ProductID    "31580807"             //产品ID
#define DeviceName     "112650"      //设备名称
#define JianQuan   "201808160002"  //鉴权信息
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
        digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(100);              // wait for a second
        digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
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
void EC800NOnenet_init(void)
{
  Serial2.begin(115200);  // 启动串口通讯，波特率设置为115200
  pinMode(4, OUTPUT);//配置EC600S  PWRKEY引脚
  digitalWrite(4, HIGH); //对EC600S开机
  char QMTCOON[] ="AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"";
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

void OnenetMQTT_SENDData(int tempdata,int humidata)//发布数据到ONENET
{
  char json_data[200];
  char jsonBuf[] = "{\"datastreams\":[{\"id\":\"temp\",\"datapoints\":[{\"value\":%d}]},{\"id\":\"humi\",\"datapoints\":[{\"value\":%d}]}]}";
  sprintf(json_data, jsonBuf, (int)tempdata, (int)humidata);
  int datalen = strlen(json_data); //获取数据长度
  char zero[3]={0x01,0x00,0xFF};
  zero[2]=datalen&0xFF;
  Serial.println(json_data);//串口将发送的数据打印出来
  Serial2.print("AT+QMTPUBEX=0,0,0,0,\"$dp\",");
  Serial2.println(String(datalen+3));
  delay(100);    
  Serial2.write(zero,3);
 connect4G(json_data, "+QMTPUBEX: 0,0,0"); //查看是否发送成功

}
