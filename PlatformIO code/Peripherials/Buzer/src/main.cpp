#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Pin definitions
const int LDR_PIN = 2;    // LDR connected to GPIO2
const int BUZZER_PIN = 9; // Buzzer connected to GPIO9

// Task function declaration
void BuzzerControlTask(void *parameter);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Configure pins
  pinMode(LDR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Create the buzzer control task
  xTaskCreate(
    BuzzerControlTask,    // Task function
    "BuzzerControl",      // Task name
    2048,                 // Stack size
    NULL,                 // Task parameters
    1,                    // Priority
    NULL                  // Task handle
  );
}

void loop() {
  // Empty loop as we're using FreeRTOS tasks
  vTaskDelete(NULL);
}

void BuzzerControlTask(void *parameter) {
  while(1) {
    // Read LDR value
    int ldrValue = analogRead(LDR_PIN);
    
    // Map LDR value (0-4095) to frequency (100-2000 Hz)
    int frequency = map(ldrValue, 0, 4095, 100, 2000);
    
    // Output the mapped frequency to the buzzer
    ledcSetup(0, frequency, 8); // Channel 0, frequency, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWrite(0, 127); // 50% duty cycle
    
    // Print debug information
    Serial.printf("LDR Value: %d, Frequency: %d Hz\n", ldrValue, frequency);
    
    // Small delay to prevent overwhelming the system
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}