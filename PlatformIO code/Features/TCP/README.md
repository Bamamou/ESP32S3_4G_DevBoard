# ESP32 4G TCP Client with DHT11 Sensor

An IoT implementation using ESP32S3 with 4G connectivity to send temperature and humidity data to a remote TCP server.

## Features

- 4G TCP client implementation
- DHT11 temperature and humidity monitoring
- Multi-tasking with FreeRTOS
- JSON-formatted data transmission
- LED status indicators
- Automatic reconnection handling

## Hardware Requirements

- ESP32S3 Development Board
- DHT11 Temperature and Humidity Sensor
- 4G Modem (EC800N)
- USB Cable for programming

## Software Dependencies

- PlatformIO
- Libraries:
  - DHT sensor library
  - FreeRTOS
  - Arduino core for ESP32

## Pin Configuration

- DHT11 Data Pin: GPIO1
- 4G Module UART: 
  - RX: GPIO36
  - TX: GPIO37
- Status LEDs:
  - LED1: GPIO15
  - LED2: GPIO8
- Power Control: GPIO35

## Setup Instructions

1. Connect the hardware according to pin configuration
2. Update server settings in the code:
   ```cpp
   #define TCP_SERVER_IP "47.92.146.210"
   #define TCP_SERVER_PORT 8888
   ```
3. Upload the code to your ESP32
4. Monitor the serial output at 115200 baud

## How It Works

1. System initializes 4G modem and DHT11 sensor
2. Establishes TCP connection with remote server
3. Three concurrent tasks run:
   - DataTask: Reads sensor and sends data every 2 seconds
   - ReceiveTask: Monitors for incoming data
   - LedTask: Controls status LED blinking

## Technical Specifications

### System Parameters
- UART Baud Rate: 115200
- TCP Server Port: 8888
- Task Stack Size: 4096 bytes
- Data Update Interval: 2 seconds
- LED Blink Interval: 500ms

### Sensor Specifications (DHT11)
- Temperature Range: 0-50°C
- Humidity Range: 20-90% RH
- Reading Interval: 2 seconds

### Data Format
```json
{
    "params": {
        "CurrentTemperature": {
            "value": float
        },
        "RelativeHumidity": {
            "value": float
        }
    }
}
```

### Task Priorities
- DataTask: Priority 2
- ReceiveTask: Priority 1
- LedTask: Priority 0

### Error Handling
- DHT11 Reading Failures: Detected and reported
- 4G Connection: Automatic retry mechanism
- TCP Send Verification: "SEND OK" confirmation
- JSON Data Parsing: Protected string handling

## License

[MIT License](LICENSE)
