# ESP32-S3 LDR-Controlled Buzzer

This project implements a light-sensitive buzzer system using an ESP32-S3 microcontroller. The system reads values from a Light Dependent Resistor (LDR) and converts them into corresponding buzzer frequencies, creating an audio representation of light intensity.

## Hardware Requirements

- ESP32-S3 Development Board
- Light Dependent Resistor (LDR)
- Active Buzzer
- 10kΩ Resistor (for LDR voltage divider)
- Jumper Wires

## Pin Connections

- LDR: GPIO2 (ADC input)
- Buzzer: GPIO9 (PWM output)

## Software Requirements

- PlatformIO IDE
- Arduino framework for ESP32

## Features

- Real-time light intensity monitoring
- Frequency mapping from 100Hz to 2000Hz
- FreeRTOS task implementation
- Serial debug output

## Installation

1. Clone this repository
2. Open the project in PlatformIO
3. Build and upload to your ESP32-S3 board

## Usage

Once uploaded, the system will:
1. Read light intensity values from the LDR
2. Map these values to frequencies (100-2000 Hz)
3. Output the corresponding tone through the buzzer
4. Display debug information via Serial Monitor (115200 baud)

## Debug Output

Connect to the Serial Monitor at 115200 baud to view:
- LDR raw values (0-4095)
- Mapped frequency values (100-2000 Hz)

## Code Preview

Here's the main implementation of the LDR-controlled buzzer:

```cpp
// Pin definitions
const int LDR_PIN = 2;    // LDR connected to GPIO2
const int BUZZER_PIN = 9; // Buzzer connected to GPIO9

void BuzzerControlTask(void *parameter) {
  while(1) {
    // Read LDR value
    int ldrValue = analogRead(LDR_PIN);
    
    // Map LDR value to frequency
    int frequency = map(ldrValue, 0, 4095, 100, 2000);
    
    // Output the mapped frequency
    ledcSetup(0, frequency, 8);
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWrite(0, 127);
    
    // Debug output
    Serial.printf("LDR Value: %d, Frequency: %d Hz\n", ldrValue, frequency);
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
```

## License

This project is released under the MIT License.
