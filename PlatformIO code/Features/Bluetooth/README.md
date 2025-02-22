# ESP32-S3 Bluetooth Relay Control

This project implements a Bluetooth-controlled relay system using an ESP32-S3 microcontroller. The system includes LED status indication and buzzer feedback for command acknowledgment.

## Hardware Requirements

- ESP32-S3 Development Board
- Relay Module (connected to GPIO46)
- LED (connected to GPIO8)
- Buzzer (connected to GPIO9)

## Features

1. **Bluetooth Control**
   - Device name: "ESP32S3_Relay"
   - Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   - Characteristic UUID: beb5483e-36e1-4688-b7f5-ea07361b26a8

2. **Commands**
   - '1': Turn relay ON
   - '0': Turn relay OFF
   - 's': Query relay status

3. **Feedback Mechanisms**
   - LED indication: Blinks when relay is ON (500ms interval)
   - Buzzer feedback: Short beep when command is received
   - Bluetooth response messages for each command

## Pin Configuration

```
RELAY_PIN  -> GPIO46
LED_PIN    -> GPIO8
BUZZER_PIN -> GPIO9
```

## Code Preview

Here are the key parts of the implementation:

```cpp
// Command handling
void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        shortBeep();  // Feedback when command received
        char command = value[0];
        switch(command) {
            case '1':  // Turn ON
                digitalWrite(RELAY_PIN, HIGH);
                status = true;
                break;
            case '0':  // Turn OFF
                digitalWrite(RELAY_PIN, LOW);
                status = false;
                break;
            case 's':  // Status query
                status = digitalRead(RELAY_PIN);
                break;
        }
    }
}

// LED Blinking Task
void BluetoothControlTask(void *parameter) {
    bool ledState = false;
    while(1) {
        if (deviceConnected && status) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            vTaskDelay(pdMS_TO_TICKS(500));
        } else {
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}
```

## Implementation Details

- Uses FreeRTOS tasks for non-blocking operation
- Implements BLE server with read/write characteristics
- Provides visual (LED) and audio (buzzer) feedback
- Automatically handles device connections/disconnections

## Usage

1. Power up the ESP32-S3 board
2. Connect to "ESP32S3_Relay" using a BLE scanner app
3. Send commands ('0', '1', or 's') to control the relay
4. Observe LED blinking when relay is active
5. Listen for buzzer feedback when commands are received

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
