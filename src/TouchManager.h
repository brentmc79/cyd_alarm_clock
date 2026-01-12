#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <TAMC_GT911.h>

// Touch event structure
struct TouchPoint {
    int16_t x;
    int16_t y;
    bool pressed;
    uint8_t id;  // For multi-touch tracking
};

class TouchManager {
public:
    TouchManager();
    ~TouchManager();

    // Initialization
    bool begin();

    // Touch reading
    void update();  // Call this in main loop
    bool isTouched() const;
    uint8_t getTouchCount() const;
    TouchPoint getTouch(uint8_t index = 0) const;

    // Convenience methods
    bool wasTouched();  // True once when first touched
    bool wasReleased(); // True once when released

    // Raw GT911 access for advanced use
    TAMC_GT911* getController();

private:
    TAMC_GT911* _touch;

    // Touch state tracking
    TouchPoint _touchPoints[5];  // GT911 supports up to 5 points
    uint8_t _touchCount;
    bool _currentlyTouched;
    bool _previouslyTouched;

    // Debouncing
    unsigned long _lastTouchTime;
    static const unsigned long DEBOUNCE_MS = 50;

    bool _initialized;
};

#endif // TOUCH_MANAGER_H
