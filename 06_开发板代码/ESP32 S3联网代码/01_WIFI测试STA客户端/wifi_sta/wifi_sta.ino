#include <WiFi.h>
#define AP_ssid   "4G-UFI-D63F" //这里改成你的设备当前环境下要连接的接入点名字
#define password  "1234567890"         //这里改成你的设备当前环境下要连接的接入点密码
const IPAddress serverIP(192,168,100,229); //欲访问的服务器地址
uint16_t serverPort = 10000;         //服务器端口号
WiFiClient client; //声明一个客户端对象，用于与服务器进行连接
int i = 0;    //用来判定连接是否超时的累加量
#define  LED_RED  14
void WIFI_Init(void)
{
   //调用 WiFi.begin()函数，开始连接接入点
  Serial.println("Start Connect");
  WiFi.begin(AP_ssid, password);
  Serial.print("Going");
  Serial.println(AP_ssid);
    //这里的循环用来判断是否连接成功的。连接过程中每隔500毫秒会检查一次是否连接成功，，并打一个点表示正在连接中
  //连接成功后会给出提示，但是若60秒后还是没有连接上，则会提示超时
  while (WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    Serial.print(".");
    if (i > 120) { //60秒后如果还是连接不上，就判定为连接超时
      Serial.println("Time Out,Connect Fail");
      break;
    }
  }
  Serial.println("Connect Suecss");  
  //这一部分用来输出连接网络的基本信息
  Serial.print("Woke Mode:");     // 告知用户设备当前工作模式
  Serial.println(WiFi.getMode());
  Serial.print("APname:");
  Serial.println(AP_ssid);            // 告知用户建立的接入点WiFi名
  Serial.print("Password:");
  Serial.println(password);        // 告知用户建立的接入点WiFi密码
  pinMode(LED_RED,OUTPUT);  //配置LED_RED端口模式为输出模式
 } 

 void TCP_Connect(void)
 {
   while (client.connect(serverIP, serverPort)==0) 
   {
      i++;
      delay(500);
      Serial.print("....");
      if (i > 120) 
      { //60秒后如果还是连接不上，就判定为连接超时
        Serial.println("Time Out, TCP Connect Fail");
        break;
      }
    }
     Serial.println("TCP Connect Suecss");
 
 }
 
void setup() {
  
   Serial.begin(115200);  // 启动串口通讯，波特率设置为115200
   delay(1000);
   WIFI_Init();
   TCP_Connect();
}

void loop()
{
  client.print("WIFI TCPtest!");   //发数据到服务器
  Serial.println("WIFI TCPtest!");
  if (client.available()) //如果有数据可读取
  {
      String line = client.readStringUntil('\n'); //读取数据到换行符
      Serial.print("Read Data From Server:");
      Serial.println(line);
      client.write(line.c_str()); //将收到的数据回发
  }
  digitalWrite(LED_RED,HIGH);  //配置LED_RED端口为高电平，灯亮
  delay(100);
  digitalWrite(LED_RED,LOW);  //LED_RED输出低电平 灯灭
  delay(100);
}
