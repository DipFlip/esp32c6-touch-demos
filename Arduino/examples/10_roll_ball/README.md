# ESP32-C6 Touch LCD Ball Game

A tilt-controlled ball game for the ESP32-C6 Touch LCD 1.47" display with IMU sensor.

## Features

- **Tilt Controls**: Use the built-in QMI8658 IMU to control the ball by tilting the device
- **Collect Stars**: Grab yellow stars for points (10 points each)
- **Avoid Enemies**: Red squares chase you - don't get caught!
- **Dynamic Difficulty**: New enemies spawn every 10 seconds with random speeds (100-200% of base speed)
- **Touch Restart**: Tap anywhere on the screen to restart after game over
- **Speed-based Ball Color**: Ball changes color based on velocity (green → yellow → orange → red)

## Hardware Requirements

- ESP32-C6-Touch-LCD-1.47 development board
- QMI8658 IMU sensor (integrated)
- AXS5106L touch controller (integrated)
- ST7789 LCD display (172x320, integrated)

## Dependencies

- Arduino_GFX_Library
- FastIMU
- esp_lcd_touch_axs5106l

## Pin Configuration

```cpp
Display SPI:
- DC: 15
- CS: 14
- SCK: 1
- MOSI: 2
- RST: 22
- Backlight: 23

Touch I2C:
- SDA: 18
- SCL: 19
- RST: 20
- INT: 21

IMU I2C:
- Address: 0x6B
- SDA: 18 (shared)
- SCL: 19 (shared)
```

## How to Play

1. Upload the sketch to your ESP32-C6 board
2. Tilt the device to move the ball
3. Collect yellow stars to increase your score
4. Avoid red enemy squares that chase you
5. Survive as long as possible!
6. When hit by an enemy, tap the screen to restart

## Game Mechanics

- **Ball Physics**: Realistic physics with friction and wall bounce
- **Enemy AI**: Enemies chase the player using simple pursuit
- **Progressive Difficulty**: Starts with 2 enemies, spawns 1 new enemy every 10 seconds (max 10)
- **Random Enemy Speeds**: Each enemy has a unique speed for variety
- **Smart Spawning**: Enemies won't spawn too close to the player

## Code Structure

- `10_roll_ball.ino`: Main game code
- Custom LCD initialization for the JD9853 display controller
- IMU-based tilt controls
- Touch screen restart functionality
- Dynamic enemy spawning system

## License

Open source - feel free to modify and share!
