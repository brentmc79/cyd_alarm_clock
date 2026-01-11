# Development Log - ESP32-S3 CrowPanel Alarm Clock

This document tracks problems encountered during development and their solutions.

---

## Issue #1: TFT_eSPI Library Incompatibility
**Problem:** TFT_eSPI doesn't support true RGB parallel displays. Build failed with missing TFT_CASET, TFT_PASET, TFT_RAMWR constants.

**Solution:** Switched from TFT_eSPI to LovyanGFX library which has native ESP32-S3 RGB panel support via `Panel_RGB` and `Bus_RGB` classes.

---

## Issue #2: Arduino Framework Version Incompatibility
**Problem:** LovyanGFX's `Panel_RGB` class not available with Arduino ESP32 v2.0.3 framework.

**Solution:** Upgraded platform from Arduino ESP32 v2.0.3 to v6.9.0 (Arduino 3.x) which includes full RGB panel support.

---

## Issue #3: Missing sdkconfig.h Header
**Problem:** Build failed with "sdkconfig.h: No such file or directory" after framework upgrade.

**Solution:**
- Removed manual SDK path override
- Set correct memory type: `board_build.arduino.memory_type = qio_opi`
- Let PlatformIO auto-select the correct SDK variant based on memory configuration

---

## Issue #4: ESP8266Audio Library Incompatibility
**Problem:** ESP8266Audio library doesn't support Arduino ESP32 v3.x (missing driver/i2s_std.h headers).

**Solution:** Temporarily disabled ESP8266Audio in platformio.ini. Will need to find v3.x compatible audio library for Phase 6 or use alternative approach.

---

## Issue #5: Flash Upload Verification Failure
**Problem:** Device failed to verify flash chip connection at 921600 baud during upload.

**Solution:** Reduced upload speed to 115200 baud in platformio.ini. ESP32-S3 requires slower, more reliable upload speeds.

---

## Issue #6: Bootloop with LoadProhibited Exception
**Problem:** Device crashed with `LoadProhibited` exception at address `0xbfa32b6c` during boot, before setup() executed. Crashed accessing invalid memory.

**Root Cause:** LGFX object was being constructed as a global variable. The constructor tried to configure RGB panel and allocate 768KB framebuffer in PSRAM before PSRAM was initialized by the ESP32 runtime.

**Solution:**
- Changed DisplayManager to use lazy initialization pattern
- Constructor now creates `_display = nullptr` instead of `new LGFX()`
- Actual LGFX object allocation moved to `begin()` method, which runs after PSRAM init
- Added null checks in all DisplayManager methods
- Added PSRAM availability check before creating display object

**Key Learning:** Large memory allocations requiring PSRAM must happen after `setup()` begins, not during global object construction.

---

## Issue #7: No Serial Output
**Problem:** Code running but no Serial.print() output visible on UART monitor.

**Root Cause:** ARDUINO_USB_CDC_ON_BOOT=1 redirected Serial to USB CDC instead of UART.

**Solution:** Set ARDUINO_USB_CDC_ON_BOOT=0 in platformio.ini build flags. ESP32-S3 has separate USB and UART interfaces, and this flag controls where Serial output goes.

**Key Learning:** For debugging with traditional UART (GPIO43/44), disable USB CDC mode.

---

## Issue #8: Display Shows Cascading Lines Instead of Colors
**Problem:** After fixing bootloop, display showed cascading lines scrolling down the screen with no proper colors or graphics. Brightness PWM working correctly, but display output incorrect.

**Symptoms:**
- Lines cascading down the screen
- No recognizable colors or patterns
- Brightness control working (backlight responding correctly)
- Serial shows "Display initialized successfully"

**Root Cause:** **Two critical errors:**
1. **All 16 RGB GPIO pins were incorrect** - Pins from schematic analysis didn't match actual hardware
2. **DE and VSYNC pins were swapped** (GPIO 40 vs 41)
3. **Wrong timing parameters** - hsync/vsync porch values didn't match display requirements
4. **Missing `pclk_active_neg` setting** - Required for correct pixel clock polarity

**Solution:** Used official Elecrow GitHub repository configuration:
- Corrected all 16 RGB data pins:
  - Blue: GPIO 8, 3, 46, 9, 1
  - Green: GPIO 5, 6, 7, 15, 16, 4
  - Red: GPIO 45, 48, 47, 21, 14
- Fixed control signals:
  - DE (pin_henable): GPIO 40 (was 41)
  - VSYNC: GPIO 41 (was 40)
  - HSYNC: GPIO 39 (correct)
  - PCLK: GPIO 0 (correct)
- Updated timing parameters:
  - Pixel clock: 15MHz (was 16MHz)
  - HSYNC: front=8, pulse=4, back=43 (was 10, 10, 40)
  - VSYNC: front=8, pulse=4, back=12 (was 8, 2, 8)
- Added missing settings:
  - `pclk_active_neg = 1` (critical!)
  - `de_idle_high = 0` (was 1)

**Reference:** https://github.com/Elecrow-RD/CrowPanel-5.0-HMI-ESP32-Display-800x480

**Key Learning:** Always use manufacturer's official example code when available, rather than interpreting schematics. Schematics show connections but may not reflect actual GPIO routing or required timing parameters.

---

## Current Status
✅ **Phase 1 (Display Foundation) - COMPLETE**

Display is now working correctly with proper colors, graphics, and brightness control. All test patterns displaying successfully.

**Key Success Factors:**
- Used official Elecrow GitHub configuration instead of interpreting schematics
- Corrected all 16 RGB GPIO pins
- Fixed timing parameters (hsync/vsync porch values)
- Added critical `pclk_active_neg = 1` setting
- Pixel clock at 15MHz (not 16MHz)

**Next Phase:**
Ready to proceed to Phase 2 (Touch Input System) - GT911 capacitive touch controller integration.
