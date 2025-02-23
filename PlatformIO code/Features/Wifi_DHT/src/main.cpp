#include "main.h"
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Structure to hold DHT data
typedef struct {
    float temperature;
    float humidity;
} DHTData;

DHTData dhtValues;

// Function to read DHT data
DHTData readDHTData() {
    DHTData data;
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();
    return data;
}

// Function to handle the /data endpoint
void handleData() {
    dhtValues = readDHTData();
    if (isnan(dhtValues.humidity) || isnan(dhtValues.temperature)) {
        server.send(500, "text/plain", "Failed to read from DHT sensor");
        return;
    }

    String json = "{\"params\":{\"CurrentTemperature\":{\"value\":" + String(dhtValues.temperature, 1) + "},\"RelativeHumidity\":{\"value\":" + String(dhtValues.humidity, 1) + "}}}";
    server.send(200, "application/json", json);
}

// Function to handle the root endpoint (serving the HTML file)
void handleRoot() {
    File file = SPIFFS.open("/index.html");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    String content = file.readString();
    server.send(200, "text/html", content);
    file.close();
}

// Task for HTTP server
void HttpServerTask(void *parameter) {
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    // Route handlers
    server.on("/", handleRoot);
    server.on("/data", handleData);

    // Start server
    server.begin();
    Serial.println("HTTP server started");

    while (1) {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Task handles
TaskHandle_t dataTaskHandle = NULL;
TaskHandle_t receiveTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;

// Check if return value is correct
void connect4G (String cmd, const char* res)
{
  while (1)
  {
    Serial2.println(cmd);
    delay(300);
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

void printConnectionStatus() {
    Serial.println("----------------------------------------");
    Serial.printf("Device Hostname: %s\n", DEVICE_HOSTNAME);
    Serial.printf("Connected to: %s:%d\n", TCP_SERVER_IP, TCP_SERVER_PORT);
    Serial.println("----------------------------------------");
}

void EC800NTCP_init(void) // Initialize TCP connection
{
  // prints title with ending line break
  connect4G("AT", "OK");
  Serial2.println("ATE0&W"); // Turn off echo
  delay(300);
  Serial2.println("AT+QICLOSE=0"); // Close previous socket connection
  delay(300);
  connect4G("AT+CPIN?", "+CPIN: READY"); // Returns +CPIN:READY, indicating SIM card is detected
  connect4G("AT+CGATT?", "+CGATT: 1"); // Returns +CGACT: 1, ready for normal operation
  
  char connectCmd[128];
  snprintf(connectCmd, sizeof(connectCmd), "AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,1234,1", TCP_SERVER_IP, TCP_SERVER_PORT);
  connect4G(connectCmd, "+QIOPEN: 0,0"); // Establish connection with server IP and port
  
  printConnectionStatus();
}

void TCP_SENDData(void) // Publish data to server
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

void TCP_RECData(void) // Receive data from server
{
  String recstr="";
  String Jsontempdata=""; // Temporary
  String Jsondata=""; // Actual
  char index,endof;
  while (Serial2.available() > 0)
  {
    if (Serial2.find("+QIURC:"))
    {
      recstr=Serial2.readString();
     index=recstr.indexOf("{"); // Find JSON location
     Jsontempdata=recstr.substring(index);
     endof=Jsontempdata.length()-3;
     //Serial.println(endof);
     Jsondata=Jsontempdata.substring(0,endof); // Remove the last quotation mark
      Serial.println(Jsondata);
      //Serial.println(recstr);
      break;
    }
  }
}

// Task for reading sensor and sending data
void DataTask(void *parameter) {
    while(1) {
        TCP_SENDData();
        vTaskDelay(pdMS_TO_TICKS(2000)); // Send data every 2 seconds
    }
}

// Task for receiving data
void ReceiveTask(void *parameter) {
    while(1) {
        TCP_RECData();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check for data every second
    }
}

// Task for LED blinking
void LedTask(void *parameter) {
    while(1) {
        digitalWrite(8, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(8, LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void setup() {
    // Initialize Serial communications
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 36, 37);

    // Initialize GPIO
    pinMode(35, OUTPUT);
    pinMode(8, OUTPUT);
    digitalWrite(35, HIGH);
    delay(500);

    // Initialize sensors and network
    dht.begin();
    EC800NTCP_init();

    // Create tasks with different priorities
    xTaskCreate(
        DataTask,           // Task function
        "DataTask",        // Name for debugging
        STACK_SIZE,        // Stack size
        NULL,              // Parameters
        2,                 // Priority (higher number = higher priority)
        &dataTaskHandle    // Task handle
    );

    xTaskCreate(
        ReceiveTask,
        "ReceiveTask",
        STACK_SIZE,
        NULL,
        1,
        &receiveTaskHandle
    );

    xTaskCreate(
        LedTask,
        "LedTask",
        STACK_SIZE,
        NULL,
        0,
        &ledTaskHandle
    );

    // Create HTTP server task
    xTaskCreate(
        HttpServerTask,      // Task function
        "HttpServerTask",   // Name for debugging
        STACK_SIZE * 3,      // Stack size
        NULL,                // Parameters
        4,                   // Priority
        NULL                 // Task handle
    );
}

void loop() {
    // Empty loop as we're using FreeRTOS tasks
    vTaskDelete(NULL);
}
