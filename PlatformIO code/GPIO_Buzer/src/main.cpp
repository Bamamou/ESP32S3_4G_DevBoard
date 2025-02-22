#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BUZZER_PIN 9              // Buzzer connected to pin 9
#define BUTTON_PIN 0              // Button connected to pin 0

void buzzerTask(void *pvParameters) {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  while (1) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      int frequency = random(100, 1000); // Random frequency between 100Hz and 1000Hz
      tone(BUZZER_PIN, frequency);
      vTaskDelay(pdMS_TO_TICKS(200)); // Change tone every 200ms
    } else {
      noTone(BUZZER_PIN);
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Check button state every 50ms
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreate(buzzerTask, "Buzzer Task", 1024, NULL, 1, NULL);
}

void loop() {
  // Nothing to do here
}