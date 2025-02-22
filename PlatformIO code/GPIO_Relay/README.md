
# GPIO Relay Control with FreeRTOS

This project demonstrates how to control a relay switch connected to GPIO46 on an ESP32S3 using the boot button (GPIO0) with FreeRTOS.

## Hardware Requirements

- ESP32S3 development board
- Relay module connected to GPIO46
- Boot button connected to GPIO0 (typically built-in on ESP32S3 boards)

## Software Requirements

- PlatformIO
- Arduino framework
- FreeRTOS library (included with Arduino framework for ESP32)

## Project Structure

```
ESP32S3/PlatformIO code/GPIO_Relay
│
├── src
│   └── main.cpp
└── README.md
```

## main.cpp Overview

The `main.cpp` file contains the following key components:

- Pin definitions for the boot button and relay switch
- FreeRTOS task to control the relay switch based on the boot button state
- Setup function to create the FreeRTOS task
- Empty loop function as the task handles the main functionality

## How to Use

1. Connect the relay module to GPIO46 and the boot button to GPIO0 on the ESP32S3 board.
2. Open the project in PlatformIO.
3. Build and upload the code to the ESP32S3 board.
4. Press the boot button to control the relay switch.

## License

This project is licensed under the MIT License.
````

</file>