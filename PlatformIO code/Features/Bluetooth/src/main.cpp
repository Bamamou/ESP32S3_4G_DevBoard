#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Pin definitions
const int RELAY_PIN = 46;  // Relay connected to GPIO46
const int LED_PIN = 8;     // LED connected to GPIO8
const int BUZZER_PIN = 9; // Buzzer connected to GPIO18
bool status = false;       // Relay status

// BLE service and characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Task function declaration
void BluetoothControlTask(void *parameter);

// Helper function for buzzer beep
void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));  // 50ms beep
  digitalWrite(BUZZER_PIN, LOW);
}

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Callback class to handle BLE server events
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

// Callback class to handle BLE characteristic events
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      // Beep on command received
      shortBeep();
      
      char command = value[0];
      switch(command) {
        case '1':
          digitalWrite(RELAY_PIN, HIGH);
          status = true;
          pCharacteristic->setValue("Relay ON");
          break;
          
        case '0':
          digitalWrite(RELAY_PIN, LOW);
          status = false;
          pCharacteristic->setValue("Relay OFF");
          break;
          
        case 's':
          // Send relay status
          status = digitalRead(RELAY_PIN);
          pCharacteristic->setValue(status ? "Relay is ON" : "Relay is OFF");
          break;
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize BLE
  BLEDevice::init("ESP32S3_Relay");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service and characteristic
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Relay OFF");
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
  
  // Create a FreeRTOS task for Bluetooth control
  xTaskCreate(
    BluetoothControlTask,
    "BluetoothControl",
    4096,
    NULL,
    1,
    NULL
  );
}

void loop() {
  // Delete the loop task as it's not needed
  vTaskDelete(NULL);
}

// Task to handle Bluetooth control and LED blinking
void BluetoothControlTask(void *parameter) {
  bool ledState = false;
  while(1) {
    if (deviceConnected) {
      if (status) {
        // Blink LED when relay is ON
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        vTaskDelay(pdMS_TO_TICKS(500)); // Blink every 500ms
      } else {
        // Turn OFF LED when relay is OFF
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(20));
      }
    } else {
      digitalWrite(LED_PIN, LOW);
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}
