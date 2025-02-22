# ESP32-S3 Dual LED Control Project

This project demonstrates the use of FreeRTOS tasks on an ESP32-S3 to control two LEDs:
- One LED blinks continuously
- Another LED is controlled by the boot button

## Hardware Requirements

- ESP32-S3 development board
- 2x LEDs
- 2x current-limiting resistors (220Ω-330Ω)
- Jumper wires

## Connections

- LED1: Connected to GPIO8 (controlled by boot button)
- LED2: Connected to GPIO21 (blinking LED)
- Boot Button: Built-in button on GPIO0

## Functionality

The project uses two FreeRTOS tasks:
1. LED Task: Continuously blinks the LED connected to GPIO21 with a 1-second interval
2. Button Task: Monitors the boot button state and controls LED1:
   - When boot button is pressed: LED1 turns ON
   - When boot button is released: LED1 turns OFF

## Implementation Details

- Uses FreeRTOS task management
- Implements button debouncing with a 50ms delay
- Both tasks run independently at priority level 1
- Uses INPUT_PULLUP for the boot button to avoid need for external pullup resistor

## Software Requirements

- PlatformIO
- Arduino framework

## Installation

1. Clone this repository to your local machine.
2. Open the project in PlatformIO.
3. Connect your ESP32S3 development board to your computer.
4. Upload the code to the board.

## License

This project is licensed under the MIT License.
