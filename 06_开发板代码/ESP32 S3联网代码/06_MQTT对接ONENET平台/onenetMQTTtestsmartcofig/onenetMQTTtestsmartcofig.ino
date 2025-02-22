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
/* 设备证书信息*/
#define PRODUCTID         "112650"//产品ID
#define DEVICEID          "31580807"//设备ID
#define DEVICE_SECRET     "201808160002"//鉴权信息
WiFiClient espClient;                                                         // 定义wifiClient实例
PubSubClient client(espClient);                                         // 定义PubSubClient的实例
int i = 0;    //用来判定连接是否超时的累加量
char num;
DynamicJsonDocument json(200);
float humi,temp;
void  TFT_WIFISHOW(void);
void  TFT_CTRSHOW(void);
void TFT_SenorSHOW(void);
void mqttCheckConnect()//检查连接状态
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT Server ...");
    Serial.println(DEVICEID);
    Serial.println(PRODUCTID);
    Serial.println(DEVICE_SECRET);
    if (client.connect(DEVICEID, PRODUCTID, DEVICE_SECRET))
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
}




void callback(char* topic, byte* payload, unsigned int length)//接收回调函数
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);//将获取到的ONENET下发的数据打印出来
  }
  if ((char)payload[0] == 'L' && (char)payload[1] == '1') {
    digitalWrite(LED_BLUE, LOW);   // Turn the LED on (Note that LOW is the voltage level
    tft.fillCircle(180,200,30,TFT_YELLOW);//对应LED
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else if ((char)payload[0] == 'L' && (char)payload[1] == '0')
  {
    digitalWrite(LED_BLUE, HIGH ); // Turn the LED off by making the voltage HIGH
    tft.fillCircle(180,200,30,TFT_GREEN);//对应LED
  }
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'S' && (char)payload[1] == '1') {
    digitalWrite(BEEP, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    tft.fillCircle(50,200,30,TFT_YELLOW); //对应BEEP
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else if ((char)payload[0] == 'S' && (char)payload[1] == '0')
  {
    digitalWrite(BEEP, LOW ); // Turn the LED off by making the voltage HIGH
    tft.fillCircle(50,200,30,TFT_GREEN); //对应BEEP
  }

}
void mqttPubdata(void)
{
  // sprintf(json,t_json,sht20.temperature(),sht20.humidity());
  char sendjson[] = "{\"datastreams\":[{\"id\":\"temp\",\"datapoints\":[{\"value\":%d.%d}]},{\"id\":\"humi\",\"datapoints\":[{\"value\":%d.%d}]}]}";
  char T_json[200];
  char msg_buf[200];
  char tempdata, humidata;
  //  sensors_event_t humidity, temp;
  //(int)temp.temperature, (int)humidity.relative_humidity
  //aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
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
  tempdata = (temp - (int)temp) * 10;
  humidata = (humi - (int)humi) * 10;
  sprintf(T_json, sendjson, (int)temp, tempdata, (int)humi, humidata);
  Serial.println(T_json);
  int json_len = strlen(T_json);
  msg_buf[0] = char(0x01);                       //要发送的数据必须按照ONENET的要求发送, 根据要求,数据第一位是3
  msg_buf[1] = char(json_len >> 8);              //数据第二位是要发送的数据长度的高八位
  msg_buf[2] = char(json_len & 0xff);            //数据第三位是要发送数据的长度的低八位
  memcpy(msg_buf + 3, T_json, strlen(T_json)); //从msg_buf的第四位开始,放入要传的数据msgJson
  msg_buf[3 + strlen(T_json)] = 0;              //添加一个0作为最后一位, 这样要发送的msg_buf准备好了
  boolean d = client.publish("$dp", (uint8_t *)msg_buf, 3 + strlen(T_json)); //发送数据到主题$dp
  // Serial.print("publish:0=Fail;1=Sucess    publish:");
  Serial.println(d);

}
void MQTT_Init(void)
{
  client.setServer("183.230.40.39", 6002);//设置连接的服务器
  Serial.println("183.230.40.39");
  client.setCallback(callback);//设置接收回调函数
}

void setup() {
  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
  delay(1000);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  pinMode(BEEP, OUTPUT); //配置GPIO9为蜂鸣器端口
  digitalWrite(BEEP, LOW); //GPIO9输出低电平
  pinMode(LED_RED, OUTPUT); //配置GPIO8为LED灯状态
  pinMode(LED_BLUE, OUTPUT); //配置GPIO21为LED灯状态
  if (!AutoConfig())//查看是否连接上路由器
  {
    SmartConfig();
  }
  TFT_WIFISHOW();//注册网络成功
  dht.begin();//初始化DHT11
  MQTT_Init();//初始化服务器端口连接
  delay(1000);
  TFT_CTRSHOW();
}

void loop()
{
  if (!client.connected()) {
    mqttCheckConnect();
  }
  digitalWrite(LED_RED, HIGH); //配置端口为高电平，灯灭
  delay(300);
  digitalWrite(LED_RED, LOW); //输出低电平 灯亮
  delay(300);
  TFT_SenorSHOW();//显示数据
  mqttPubdata();
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
    tft.drawString("MOZIHAO_ONENET",10,10,4);
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
