#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "smartconfig.h"
#include "DHT.h"
#include "hanzi.h"
#include "EC800N.h"
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
/* 设备证书信息*/
#define PRODUCT_KEY       "a1ZMkwhevG9"
#define DEVICE_NAME       "mzh001"
#define DEVICE_SECRET     "cc96840980ac49570acc47e2c0195b5c"
#define REGION_ID         "cn-shanghai"
/* 线上环境域名和端口号，不需要改 */
#define MQTT_SERVER       PRODUCT_KEY ".iot-as-mqtt." REGION_ID ".aliyuncs.com"
#define MQTT_PORT         1883
#define MQTT_USRNAME      DEVICE_NAME "&" PRODUCT_KEY
#define CLIENT_ID         "B4E62DFB29AD|securemode=3,signmethod=hmacsha1|"
// 请使用加密工具或参见MQTT-TCP连接通信文档加密生成password。
#define MQTT_PASSWD       "6D85070AA5A7BE3C19D40B63AA591D2A9B47B3BB"
#define ALINK_TOPIC_PROP_POST     "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
WiFiClient espClient;                                                         // 定义wifiClient实例
PubSubClient client(espClient);                                         // 定义PubSubClient的实例
DynamicJsonDocument json(200);
int freq = 50;      // 频率(20ms周期)
int channel = 8;    // 通道(高速通道（0 ~ 7）由80MHz时钟驱动，低速通道（8 ~ 15）由 1MHz 时钟驱动。)
int resolution = 8; // 分辨率
const int led = 12;
float humi,temp;
void  TFT_WIFISHOW(void);
void  TFT_CTRSHOW(void);
void TFT_SenorSHOW(void);
char num;
int calculatePWM(int degree)
{ //0-180度
  //20ms周期，高电平0.5-2.5ms，对应0-180度角度
  const float deadZone = 6.4;//对应0.5ms（0.5ms/(20ms/256）)
  const float max = 32;//对应2.5ms
  if (degree < 0)
    degree = 0;
  if (degree > 180)
    degree = 180;
  return (int)(((max - deadZone) / 180) * degree + deadZone);
}

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
}




void callback(char* topic, byte* payload, unsigned int length)//接收回调函数
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  deserializeJson(json, payload);
  JsonObject obj = json.as<JsonObject>();//将json内容各个解析
  String  Params = obj["params"];//解析出里面包含的json数据，进行二次解析
  Serial.println(Params);
  deserializeJson(json, Params);
  obj = json.as<JsonObject>();//将json内容各个解析
  if (Params.indexOf("NightLightSwitch") != -1) //说明存在这个参数
  {
    int LED = obj["NightLightSwitch"];//解析出LED灯,控制继电器
    if (LED)
    {
      char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"NightLightSwitch\":1},\"method\":\"thing.event.property.post\"}";
      digitalWrite(LED_BLUE, LOW); //输出低电平
      boolean d = client.publish(ALINK_TOPIC_PROP_POST, Readjson);
    }
    else
    {
      char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"NightLightSwitch\":0},\"method\":\"thing.event.property.post\"}";
      digitalWrite(LED_BLUE, HIGH); //输出高电平
      boolean d = client.publish(ALINK_TOPIC_PROP_POST, Readjson);

    }
    Serial.println(LED);
  }
  if (Params.indexOf("Buzzer") != -1) //说明存在这个参数
  {
    int BEEPstatus = obj["Buzzer"];//解析出蜂鸣器状态值
    if (BEEPstatus)
    {
      char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"Buzzer\":1},\"method\":\"thing.event.property.post\"}";
      digitalWrite(BEEP, HIGH); //GPIO2输出g高电平
      boolean d = client.publish(ALINK_TOPIC_PROP_POST, Readjson);

    }
    else
    {
      char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"Buzzer\":0},\"method\":\"thing.event.property.post\"}";
      digitalWrite(BEEP, LOW); //GPIO2输出低电平
      boolean d = client.publish(ALINK_TOPIC_PROP_POST, Readjson);
    }
    Serial.println(BEEP);
  }
  if (Params.indexOf("PWM") != -1) //说明存在这个参数
  {
    int PWMdata = obj["PWM"];//解析出蜂鸣器状态值
    ledcWrite(channel, calculatePWM(PWMdata)); // 输出PWM
    char Readjson[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"PWM\":%d},\"method\":\"thing.event.property.post\"}";
    char json_data[200];
    sprintf(json_data, Readjson, PWMdata);
    boolean d = client.publish(ALINK_TOPIC_PROP_POST, json_data);
    Serial.println(PWMdata);
  }

  // Switch on the LED if an 1 was received as first character
  /* if ((char)payload[0] == '1') {
     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     // but actually the LED is on; this is because
     // it is active low on the ESP-01)
    } else {
     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  */

}
void mqttPubdata(void)
{
  char json_data[200];
  char jsonBuf[] = "{\"id\":\"26\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"CurrentTemperature\":{\"value\":%d},\"CurrentHumidity\":{\"value\":%d}},\"method\":\"thing.event.property.post\"}";
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
  sprintf(json_data, jsonBuf, (int)temp, (int)humi);
  Serial.println(json_data);
  boolean d = client.publish(ALINK_TOPIC_PROP_POST, json_data);
  Serial.print("publish:0=Fail;1=Sucess    publish:");
  Serial.println(d);

}
void MQTT_Init(void)
{
  client.setServer(MQTT_SERVER, 1883);//设置连接的服务器
  Serial.println(MQTT_SERVER);
  client.setCallback(callback);//设置接收回调函数
}

void setup() {
  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
  Serial2.begin(115200,SERIAL_8N1,36,37);  // 启动串口通讯，波特率设置为115200,并配置对应端口
  pinMode(35, OUTPUT);//配置EC600S  PWRKEY引脚
  digitalWrite(35, HIGH); //对EC600S开机
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
  ledcSetup(channel, freq, resolution); // 设置通道
  ledcAttachPin(led, channel);          // 将通道与对应的引脚连接
  EC800NOnenet_init();//初始化EC800N
  MQTT_Init();
  delay(1000);
  TFT_CTRSHOW();
}
void loop()
{
  if (!client.connected()) {
    mqttCheckConnect();
  }
  digitalWrite(LED_RED, HIGH); //配置GPIO8端口为高电平，灯亮
  delay(300);
  digitalWrite(LED_RED, LOW); //GPIO8输出低电平 灯灭
  delay(300);
  mqttPubdata();//ESP32发数据
  OnenetMQTT_SENDData(temp,humi);//EC800N数据发送
  TFT_SenorSHOW();//显示数据
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
    tft.drawString("MOZIHAO_ALIYUN",10,10,4);
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
