#include <WiFi.h>
#include <esp_wifi.h>
#define LED_RED 8
#include <TFT_eSPI.h> // Graphics and font library for ST7789 driver chip
#include <SPI.h>
/*
 * 智能配网，通过使用ESP32 提供的软件选择智能配网，让WIFI模块不需要手动配置WIFI用户名和密码，掉电自动保存
 */
TFT_eSPI tft = TFT_eSPI();  // Invoke library
void SmartConfig()
{
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WIFI Wait for Smartconfig");
    WiFi.beginSmartConfig();
    while (1)
    {
        Serial.print("......");
        if (WiFi.smartConfigDone())
        {
        Serial.println("WIFI SmartConfig Success");
        Serial.printf("SSID:%s", WiFi.SSID().c_str());
        Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
        Serial.print("LocalIP:");
        Serial.print(WiFi.localIP());
        Serial.print(" ,GateIP:");
        Serial.println(WiFi.gatewayIP());
        WiFi.setAutoConnect(true);  // 设置自动连接
        break;
        }
        delay(1000);
    }
}
bool AutoConfig()
{
    WiFi.begin();
    for (int i = 0; i < 20; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
          Serial.println("WIFI SmartConfig Success");
          Serial.printf("SSID:%s", WiFi.SSID().c_str());
          Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
          Serial.print("LocalIP:");
          Serial.print(WiFi.localIP());
          Serial.print(" ,GateIP:");
          Serial.println(WiFi.gatewayIP());
          tft.setTextColor(TFT_YELLOW,TFT_BLACK);  tft.setTextSize(2);
          tft.println(WiFi.SSID());
          tft.println(WiFi.psk().c_str());
          tft.println(WiFi.localIP());
          return true;
        }
        else
        {
          Serial.print("WIFI AutoConfig Waiting......");
          Serial.println(wstatus);
          delay(1000);
        }
    }
    Serial.println("WIFI AutoConfig Faild!" );
    return false;
}
void setup() {
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  delay(100);
  TFT_Init();
  if (!AutoConfig())
  {
    SmartConfig();
  }
 // tft.fillCircle(140,80,10,TFT_RED);
}
void loop() 
{
 
  Serial.println("WIFI_APtest");
  digitalWrite(2,HIGH);  //配置GPIO2端口为高电平，灯亮
  delay(300);
  digitalWrite(2,LOW);  //GPIO2输出低电平 灯灭
  delay(200);
}
void TFT_Init(void)
{
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_ORANGE,TFT_BLACK);  tft.setTextSize(4);
  tft.println("MOZIHAO");
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(2);
  tft.println("Wait for Smartconfig");
}
