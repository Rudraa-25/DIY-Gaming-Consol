# DIY Gaming Console
*An open-source embedded game console.*

## Project Overview
This project is a fully open-source, beginner-friendly DIY gaming console based on the ESP32-C3 microcontroller. It features a compact 1.3-inch OLED display and a custom analog 5-key button input system for intuitive gameplay.

Designed for makers, students, and DIY electronics enthusiasts, this console demonstrates how to build a complete embedded system from scratch. The firmware is clean and minimal, providing a solid foundation for learning game development on microcontrollers.

Whether you want to play classic mini-games like Snake and Pong or dive into a DOOM-style micro FPS, the DIY Gaming Console offers a fun and educational experience. It is easy to build, easy to program, and ready for future improvements..

## Project Photos
![Project Photo 1](img/P1.jpeg)
![Project Photo 2](img/P2.jpeg)

## Features
- **ESP32-C3 based DIY gaming console**: Powerful and efficient.
- **1.3-inch SH1106 OLED display**: Sharp monochrome graphics.
- **Single analog 5-key button input**: Simplified wiring.
- **Multiple built-in mini games**: Ready to play out of the box.
- **DOOM-style micro FPS**: A showcase of 3D-like rendering.
- **Battleship (micro version)**: Strategy in your pocket.
- **Snake, Pong, Dodge**: Classic arcade fun.
- **Minimal hardware and clean firmware**: Easy to understand and modify.
- **Fully open-source and beginner-friendly**: Perfect for learning.

## Games Included
- Snake
- Pong
- Dodge
- DOOM-style Micro FPS
- Battleship (Micro)

## Hardware Components
- ESP32-C3 Development Board
- 1.3-inch OLED Display (SH1106)
- Analog 5-Key Button Module
- USB-C Cable
- Jumper Wires

## Pin Connections
| Component | Pin | ESP32-C3 GPIO | Description |
|-----------|-----|---------------|-------------|
| OLED | SDA | GPIO 8 | I2C Data |
| OLED | SCL | GPIO 9 | I2C Clock |
| OLED | VCC | 3.3V | Power |
| OLED | GND | GND | Ground |
| Button | SIG | GPIO 0 (ADC) | Analog Signal |
| Button | VCC | 3.3V | Power |
| Button | GND | GND | Ground |

## Controls
- **LEFT**: Move / Rotate left
- **RIGHT**: Move / Rotate right
- **UP**: Move forward / Navigate up
- **DOWN**: Move backward / Navigate down
- **SW5 (Tap)**: Select / Fire
- **SW5 (Hold 3s)**: Exit game

## How to Build
### Hardware
Connect all components according to the [Pin Connections](#pin-connections) table above. Ensure your connections are secure before powering the device.

### Software
1.  **Install Arduino IDE**: Download and install the latest version.
2.  **Install ESP32 board support package**: Add ESP32 support in the Board Manager.
3.  **Install U8g2 library**: Search for and install the U8g2 library in the Library Manager.
4.  **Open the main .ino file**: Locate `code/CMF_GAME.ino` in this repository.
5.  **Select the ESP32-C3 board**: Choose "ESP32C3 Dev Module" (or similar) in the Tools menu.
6.  **Upload the code**: Connect your board via USB-C and click the Upload button.

## Folder Structure
```
DIY Gaming Console/
├── code/           # Source code (CMF_GAME.ino)
├── img/            # Project photos and assets
├── Schematics/     # Circuit diagrams and PDFs
└── README.md       # Project documentation
```

## Future Improvements
- [ ] Add sound support (buzzer or I2S DAC).
- [ ] Implement multiplayer via ESP-NOW.
- [ ] Design a 3D-printed case.
- [ ] Add SD card support for loading more games.
-tesing
## License
MIT License
