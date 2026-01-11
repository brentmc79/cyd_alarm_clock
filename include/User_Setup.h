// User_Setup.h for ESP32-S3 CrowPanel 5" Display
// RGB parallel interface configuration for TFT_eSPI library

#ifndef USER_SETUP_H
#define USER_SETUP_H

#define USER_SETUP_ID 502

// Driver selection - using ESP32 S3 RGB panel
#define ESP32_PARALLEL

// Display resolution
#define TFT_WIDTH  800
#define TFT_HEIGHT 480

// RGB interface configuration
#define TFT_RGB_BGR  // Color order (may need to toggle between BGR/RGB)

// Backlight control
#define TFT_BL   2
#define TFT_BACKLIGHT_ON HIGH

// RGB parallel pins (16-bit parallel interface)
// Data pins D0-D15
#define TFT_D0   0
#define TFT_D1   1
#define TFT_D2   2
#define TFT_D3   3
#define TFT_D4   4
#define TFT_D5   5
#define TFT_D6   6
#define TFT_D7   7
#define TFT_D8   8
#define TFT_D9   9
#define TFT_D10  10
#define TFT_D11  11
#define TFT_D12  12
#define TFT_D13  13
#define TFT_D14  14
#define TFT_D15  15

// Control pins
#define TFT_HSYNC  39  // Horizontal sync
#define TFT_VSYNC  41  // Vertical sync
#define TFT_DE     40  // Data enable (HENABLE)
#define TFT_PCLK   42  // Pixel clock

// Display timing parameters (may need adjustment based on datasheet)
#define TFT_HBPORCH  40   // Horizontal back porch
#define TFT_HFPORCH  10   // Horizontal front porch
#define TFT_HSYNC_W  10   // HSYNC width

#define TFT_VBPORCH  8    // Vertical back porch
#define TFT_VFPORCH  8    // Vertical front porch
#define TFT_VSYNC_W  2    // VSYNC width

// Pixel clock frequency (MHz) - adjust if display has issues
#define TFT_PCLK_HZ  16000000  // 16MHz (can try 14MHz-18MHz range)

// Enable PSRAM for frame buffer
#define USE_PSRAM_FRAME_BUFFER

// Font support
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH
#define LOAD_FONT8  // Font 8. Large 75 pixel font, needs ~3256 bytes in FLASH

#define SMOOTH_FONT  // Enable anti-aliased fonts

// SPI not used for RGB parallel display
#define SPI_FREQUENCY  27000000  // Not used but required by library

#endif // USER_SETUP_H
