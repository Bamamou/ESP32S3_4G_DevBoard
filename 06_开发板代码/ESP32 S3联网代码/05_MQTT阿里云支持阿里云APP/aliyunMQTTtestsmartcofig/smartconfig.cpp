#include "smartconfig.h"
#include "oled.h"
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
      //  tft.println(WiFi.SSID().c_str());
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
