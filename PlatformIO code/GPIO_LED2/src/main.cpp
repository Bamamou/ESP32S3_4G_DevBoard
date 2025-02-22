#include <Arduino.h>

// LED pins
const int led2Pin = 21;
const int led1Pin = 8;
const int bootButtonPin = 0;  // Boot button is on GPIO0

// Task handles
TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;

// LED2 task
void ledTask(void *pvParameters) {
  pinMode(led2Pin, OUTPUT);
  while (1) {
    digitalWrite(led2Pin, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(led2Pin, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Button control task
void buttonTask(void *pvParameters) {
  pinMode(led1Pin, OUTPUT);
  pinMode(bootButtonPin, INPUT_PULLUP);
  
  while (1) {
    if (digitalRead(bootButtonPin) == LOW) {
      digitalWrite(led1Pin, LOW);
    } else {
      digitalWrite(led1Pin, HIGH);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);  // Small delay for debouncing
  }
}

void setup() {
  // Create the LED task
  xTaskCreate(
    ledTask,          // Task function
    "LED Task",       // Task name
    1024,             // Stack size
    NULL,             // Task parameters
    1,                // Task priority
    &ledTaskHandle    // Task handle
  );

  // Create the button task
  xTaskCreate(
    buttonTask,       // Task function
    "Button Task",    // Task name
    1024,             // Stack size
    NULL,             // Task parameters
    1,                // Task priority
    &buttonTaskHandle // Task handle
  );
}

void loop() {
  // Nothing to do here, everything is handled by the tasks
}