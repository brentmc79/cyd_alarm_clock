#include <Arduino.h>
#include "DisplayManager.h"

// Global display manager
DisplayManager display;

void displayTestPatterns() {
  // Clear screen to black
  display.clear(TFT_BLACK);
  delay(1000);

  // Test color bars
  int barWidth = display.width() / 7;
  display.fillRect(0 * barWidth, 0, barWidth, display.height(), TFT_RED);
  display.fillRect(1 * barWidth, 0, barWidth, display.height(), TFT_GREEN);
  display.fillRect(2 * barWidth, 0, barWidth, display.height(), TFT_BLUE);
  display.fillRect(3 * barWidth, 0, barWidth, display.height(), TFT_YELLOW);
  display.fillRect(4 * barWidth, 0, barWidth, display.height(), TFT_CYAN);
  display.fillRect(5 * barWidth, 0, barWidth, display.height(), TFT_MAGENTA);
  display.fillRect(6 * barWidth, 0, barWidth, display.height(), TFT_WHITE);

  Serial.println("Color bars displayed");
  delay(3000);

  // Test text rendering
  display.clear(TFT_BLACK);

  // Title
  display.setTextColor(TFT_WHITE);
  display.setTextFont(4);
  display.drawCentreString("ESP32-S3 Alarm Clock", display.width() / 2, 20);

  // Display info
  display.setTextFont(2);
  display.setTextColor(TFT_GREEN);
  display.setCursor(20, 80);
  display.print("Display: ");
  display.print(display.width());
  display.print(" x ");
  display.println(display.height());

  display.setTextColor(TFT_CYAN);
  display.setCursor(20, 110);
  display.println("CrowPanel 5-inch RGB Display");

  display.setCursor(20, 140);
  display.setTextColor(TFT_YELLOW);
  display.println("TFT_eSPI Library Initialized");

  // Test shapes
  display.setTextColor(TFT_WHITE);
  display.setCursor(20, 200);
  display.println("Testing Shapes:");

  display.drawRect(20, 230, 100, 80, TFT_RED);
  display.fillRect(140, 230, 100, 80, TFT_GREEN);
  display.drawCircle(310, 270, 40, TFT_BLUE);
  display.fillCircle(430, 270, 40, TFT_YELLOW);
  display.drawRoundRect(500, 230, 100, 80, 15, TFT_MAGENTA);
  display.fillRoundRect(620, 230, 100, 80, 15, TFT_CYAN);

  Serial.println("Test patterns displayed");
  delay(3000);

  // Test large font
  display.clear(TFT_BLACK);
  display.setTextFont(8);
  display.setTextColor(TFT_GREEN, TFT_BLACK);
  display.drawCentreString("12:34", display.width() / 2, 150);

  display.setTextFont(4);
  display.setTextColor(TFT_CYAN, TFT_BLACK);
  display.drawCentreString("Phase 1: Display Test", display.width() / 2, 300);

  Serial.println("Large font test displayed");
}

void testBrightness() {
  Serial.println("Testing backlight brightness...");

  display.clear(TFT_BLACK);
  display.setTextFont(4);
  display.setTextColor(TFT_WHITE);
  display.drawCentreString("Brightness Test", display.width() / 2, display.height() / 2 - 20);

  // Fade in
  for (int i = 0; i <= 255; i += 5) {
    display.setBrightness(i);
    delay(20);
  }

  delay(1000);

  // Fade out
  for (int i = 255; i >= 0; i -= 5) {
    display.setBrightness(i);
    delay(20);
  }

  delay(500);

  // Back to full brightness
  display.setBrightness(255);
  Serial.println("Brightness test complete");
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);  // Give serial time to initialize

  Serial.println("\n\n=== ESP32-S3 Alarm Clock - Phase 1 Test ===");
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  Serial.printf("Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());

  Serial.println("\nInitializing display...");

  // Initialize display
  if (display.begin()) {
    Serial.println("Display initialized successfully!");
    Serial.printf("Display size: %d x %d\n", display.width(), display.height());
    Serial.printf("Brightness: %d\n", display.getBrightness());

    // Run test patterns
    displayTestPatterns();

    // Test brightness control
    testBrightness();

    // Final display
    display.clear(TFT_BLACK);
    display.setTextFont(4);
    display.setTextColor(TFT_GREEN);
    display.drawCentreString("Display Test", display.width() / 2, display.height() / 2 - 40);
    display.setTextColor(TFT_WHITE);
    display.setTextFont(2);
    display.drawCentreString("PASS - Ready for next phase", display.width() / 2, display.height() / 2 + 20);

    Serial.println("\n=== Phase 1: Display Foundation - COMPLETE ===");
  } else {
    Serial.println("ERROR: Display initialization failed!");
  }
}

void loop() {
  // Cycle through brightness levels every 5 seconds for demo
  static unsigned long lastUpdate = 0;
  static uint8_t brightness = 255;
  static bool dimming = true;

  if (millis() - lastUpdate > 5000) {
    lastUpdate = millis();

    if (dimming) {
      brightness -= 50;
      if (brightness <= 50) {
        brightness = 50;
        dimming = false;
      }
    } else {
      brightness += 50;
      if (brightness >= 255) {
        brightness = 255;
        dimming = true;
      }
    }

    display.setBrightness(brightness);
    Serial.printf("Brightness adjusted to: %d\n", brightness);
  }

  delay(100);
}
