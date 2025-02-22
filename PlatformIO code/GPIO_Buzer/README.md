# GPIO Buzzer with ESP32S3

This project demonstrates how to use an ESP32S3 microcontroller to control a buzzer using a button. The buzzer will emit a random frequency tone when the button is pressed.

## Hardware Requirements

- ESP32S3 development board
- Buzzer
- Push button
- Connecting wires

## Pin Configuration

- Buzzer: Connected to GPIO 9
- Button: Connected to GPIO 0

## Software Requirements

- [PlatformIO](https://platformio.org/) IDE
- [Arduino framework](https://www.arduino.cc/)

## Setup Instructions

1. Clone this repository to your local machine.
2. Open the project in PlatformIO IDE.
3. Connect the ESP32S3 board to your computer.
4. Upload the code to the ESP32S3 board.

## Usage

1. Connect the buzzer and button to the specified GPIO pins.
2. Press the button to make the buzzer emit a random frequency tone.
3. Release the button to stop the buzzer.

## Code Overview

The main code is located in `src/main.cpp` and consists of the following tasks:
- `buzzerTask`: Handles the buzzer control based on the button state.

```cpp
// filepath: /C:/Users/admin/OneDrive/Desktop/ESP32S3/PlatformIO code/GPIO_Buzer/src/main.cpp
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BUZZER_PIN 9
#define BUTTON_PIN 0

void buzzerTask(void *pvParameters) {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  while (1) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      int frequency = random(100, 1000);
      tone(BUZZER_PIN, frequency);
      vTaskDelay(pdMS_TO_TICKS(200));
    } else {
      noTone(BUZZER_PIN);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreate(buzzerTask, "Buzzer Task", 1024, NULL, 1, NULL);
}

void loop() {
  // Nothing to do here
}
```

## License

This project is licensed under the MIT License.
