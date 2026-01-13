# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO-based ESP32-S3 project targeting the Elecrow 5-inch CrowPanel display (ESP32-S3 based). This is an alarm clock application with WiFi connectivity, touch UI, and BLE provisioning.

**Current Status**: Phase 3 complete - WiFi provisioning system operational with auto-reconnect and credential storage.

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
- Arduino ESP32 v3.x (platform espressif32@6.9.0)
- Custom board definition for Elecrow hardware

## Critical Dependencies

### ⚠️ LovyanGFX Version Requirement
**MUST use LovyanGFX 1.1.16 exactly** - DO NOT upgrade to 1.2.7 or later.

**Reason**: LovyanGFX 1.2.7 introduced a breaking change in `Bus_RGB::init()` that causes a LoadProhibited crash at address `0x80000000` during PSRAM framebuffer allocation on ESP32-S3 OPI PSRAM mode. The framebuffer allocation in `heap_alloc_psram()` returns an invalid pointer, leading to immediate crashes during `_display->init()`.

**Symptoms if wrong version is used**:
- Device crashes with `Guru Meditation Error: Core 1 panic'ed (LoadProhibited)`
- EXCVADDR: `0x80000000` (invalid PSRAM address)
- Crash occurs during DisplayManager::begin() → _display->init()
- PSRAM test allocations succeed, but LovyanGFX framebuffer fails

**Fixed in**: Locked to `lovyan03/LovyanGFX @ 1.1.16` in platformio.ini

## Project Architecture

### Manager Pattern
The project uses a manager-based architecture with separate subsystems:

- **DisplayManager** (`src/DisplayManager.h/cpp`) - LovyanGFX abstraction, lazy LGFX initialization
- **TouchManager** (`src/TouchManager.h/cpp`) - GT911 capacitive touch with edge detection
- **WiFiManager** (`src/WiFiManager.h/cpp`) - WiFi connection state machine with BLE provisioning
- **StorageManager** (`src/StorageManager.h/cpp`) - NVS credential persistence

### UI Component System
- **UIElement** (`src/UI/UIElement.h`) - Base class for all UI components
- **Button** (`src/UI/Button.h/cpp`) - Touch-enabled button widget
- **QRCodeWidget** (`src/UI/QRCodeWidget.h/cpp`) - QR code display using ricmoo/QRCode
- **WiFiSetupScreen** (`src/UI/WiFiSetupScreen.h/cpp`) - WiFi provisioning UI

### WiFi Provisioning
- Uses ESP-IDF WiFiProv library (patched version in `lib/WiFiProv/`)
- BLE-based provisioning via "ESP BLE Provisioning" mobile app
- Security level 1 with proof-of-possession: "abcd1234"
- Device name format: `PROV_ALARM_<MAC>`
- Credentials stored in NVS namespace "wifi_config"

## Important Implementation Details

### Display Initialization
- LGFX object created in `DisplayManager::begin()` **after** PSRAM check
- **Never** create LGFX as global variable - causes bootloop
- 768KB framebuffer allocated from 8MB OPI PSRAM
- RGB parallel interface requires specific pin configuration in `LGFX_CrowPanel.h`

### Touch Input
- GT911 uses I2C (SDA: GPIO 19, SCL: GPIO 20)
- Interrupt pin: GPIO 38, Reset pin: GPIO 21 (shared with Red R0)
- Edge detection: `wasTouched()` for press, `wasReleased()` for release
- Button widgets handle touch in their bounding boxes

### WiFi State Machine
States: IDLE → CHECKING_CREDS → (CONNECTING → CONNECTED) or (PROVISIONING → PROV_SUCCESS → CONNECTING)
- Auto-connects with saved credentials on boot
- Shows setup screen on PROVISIONING or FAILED states
- Credentials persist across reboots via NVS
