#include <Arduino.h>
#include "DHT.h"
#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
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

void EC800NTCP_init(void)//初始化TCP连接
{

  // prints title with ending line break
  connect4G("AT", "OK");
  Serial2.println("ATE0&W");//关闭回显
  delay(300);
  Serial2.println("AT+QICLOSE=0");//关闭上一次socekt连接
  delay(300);
  connect4G("AT+CPIN?", "+CPIN: READY"); //返+CPIN:READY，表明识别到卡了
  connect4G("AT+CGATT?", "+CGATT: 1"); //返+CGACT: 1,就能正常工作了
  connect4G("AT+QIOPEN=1,0,\"TCP\",\"47.92.146.210\",8888,1234,1","+QIOPEN: 0,0"); //建立服务器的IP和端口连接
}

void TCP_SENDData(void)//发布数据到服务器
{
  char sendjson[]="{\"params\":{\"CurrentTemperature\":{\"value\":%d.%d},\"RelativeHumidity\":{\"value\":%d.%d}}}";
  char sendtcp[]="AT+QISEND=0,%d";
  char T_json[100];
  char T_send[100];
  char tempdata,humidata;
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
  tempdata = (t - (int)t) * 10;
  humidata = (h - (int)h) * 10;
  sprintf(T_json, sendjson, (int)t, tempdata, (int)h, humidata);
  sprintf(T_send,sendtcp,strlen(T_json));//先发指令长度
  Serial2.println(T_send);
  delay(300);              // wait for a second
  Serial2.println(T_json);
  while (Serial2.available() > 0)
  {
    //  char inByte = mySerial.read();
    //  Serial.print(inByte);
    if (Serial2.find("SEND OK"))
    {
      Serial.println("SEND OK");
      break;
    }
  }
}

void TCP_RECData(void)//收取服务器下发数据
{
  String recstr="";
  String Jsontempdata="";//暂时的
  String Jsondata="";//实际的
  char index,endof;
  while (Serial2.available() > 0)
  {
    if (Serial2.find("+QIURC:"))
    {
      recstr=Serial2.readString();
     index=recstr.indexOf("{");//找到JOSN的地方
     Jsontempdata=recstr.substring(index);
     endof=Jsontempdata.length()-3;
     //Serial.println(endof);
     Jsondata=Jsontempdata.substring(0,endof);//去掉最后一个双引号.
      Serial.println(Jsondata);
      //Serial.println(recstr);
      break;
    }
  }
}
void setup() {

  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
  Serial2.begin(115200,SERIAL_8N1,36,37);  // 启动串口通讯，波特率设置为115200,并配置对应端口
  pinMode(35, OUTPUT);//配置EC800N  PWRKEY引脚
  pinMode(8, OUTPUT);//配置闪烁灯
  digitalWrite(35, HIGH); //对EC800N开机 
  delay(500);
  dht.begin();
  EC800NTCP_init();//初始化EC800N

}

void loop() {
TCP_SENDData();//发数据
 digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);
TCP_RECData();//收数据
 digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}
