#include <WiFi.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7789 driver chip
#include <SPI.h>
#include "ec800n.h"
IPAddress local_IP(192, 168, 4, 100);
IPAddress gateway(192, 168, 4, 100);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server;//声明服务器对象
const char *ssid = "ESP32_AP_TEST";
const char *password = "1234567890";
TFT_eSPI tft = TFT_eSPI();  // Invoke library
#define  LED_RED  8
void  TFT_SHOW(void);
void setup()
{
 tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  WiFi.mode(WIFI_AP); //设置工作在AP模式
  WiFi.softAPConfig(local_IP, gateway, subnet); //设置AP地址
  while (!WiFi.softAP(ssid, password)) {}; //启动AP
  Serial.println("AP Sucess!");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP()); // 打印IP地址
  WiFi.softAPsetHostname("myHostName"); //设置主机名
  Serial.print("HostName: ");
  Serial.println(WiFi.softAPgetHostname()); //打印主机名
  Serial.print("mac Address: ");
  Serial.println(WiFi.softAPmacAddress()); //打印mac地址
  server.begin(8888); //服务器启动监听端口号8888
  Serial.println("OPEN Server!");//建立服务端
  TFT_SHOW();
  EC800NAliyun_init();//EC800N初始化
}

void loop()
{
  delay(100);
  WiFiClient client = server.available(); //尝试建立客户对象
  if (client) //如果当前客户可用
  {
    Serial.println("[Client connected]");
    while (client.connected()) //如果客户端处于连接状态
    {
      if (client.available()) //如果有可读数据
      {
        String readBuff = client.readString(); //读取整个字符串
        client.print("Received: " + readBuff); //向客户端发送
        Serial.println("Received: " + readBuff); //从串口打印
        AliyunMQTT_SENDData(readBuff.toInt());
        TFT_SHOW();
        tft.print(readBuff);
        readBuff = "";
      }
    }
    //  client.stop(); //结束当前连接:
    //  Serial.println("[Client disconnected]");
  }
}

void  TFT_SHOW(void)
{
   tft.fillScreen(TFT_BLACK);
   tft.setCursor(0, 0, 2);
   tft.setTextColor(TFT_ORANGE,TFT_BLACK);  tft.setTextSize(4);
   tft.println("MOZIHAO");
   tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(2);
   tft.println("WIFI_AP");
   tft.setTextColor(TFT_YELLOW,TFT_BLACK);  tft.setTextSize(2);
   tft.println("AP Sucess!");
   tft.setTextColor(TFT_RED,TFT_BLACK);  tft.setTextSize(2);
   tft.println(WiFi.softAPIP());
   tft.println("8888");
   tft.setTextColor(TFT_GREEN,TFT_BLACK);
 }
