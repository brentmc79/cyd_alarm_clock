#include "TouchManager.h"

// GT911 GPIO pins from CrowPanel hardware
#define TOUCH_SDA     19
#define TOUCH_SCL     20
#define TOUCH_INT     38
#define TOUCH_RST     21
#define TOUCH_WIDTH   800
#define TOUCH_HEIGHT  480

TouchManager::TouchManager()
    : _touch(nullptr),
      _touchCount(0),
      _currentlyTouched(false),
      _previouslyTouched(false),
      _lastTouchTime(0),
      _initialized(false) {
    // Initialize touch points
    for (int i = 0; i < 5; i++) {
        _touchPoints[i] = {0, 0, false, 0};
    }
}

TouchManager::~TouchManager() {
    if (_touch) {
        delete _touch;
    }
}

bool TouchManager::begin() {
    Serial.println("TouchManager::begin() - Starting initialization");

    if (_initialized) {
        Serial.println("Touch already initialized");
        return true;
    }

    // Initialize I2C
    Wire.begin(TOUCH_SDA, TOUCH_SCL);

    // Create GT911 controller object
    Serial.println("Creating GT911 object...");
    _touch = new TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

    if (!_touch) {
        Serial.println("ERROR: Failed to allocate GT911 object");
        return false;
    }

    // Initialize the touch controller
    Serial.println("Initializing GT911...");
    _touch->begin();

    // Set rotation to match display (rotation 0 = native landscape)
    // Touch is 180 degrees off, so use ROTATION_INVERTED (1)
    _touch->setRotation(ROTATION_INVERTED);

    Serial.println("TouchManager initialized successfully!");
    Serial.printf("Touch resolution: %d x %d\n", TOUCH_WIDTH, TOUCH_HEIGHT);

    _initialized = true;
    return true;
}

void TouchManager::update() {
    if (!_initialized || !_touch) return;

    // Read touch data
    _touch->read();

    // Store previous state
    _previouslyTouched = _currentlyTouched;

    // Check if currently touched
    _currentlyTouched = _touch->isTouched;
    _touchCount = _touch->touches;

    // Update touch points
    if (_currentlyTouched && _touchCount > 0) {
        // Debounce check
        unsigned long now = millis();
        if (now - _lastTouchTime < DEBOUNCE_MS) {
            return;
        }
        _lastTouchTime = now;

        // Read up to 5 touch points
        for (uint8_t i = 0; i < _touchCount && i < 5; i++) {
            _touchPoints[i].x = _touch->points[i].x;
            _touchPoints[i].y = _touch->points[i].y;
            _touchPoints[i].pressed = true;
            _touchPoints[i].id = i;
        }
    } else {
        // No touch - clear all points
        for (int i = 0; i < 5; i++) {
            _touchPoints[i].pressed = false;
        }
        _touchCount = 0;
    }
}

bool TouchManager::isTouched() const {
    return _currentlyTouched;
}

uint8_t TouchManager::getTouchCount() const {
    return _touchCount;
}

TouchPoint TouchManager::getTouch(uint8_t index) const {
    if (index < 5) {
        return _touchPoints[index];
    }
    return {0, 0, false, 0};
}

bool TouchManager::wasTouched() {
    // True on rising edge (just touched)
    return _currentlyTouched && !_previouslyTouched;
}

bool TouchManager::wasReleased() {
    // True on falling edge (just released)
    return !_currentlyTouched && _previouslyTouched;
}

TAMC_GT911* TouchManager::getController() {
    return _touch;
}
