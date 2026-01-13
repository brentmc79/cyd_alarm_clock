# ESP32-S3 CrowPanel Alarm Clock

A full-featured alarm clock built on the Elecrow CrowPanel ESP32-S3 5-inch display (800×480 RGB parallel interface) with capacitive touch, WiFi time synchronization, and intelligent brightness control.

## Features

### Implemented ✅
- **RGB Parallel Display** - 800×480 resolution with LovyanGFX graphics library
- **PWM Backlight Control** - Smooth brightness adjustment
- **PSRAM Support** - 768KB framebuffer allocation for flicker-free graphics
- **Capacitive Touch** - GT911 multi-touch support (up to 5 points)
- **WiFi Provisioning** - BLE-based WiFi setup via ESP BLE Provisioning app
- **Credential Storage** - NVS-based WiFi credential persistence
- **Touch UI System** - Button widgets with press/release detection
- **Serial Debugging** - UART-based logging for development

### Planned 🚧
- **WiFi Setup Screen** - QR code and error handling UI (infrastructure complete)
- **NTP Time Sync** - Automatic time synchronization over WiFi
- **Alarm System** - Configurable alarm with repeat days
- **I2S Audio** - Alarm sound playback
- **Hardware Snooze** - Physical button (GPIO 45)
- **Auto Brightness** - BH1750 ambient light sensor integration
- **Touch UI** - Modern interface with buttons, keyboards, and animations

## Hardware Specifications

### CrowPanel ESP32-S3 5-inch Display
- **MCU**: ESP32-S3-WROOM-1-N4R8 (4MB Flash, 8MB PSRAM)
- **Display**: 5.0" IPS TFT LCD, 800×480 resolution, RGB parallel interface
- **Touch**: GT911 capacitive touch controller (I2C)
- **Audio**: I2S interface (MAX98357A amplifier)
- **Light Sensor**: BH1750 ambient light sensor (I2C)
- **Storage**: MicroSD card slot (SPI)

### GPIO Pin Assignments

#### Display (RGB Parallel Interface)
```
Blue Channel (B0-B4):   GPIO 8, 3, 46, 9, 1
Green Channel (G0-G5):  GPIO 5, 6, 7, 15, 16, 4
Red Channel (R0-R4):    GPIO 45, 48, 47, 21, 14

Control Signals:
  DE (Data Enable):     GPIO 40
  VSYNC:                GPIO 41
  HSYNC:                GPIO 39
  PCLK (Pixel Clock):   GPIO 0
  Backlight (PWM):      GPIO 2
```

#### Peripherals
```
Touch (GT911):          SDA: GPIO 19, SCL: GPIO 20, INT: GPIO 38, RST: GPIO 21
I2S Audio:              BCLK: GPIO 42, LRCLK: GPIO 17, DOUT: GPIO 18
Light Sensor (BH1750):  SDA: GPIO 19, SCL: GPIO 20 (shared with touch)
Snooze Button:          GPIO 45 (active LOW, internal pullup)
SD Card:                MOSI: GPIO 11, MISO: GPIO 13, CLK: GPIO 12, CS: GPIO 10
```

## Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- USB-C cable for programming and power
- CrowPanel ESP32-S3 5-inch display

### Building the Project

```bash
# Clone the repository
git clone <your-repo-url>
cd cyd_alarm_clock

# Build the firmware
pio run

# Upload to device (hold BOOT button if needed)
pio run --target upload

# Monitor serial output
pio device monitor
```

### Upload Troubleshooting
- **Upload fails**: Hold the BOOT button while connecting USB, then release after upload starts
- **No serial output**: Ensure monitor_speed is 115200 in platformio.ini
- **Verification errors**: Upload speed is set to 115200 for stability

## Project Structure

```
cyd_alarm_clock/
├── include/
│   ├── LGFX_CrowPanel.h         # LovyanGFX display configuration
│   └── config.h                  # Project-wide configuration
├── src/
│   ├── main.cpp                  # Main application entry point
│   ├── DisplayManager.h/cpp      # Display abstraction layer
│   ├── TouchManager.h/cpp        # GT911 touch controller interface
│   ├── WiFiManager.h/cpp         # WiFi connection & BLE provisioning
│   ├── StorageManager.h/cpp      # NVS credential storage
│   └── UI/
│       ├── UIElement.h           # Base class for UI components
│       ├── Button.h/cpp          # Touch button widget
│       ├── QRCodeWidget.h/cpp    # QR code display widget
│       └── WiFiSetupScreen.h/cpp # WiFi provisioning UI
├── lib/
│   └── WiFiProv/                 # Patched WiFiProv library
├── platformio.ini                # Build configuration
├── CLAUDE.md                     # AI assistant guidance document
├── DEVELOPMENT_LOG.md            # Issues and solutions tracker
└── README.md                     # This file
```

## Development Phases

- [x] **Phase 1: Display Foundation** - RGB display with LovyanGFX ✅
- [x] **Phase 2: Touch Input System** - GT911 capacitive touch ✅
- [x] **Phase 3: WiFi Configuration** - BLE provisioning with credential storage ✅
- [ ] **Phase 4: Time Management** - NTP synchronization
- [ ] **Phase 5: Alarm System** - Alarm settings and triggering
- [ ] **Phase 6: Audio System** - I2S alarm sound playback
- [ ] **Phase 7: Brightness Control** - Auto-adjust with light sensor
- [ ] **Phase 8: State Management** - Screen navigation system
- [ ] **Phase 9: Polish & Optimization** - UI enhancements
- [ ] **Phase 10: Testing & Deployment** - Integration testing

## Technical Details

### Memory Configuration
- **PSRAM**: OPI PSRAM (qio_opi mode) for 768KB display framebuffer
- **Flash**: QIO mode at 80MHz
- **Partition**: Custom partition table (partitions.csv)

### Library Dependencies
- **LovyanGFX** (1.1.16) - Graphics library with ESP32-S3 RGB support ⚠️ **Must use 1.1.16** (1.2.7+ has PSRAM crash bug)
- **GT911** (TAMCTec/gt911-arduino) - Capacitive touch controller
- **WiFiProv** (bundled, patched) - BLE provisioning library
- **QRCode** (^0.0.1) - QR code generation for WiFi setup
- **Preferences** (built-in) - ESP32 NVS wrapper for credential storage
- **DNSServer** (^1.1.0) - WiFi captive portal (for future use)
- **ArduinoJson** (^6.21.5) - JSON parsing
- **NTPClient** (^3.2.1) - Network time synchronization
- **BH1750** (^1.3.0) - Light sensor

### Key Architectural Decisions

1. **Lazy Initialization**: Display object created in `begin()` after PSRAM initialization, not during global construction
2. **Manager Pattern**: Separate managers for display, touch, WiFi, time, alarm, and audio with state machines
3. **PSRAM Framebuffer**: Leverages 8MB PSRAM for smooth graphics rendering
4. **LovyanGFX Version Lock**: Must use 1.1.16 (1.2.7+ has breaking PSRAM allocation bug on ESP32-S3 OPI mode)
5. **BLE Provisioning**: Uses ESP-IDF WiFiProv library with security level 1 encryption
6. **UI Component System**: Base UIElement class with derived Button and screen widgets
7. **Serial via UART**: Debugging through GPIO43/44 (ARDUINO_USB_CDC_ON_BOOT=0)

## Troubleshooting

### Display Issues
- **Cascading lines/wrong colors**: Verify GPIO pin assignments match LGFX_CrowPanel.h
- **Black screen**: Check backlight PWM on GPIO 2
- **Bootloop**: Ensure PSRAM is initialized before display allocation

### Common Build Errors
- **sdkconfig.h missing**: Set `board_build.arduino.memory_type = qio_opi`
- **Panel_RGB not found**: Upgrade to platform espressif32@6.9.0 (Arduino 3.x)
- **Upload failures**: Reduce upload_speed to 115200

See [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) for detailed issue history and solutions.

## Development History

### Phase 1: Display Foundation
- **Issue #1**: TFT_eSPI incompatibility → Switched to LovyanGFX
- **Issue #2**: Framework version incompatibility → Upgraded to Arduino 3.x
- **Issue #3**: Missing SDK headers → Configured memory type correctly
- **Issue #5**: Upload verification failures → Reduced baud rate
- **Issue #6**: PSRAM bootloop → Implemented lazy initialization
- **Issue #8**: Wrong GPIO pins → Used official Elecrow configuration

### Phase 2: Touch Input
- Successfully integrated GT911 multi-touch controller
- Implemented edge detection for button press/release events
- Created UIElement base class and Button widget system

### Phase 3: WiFi Configuration
- **Critical Issue**: LovyanGFX 1.2.7 causes LoadProhibited crash at 0x80000000 during PSRAM framebuffer allocation
  - **Root Cause**: Breaking change in Bus_RGB memory allocation on ESP32-S3 OPI PSRAM
  - **Solution**: Locked to LovyanGFX 1.1.16 which has working PSRAM support
- Implemented BLE WiFi provisioning with ESP BLE Provisioning mobile app
- Created NVS-based credential storage with auto-reconnect
- Built WiFi setup screen with QR code widget

Full details in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md).

## Hardware Links

- [CrowPanel ESP32-S3 5" Display](https://www.elecrow.com/esp32-display-5-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html)
- [Official Documentation](https://www.elecrow.com/wiki/esp32-display-502727-intelligent-touch-screen-wi-fi26ble-800480-hmi-display.html)
- [Reference GitHub](https://github.com/Elecrow-RD/CrowPanel-5.0-HMI-ESP32-Display-800x480)

## License

This project is provided as-is for educational and personal use.

## Acknowledgments

- **Elecrow** - For the CrowPanel hardware and reference configurations
- **LovyanGFX** - Excellent graphics library with ESP32-S3 RGB panel support
- **ESP32 Community** - For extensive documentation and examples

## Current Status

**Phase 3 Complete** ✅ - WiFi provisioning system fully operational:
- BLE-based WiFi configuration via ESP BLE Provisioning mobile app
- NVS credential storage with auto-reconnect on boot
- WiFi state machine with setup screen UI
- Touch input system with button widgets

**Next**: Phase 4 - Time Management with NTP synchronization
