# ESP32-C6 Touch LCD 1.47" Demo Collection

Demo collection for the ESP32-C6-Touch-LCD-1.47 development board (172x320 touch display, QMI8658 IMU, WiFi 6, BLE 5).

## Examples

**Examples 01-08** are from the official [Waveshare wiki](https://www.waveshare.com/wiki/1.47inch_Touch_LCD).

**Example 09** - Real-time IMU data plotter

**Example 10** - Tilt-controlled ball game with collectibles and enemy AI. New enemies spawn every 10 seconds with random speeds.

## Getting Started

1. Clone this repository:
   ```bash
   git clone git@github.com:DipFlip/esp32c6-touch-demos.git
   cd esp32c6-touch-demos
   ```

2. Copy libraries to your Arduino libraries folder:
   ```bash
   cp -r Arduino/libraries/* ~/Arduino/libraries/
   ```

3. Install ESP32 board support in Arduino IDE:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Install from Board Manager

4. Select Board: "ESP32C6 Dev Module"

5. Open any example and upload!

## License

Open source. Individual libraries may have their own licenses.
