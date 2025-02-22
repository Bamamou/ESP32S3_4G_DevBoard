#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "smartconfig.h"
#include "DHT.h"
#include "hanzi.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
#define  LED_RED  8
#define  LED_BLUE 21
#define  BEEP     9
#define AP_ssid   "iPhone8" //这里改成你的设备当前环境下要连接的接入点名字
#define password  "12345678"         //这里改成你的设备当前环境下要连接的接入点密码
/* ID  用户名密码*/
#define CLIENT_ID         "EPS32"//CLIENT_ID一定要唯一
#define MQTT_USRNAME       "user"
#define MQTT_PASSWD     "12345678"
#define PUBTOPIC        "app/ESP32"//发布主题
#define SUBTOPIC        "dev/ESP32"//订阅主题
boolean LEDstatus,BEEPstatus;
WiFiClient espClient;                                                         // 定义wifiClient实例
PubSubClient client(espClient);                                         // 定义PubSubClient的实例
DynamicJsonDocument json(200);
float humi,temp;
void  TFT_WIFISHOW(void);
void  TFT_CTRSHOW(void);
void TFT_SenorSHOW(void);
char num;
void mqttCheckConnect()//检查连接状态
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT Server ...");
    Serial.println(CLIENT_ID);
    Serial.println(MQTT_USRNAME);
    Serial.println(MQTT_PASSWD);
    if (client.connect(CLIENT_ID, MQTT_USRNAME, MQTT_PASSWD))
    {

      Serial.println("MQTT Connected!");

    }
    else
    {
      Serial.print("MQTT Connect err:");
      Serial.println(client.state());
      delay(2000);
    }
  }
  //    boolean d =client.subscribe("dev/ESP32"); 订阅个主题
  boolean d = client.subscribe(SUBTOPIC);
  if (d == 1)
  {
    Serial.println("subscribe success!");
  }

}




void callback(char* topic, byte* payload, unsigned int length)//接收回调函数
{
  char recdata[100];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("\r\n");
  for (int i = 0; i < length - 2; i++) //去掉OK数据
  {
    Serial.print((char)payload[i]);
    recdata[i] = (char)payload[i]; //将数据存到到数组里面
  }
  deserializeJson(json, recdata);
  JsonObject obj = json.as<JsonObject>();
  Serial.println(obj);
   LEDstatus = obj["L1"];//解析出LED灯,控制继电器
  if (LEDstatus)
  {
    digitalWrite(LED_BLUE, LOW); //输出低电平
    tft.fillCircle(180,200,30,TFT_YELLOW);//对应LED
  }
  else
  {
    digitalWrite(LED_BLUE, HIGH); //输出高电平
    tft.fillCircle(180,200,30,TFT_GREEN);//对应LED
    //
    // Serial.println(LED);
  }
    BEEPstatus = obj["L2"];//解析出蜂鸣器状态值
  if (BEEPstatus)
  {
    digitalWrite(BEEP, HIGH); //GPIO9输出g高电平
    tft.fillCircle(50,200,30,TFT_YELLOW); //对应BEEP
  }
  else
  {
    digitalWrite(BEEP, LOW); //GPIO9输出低电平
     tft.fillCircle(50,200,30,TFT_GREEN); //对应BEEP
  }
  Serial.println(BEEPstatus);


}
void mqttPubdata(void)
{
  char json_data[200];
  char jsonBuf[] = "{\"order\":\"uploadStatus\",\"temp\":%d,\"humi\":%d,\"lon\":%d.%06d,\"lat\":%d.%06d,\"L1\":%d,\"L2\":%d}";
   humi = dht.readHumidity();
  // Read temperature as Celsius (the default)
   temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humi) || isnan(temp) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  sprintf(json_data, jsonBuf, (int)temp, (int)humi, 0, 0, 0, 0, LEDstatus, BEEPstatus);
  Serial.println(json_data);
  boolean d = client.publish(PUBTOPIC, json_data);
  Serial.print("publish:0=Fail;1=Sucess    publish:");
  Serial.println(d);

}
void MQTT_Init(void)
{
  client.setServer("47.92.146.210", 1883);//设置连接的服务器
  Serial.println("47.92.146.210");
  client.setCallback(callback);//设置接收回调函数

}

void setup() {
  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
  delay(1000);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  if (!AutoConfig())//查看是否连接上路由器
  {
    SmartConfig();
  }
  TFT_WIFISHOW();//注册网络成功
  dht.begin();//初始化DHT11
  pinMode(BEEP, OUTPUT); //配置GPIO2为蜂鸣器端口
  digitalWrite(BEEP, LOW); //GPIO2输出低电平
  pinMode(LED_RED, OUTPUT); //配置GPIO15为LED灯状态
  pinMode(LED_BLUE, OUTPUT); //配置GPIO14为LED灯状态
  MQTT_Init();
  delay(1000);
  TFT_CTRSHOW();
}
void loop()
{
  if (!client.connected()) {
    mqttCheckConnect();
  }
  digitalWrite(LED_RED, HIGH); //配置GPIO15端口为高电平，灯亮
  delay(300);
  digitalWrite(LED_RED, LOW); //GPIO15输出低电平 灯灭
  delay(300);
  mqttPubdata();
  TFT_SenorSHOW();
  delay(300);
  client.loop();
}

void  TFT_WIFISHOW(void)
{
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);  tft.setTextSize(2);
  tft.println(WiFi.SSID());
  tft.println(WiFi.psk().c_str());
  tft.println(WiFi.localIP());
 }

 void  TFT_CTRSHOW(void)
 {
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    //drawHanzi(32, 8, "我", TFT_YELLOW);
   // tft.drawBitmap(10, 24, mo, 64, 64, TFT_RED);
   // tft.drawBitmap(90, 24, zi, 64, 64, TFT_RED);
   // tft.drawBitmap(170, 24, hao, 64, 64, TFT_RED);
    tft.drawString("MOZIHAO_EMQTT",10,10,4);
    tft.drawBitmap(10, 50, wen, 32, 32, TFT_YELLOW);
    tft.drawBitmap(50, 50, du, 32, 32, TFT_YELLOW); 
    tft.drawBitmap(10, 100, shi, 32, 32, TFT_YELLOW);
    tft.drawBitmap(50, 100, du, 32, 32, TFT_YELLOW); 
    tft.drawBitmap(200, 50, C, 32, 32, TFT_YELLOW); 
    tft.drawBitmap(200, 100, PRE, 16, 32, TFT_YELLOW); 
    tft.fillCircle(50,200,30,TFT_GREEN); //对应BEEP
    tft.fillCircle(180,200,30,TFT_GREEN);//对应LED
    tft.fillCircle(115,200,15,TFT_RED);//对应工作状态
  }

  void TFT_SenorSHOW(void)//显示温湿度值
  {
      tft.drawFloat(temp, 1, 100,50,6);
     tft.setTextColor(TFT_ORANGE, TFT_BLACK);    // Set colour back to yellow
     tft.drawFloat(humi, 1, 100,100,6);
     tft.setTextColor(TFT_ORANGE, TFT_BLACK);    // Set colour back to yellow
     num++;
     if (num%2)
     {
       tft.fillCircle(115,200,15,TFT_RED);
     }
      else
      {
       tft.fillCircle(115,200,15,TFT_PURPLE);
      }
    }
