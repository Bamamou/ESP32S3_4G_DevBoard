# ESP32-S3 WiFi Access Point with FreeRTOS

## Project Overview
This project implements a WiFi Access Point on the ESP32-S3 using FreeRTOS for task management. It features a TCP server and LED indication for client connections.

## Hardware Requirements
- ESP32-S3 Development Board
- LED connected to GPIO8 (RED)

## Network Configuration
```
SSID: ESP32S3_AP_TEST
Password: 1234567890
IP Address: 192.168.4.100
Gateway: 192.168.4.100
Subnet Mask: 255.255.255.0
TCP Server Port: 8888
```

## FreeRTOS Task Structure
The application uses three main tasks distributed across both cores:

### Core 0
- **WiFi AP Task** (Priority 2)
  - Configures and starts the WiFi Access Point
  - Sets up network parameters
  - Self-terminates after initialization
  
- **Server Task** (Priority 1)
  - Handles TCP server operations
  - Manages client connections
  - Processes incoming data

### Core 1
- **LED Task** (Priority 1)
  - Controls LED indication
  - Blinks when client is connected
  - Stays off when no clients

## LED Indicators
- **RED LED (GPIO8)**
  - Blinking (500ms interval): Client connected
  - OFF: No client connected

## Serial Communication
- Baud Rate: 115200
- Debug Information:
  - WiFi connection status
  - Client connection events
  - Data received from clients
  - Network configuration details

## Key Features
1. Automatic AP configuration
2. Static IP configuration
3. Task synchronization using FreeRTOS notifications
4. Visual feedback for client connections
5. TCP server with echo functionality

## TCP Server Functionality
- Listens on port 8888
- Echoes received messages back to client
- Provides connection status via Serial monitor

## Usage Instructions
1. Upload the code to ESP32-S3
2. Monitor Serial output at 115200 baud
3. Connect to "ESP32S3_AP_TEST" WiFi network
4. Use TCP client to connect to 192.168.4.100:8888
5. Send messages to see echo response

## Debug Output Format
```
AP Success!
====================
SSID: ESP32S3_AP_TEST
IP address: 192.168.4.100
HostName: myHostName
MAC Address: XX:XX:XX:XX:XX:XX
====================
```
