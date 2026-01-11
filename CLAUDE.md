# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO-based ESP32-S3 project targeting the Elecrow 5-inch CrowPanel display (ESP32-S3 based). The project appears to be set up for an alarm clock application on embedded hardware.

## Hardware Target

- **Board**: Custom Elecrow CrowPanel ESP32 5-inch display
- **MCU**: ESP32-S3 (dual-core @ 240MHz)
- **Memory**: 4MB flash, PSRAM enabled
- **Display**: 5-inch panel (configuration in `crowpanel_esp32_5in_display.json`)
- **Framework**: Arduino (ESP32 v2.0.3)

## Build System Commands

```bash
# Build the project
pio run

# Build for specific environment
pio run -e crowpanel_5in

# Upload to device
pio run -t upload

# Upload and monitor serial output
pio run -t upload -t monitor

# Clean build files
pio run -t clean

# Open serial monitor
pio device monitor
```

## Project Structure

- `src/main.cpp` - Main application code (Arduino setup/loop)
- `include/` - Project header files
- `lib/` - Project-specific libraries (compiled to static libraries)
- `platformio.ini` - PlatformIO configuration and build settings
- `partitions.csv` - ESP32 flash memory partition table
- `crowpanel_esp32_5in_display.json` - Custom board definition

## Memory Partitions

The project uses a custom partition scheme defined in `partitions.csv`:
- NVS (Non-Volatile Storage): 20KB for configuration
- OTA: Dual partition scheme for over-the-air updates (3MB app partition)
- SPIFFS: 896KB for filesystem storage
- Core dump: 64KB for crash debugging

## Key Build Flags

The board configuration includes:
- `BOARD_HAS_PSRAM` - External PSRAM available
- `ARDUINO_USB_MODE=1` - USB CDC enabled
- `ARDUINO_RUNNING_CORE=1` - Main Arduino tasks run on core 1
- `ARDUINO_EVENT_RUNNING_CORE=1` - Event handlers on core 1

## Arduino Framework

This project uses the Arduino framework on ESP32 with:
- Standard `setup()` and `loop()` structure
- Arduino ESP32 v2.0.3 locked version (via GitHub)
- Custom board definition for Elecrow hardware
