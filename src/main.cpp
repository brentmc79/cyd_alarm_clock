#include <Arduino.h>
#include "DisplayManager.h"
#include "TouchManager.h"
#include "UI/Button.h"

// Global managers
DisplayManager display;
TouchManager touch;

// Test buttons
Button* button1 = nullptr;
Button* button2 = nullptr;
Button* button3 = nullptr;
Button* button4 = nullptr;

// Touch statistics
int touchCounter = 0;
int multiTouchCounter = 0;

// Button callbacks
void onButton1Press() {
  Serial.println("Button 1 pressed!");
  touchCounter++;

  display.setTextFont(2);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.drawString("Button 1 Pressed!  ", 480, 400);
}

void onButton2Press() {
  Serial.println("Button 2 pressed!");
  touchCounter++;

  display.setTextFont(2);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.drawString("Button 2 Pressed!  ", 480, 400);
}

void onButton3Press() {
  Serial.println("Button 3 pressed!");
  touchCounter++;

  display.setTextFont(2);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.drawString("Button 3 Pressed!  ", 480, 400);
}

void onButton4Press() {
  Serial.println("Button 4 pressed - Reset counter!");
  touchCounter = 0;
  multiTouchCounter = 0;

  display.setTextFont(2);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.drawString("Counters Reset!   ", 480, 400);
}

void drawUI() {
  display.clear(TFT_BLACK);

  // Title
  display.setTextFont(4);
  display.setTextColor(TFT_CYAN);
  display.drawCentreString("Phase 2: Touch Input Test", display.width() / 2, 20);

  // Instructions
  display.setTextFont(2);
  display.setTextColor(TFT_WHITE);
  display.drawString("Touch the screen or press buttons", 20, 60);

  // Draw all buttons
  if (button1) button1->draw(&display);
  if (button2) button2->draw(&display);
  if (button3) button3->draw(&display);
  if (button4) button4->draw(&display);

  // Status area (right side)
  display.setTextColor(TFT_YELLOW);
  display.drawString("Touch Status:", 480, 100);
  display.drawString("Touch Count:", 480, 150);
  display.drawString("Multi-Touch:", 480, 200);
}

void updateTouchDisplay() {
  // Clear previous touch indicators
  static int lastTouchCount = 0;

  if (touch.isTouched()) {
    uint8_t touchCount = touch.getTouchCount();

    // Track multi-touch
    if (touchCount > 1 && touch.wasTouched()) {
      multiTouchCounter++;
    }

    // Draw touch points
    for (uint8_t i = 0; i < touchCount; i++) {
      TouchPoint tp = touch.getTouch(i);
      if (tp.pressed) {
        // Draw crosshair at touch point
        display.drawLine(tp.x - 15, tp.y, tp.x + 15, tp.y, TFT_RED);
        display.drawLine(tp.x, tp.y - 15, tp.x, tp.y + 15, TFT_RED);
        display.fillCircle(tp.x, tp.y, 5, TFT_RED);

        // Display coordinates
        display.setTextFont(2);
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.drawString("                    ", 630, 100);  // Clear old text
        String coords = String(tp.x) + "," + String(tp.y);
        display.drawString(coords.c_str(), 630, 100);
      }
    }

    // Update touch count display
    display.setTextFont(2);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.drawString("      ", 630, 150);  // Clear
    display.drawString(String(touchCounter).c_str(), 630, 150);

    display.drawString("      ", 630, 200);  // Clear
    display.drawString(String(multiTouchCounter).c_str(), 630, 200);

    lastTouchCount = touchCount;
  } else {
    // No touch - clear status if needed
    if (lastTouchCount > 0) {
      display.setTextFont(2);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.drawString("No touch          ", 630, 100);
      lastTouchCount = 0;
    }
  }
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);  // Give serial time to initialize

  Serial.println("\n\n=== ESP32-S3 Alarm Clock - Phase 2 Test ===");
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());

  // Initialize display
  Serial.println("\nInitializing display...");
  if (!display.begin()) {
    Serial.println("ERROR: Display initialization failed!");
    return;
  }
  Serial.println("Display initialized successfully!");

  // Initialize touch
  Serial.println("\nInitializing touch controller...");
  if (!touch.begin()) {
    Serial.println("ERROR: Touch initialization failed!");
    display.clear(TFT_BLACK);
    display.setTextFont(4);
    display.setTextColor(TFT_RED);
    display.drawCentreString("Touch Init Failed!", display.width() / 2, display.height() / 2);
    return;
  }
  Serial.println("Touch initialized successfully!");

  // Create test buttons (left side, 2x2 grid)
  // Button dimensions: 140x60 pixels with large touch targets
  button1 = new Button(40, 120, 140, 60, "Button 1");
  button1->setColors(TFT_BLUE, TFT_WHITE, TFT_DARKGREY);
  button1->setCallback(onButton1Press);

  button2 = new Button(200, 120, 140, 60, "Button 2");
  button2->setColors(TFT_GREEN, TFT_BLACK, TFT_DARKGREY);
  button2->setCallback(onButton2Press);

  button3 = new Button(40, 200, 140, 60, "Button 3");
  button3->setColors(TFT_MAGENTA, TFT_WHITE, TFT_DARKGREY);
  button3->setCallback(onButton3Press);

  button4 = new Button(200, 200, 140, 60, "Reset");
  button4->setColors(TFT_RED, TFT_WHITE, TFT_DARKGREY);
  button4->setCallback(onButton4Press);

  // Draw initial UI
  drawUI();

  Serial.println("\n=== Phase 2 Touch Test Ready ===");
  Serial.println("Touch the screen or press buttons to test");
}

void loop() {
  // Update touch state
  touch.update();

  // Handle touch events - always pass to buttons (even on release)
  TouchPoint tp = touch.getTouch(0);  // Primary touch point

  // Pass touch to buttons
  if (button1) button1->onTouch(tp);
  if (button2) button2->onTouch(tp);
  if (button3) button3->onTouch(tp);
  if (button4) button4->onTouch(tp);

  // Redraw buttons if pressed state changed
  static bool lastPressed = false;
  bool anyPressed = (button1 && button1->isPressed()) ||
                    (button2 && button2->isPressed()) ||
                    (button3 && button3->isPressed()) ||
                    (button4 && button4->isPressed());

  if (anyPressed != lastPressed) {
    if (button1) button1->draw(&display);
    if (button2) button2->draw(&display);
    if (button3) button3->draw(&display);
    if (button4) button4->draw(&display);
    lastPressed = anyPressed;
  }

  // Update touch display (crosshairs and coordinates)
  updateTouchDisplay();

  delay(10);  // ~100 FPS update rate
}
