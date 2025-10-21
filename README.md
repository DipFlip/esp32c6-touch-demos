# ESP32-C6 Touch LCD 1.47" Demo Collection

A comprehensive collection of demos and examples for the ESP32-C6-Touch-LCD-1.47 development board featuring a 172x320 touch display, QMI8658 IMU sensor, and more.

## Hardware Specifications

- **MCU**: ESP32-C6FH4 (WiFi 6, BLE 5, IEEE 802.15.4)
- **Display**: 1.47" ST7789 LCD (172x320 pixels)
- **Touch**: AXS5106L capacitive touch controller
- **IMU**: QMI8658 6-axis accelerometer/gyroscope
- **Storage**: SD card slot
- **Battery**: Built-in battery management

## Repository Structure

```
├── Arduino/
│   ├── examples/          # Arduino example sketches
│   │   ├── 01_gfx_helloworld/
│   │   ├── 02_qmi8658_output/
│   │   ├── 03_sd_card_test/
│   │   ├── 04_lvgl_arduino_v8/
│   │   ├── 05_lvgl_battery/
│   │   ├── 06_lvgl_brightness/
│   │   ├── 07_lvgl_qmi8658/
│   │   ├── 08_lvgl_image/
│   │   ├── 09_imu_plotter/
│   │   └── 10_roll_ball/     # ⭐ Featured: Tilt-controlled ball game
│   └── libraries/         # Required libraries
└── ESP-IDF/              # ESP-IDF examples
```

## Featured Demo: Roll Ball Game 🎮

A complete tilt-controlled ball game with dynamic enemies, collectibles, and touch controls.

**Features:**
- Tilt controls using the IMU sensor
- Collect yellow stars for points
- Avoid red enemies that chase you
- Dynamic difficulty - new enemies spawn every 10 seconds
- Random enemy speeds for variety
- Touch screen to restart
- Progressive gameplay up to 10 enemies

**Location:** `Arduino/examples/10_roll_ball/`

[See detailed documentation →](Arduino/examples/10_roll_ball/README.md)

## Arduino Examples

### Basic Examples
- **01_gfx_helloworld** - Simple graphics demo using Arduino_GFX
- **02_qmi8658_output** - Read IMU sensor data (accelerometer/gyroscope)
- **03_sd_card_test** - Test SD card read/write functionality

### LVGL Examples
- **04_lvgl_arduino_v8** - LVGL v8 widgets demo with touch support
- **05_lvgl_battery** - Battery level monitoring UI
- **06_lvgl_brightness** - Display brightness control
- **07_lvgl_qmi8658** - IMU data visualization with LVGL
- **08_lvgl_image** - Image display examples

### Advanced Examples
- **09_imu_plotter** - Real-time IMU data plotting
- **10_roll_ball** - Complete game with physics and AI

## Getting Started

### Prerequisites

**Arduino IDE:**
1. Install [Arduino IDE](https://www.arduino.cc/en/software) 1.8.19 or newer
2. Add ESP32 board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Install ESP32 boards from Board Manager
4. Select Board: "ESP32C6 Dev Module"

**Required Libraries:**
- Arduino_GFX_Library
- lvgl (version 8.x)
- FastIMU
- esp_lcd_touch_axs5106l (included in `libraries/`)

### Quick Start

1. Clone this repository:
   ```bash
   git clone git@github.com:DipFlip/esp32c6-touch-demos.git
   cd esp32c6-touch-demos
   ```

2. Copy libraries to your Arduino libraries folder:
   ```bash
   cp -r Arduino/libraries/* ~/Arduino/libraries/
   ```

3. Open any example from `Arduino/examples/` in Arduino IDE

4. Select your board and port:
   - Board: "ESP32C6 Dev Module"
   - Port: (your USB port)

5. Upload and enjoy!

## Pin Configuration

### Display (SPI)
- DC: GPIO 15
- CS: GPIO 14
- SCK: GPIO 1
- MOSI: GPIO 2
- RST: GPIO 22
- Backlight: GPIO 23

### Touch Controller (I2C)
- SDA: GPIO 18
- SCL: GPIO 19
- RST: GPIO 20
- INT: GPIO 21
- Address: 0x63

### IMU QMI8658 (I2C)
- SDA: GPIO 18 (shared with touch)
- SCL: GPIO 19 (shared with touch)
- Address: 0x6B

### SD Card (SPI)
- CS: GPIO 7
- MOSI: GPIO 4
- SCK: GPIO 5
- MISO: GPIO 6

## Troubleshooting

**Display not working:**
- Check SPI connections
- Verify backlight is on (GPIO 23)
- Run `01_gfx_helloworld` to test

**Touch not responding:**
- Ensure touch library is installed
- Check I2C connections (SDA/SCL)
- Verify INT pin connection
- Run `04_lvgl_arduino_v8` to test touch

**IMU not detected:**
- Check I2C address (0x6B)
- Verify shared I2C bus with touch
- Run `02_qmi8658_output` to test

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## License

This project is open source. Individual libraries and components may have their own licenses.

## Acknowledgments

- Arduino_GFX_Library by moononournation
- LVGL - Light and Versatile Graphics Library
- FastIMU library
- ESP32 Arduino Core by Espressif
