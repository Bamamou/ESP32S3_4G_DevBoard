# ESP32 WiFi Weather Station

A real-time weather monitoring system using ESP32, DHT11 sensor, and WebSocket communication.

## Features

- Real-time temperature and humidity monitoring
- WebSocket-based data transmission
- Responsive web interface
- Asynchronous web server
- FreeRTOS task management
- Custom hostname configuration

## Hardware Requirements

- ESP32S3 Development Board
- DHT11 Temperature and Humidity Sensor
- USB Cable for programming

## Software Dependencies

- PlatformIO
- Libraries:
  - ESPAsyncWebServer
  - AsyncTCP
  - DHT sensor library
  - ArduinoJson

## Pin Configuration

- DHT11 Data Pin: Defined in `main.h` as `DHTPIN`

## Setup Instructions

1. Clone this repository
2. Open the project in PlatformIO
3. Update WiFi credentials in `main.h`:
   ```cpp
   const char* ssid = "your_wifi_ssid";
   const char* password = "your_wifi_password";
   ```
4. Connect the DHT11 sensor to the specified pin
5. Upload the code to your ESP32
6. Monitor the serial output for the IP address
7. Access the web interface at `http://<esp32-ip-address>`

## Project Structure

```
Wifi/
├── src/
│   ├── main.cpp       # Main application code
│   └── main.h         # Header file with configurations
└── README.md          # Project documentation
```

## How It Works

1. The ESP32 connects to WiFi using the provided credentials
2. A web server is started on port 80
3. The system creates a FreeRTOS task to read sensor data
4. Temperature and humidity readings are broadcast via WebSocket
5. Connected clients receive real-time updates
6. The web interface displays the current readings

## Technical Specifications

### Sensor Specifications (DHT11)
- Temperature Measurement Range: 0°C to 50°C (±2°C accuracy)
- Humidity Measurement Range: 20% to 90% RH (±5% RH accuracy)
- Sampling Rate: Every 2 seconds
- Resolution: 
  - Temperature: 1°C
  - Humidity: 1% RH

### System Parameters
- WiFi Operation: 2.4GHz band
- Web Server Port: 80
- WebSocket Path: `/ws`
- Data Format: JSON
- Update Interval: 2000ms (2 seconds)
- Task Stack Size: 4096 bytes
- Task Priority: 1 (low priority)
- Task Core Assignment: Core 1

### JSON Data Structure
```json
{
    "temperature": float,    // Temperature in Celsius
    "humidity": float,       // Relative Humidity in %
    "hostname": string,      // Device hostname
    "ip": string            // Device IP address
}
```

### Operating Conditions
- Input Voltage: 3.3V DC
- Operating Temperature: -40°C to 85°C (ESP32)
- Power Consumption:
  - Active Mode: ~80mA
  - Light Sleep: ~0.8mA
- Flash Memory Usage: ~900KB
- RAM Usage: ~50KB

### Error Handling
- Invalid Readings: Filtered out (NaN check)
- WiFi Disconnection: Automatic reconnection attempt
- WebSocket Disconnection: Auto cleanup of stale clients

## Contributing

Feel free to submit issues and enhancement requests.

## License

[MIT License](LICENSE)
