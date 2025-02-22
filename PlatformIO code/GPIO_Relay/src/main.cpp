#include <Arduino.h>

// Pin definitions
const int BOOT_BUTTON_PIN = 0;  // Boot button is typically on GPIO0
const int SWITCH_PIN = 46;      // Switch connected to GPIO46

// Task handle
TaskHandle_t switchTaskHandle = NULL;

// Switch control task
void switchControlTask(void *parameter) {
    // Configure pins
    pinMode(BOOT_BUTTON_PIN, INPUT);
    pinMode(SWITCH_PIN, OUTPUT);
    
    while(1) {
        // Read boot button state and write it to switch
        bool buttonState = !digitalRead(BOOT_BUTTON_PIN); // Inverted because boot button is active LOW
        digitalWrite(SWITCH_PIN, buttonState);
        
        // Small delay to prevent task from hogging CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    // Create the switch control task
    xTaskCreate(
        switchControlTask,    // Task function
        "SwitchControl",      // Task name
        2048,                // Stack size (bytes)
        NULL,                // Task parameters
        1,                   // Task priority
        &switchTaskHandle    // Task handle
    );
}

void loop() {
    // Empty loop as we're using FreeRTOS tasks
    vTaskDelay(pdMS_TO_TICKS(1000));
}