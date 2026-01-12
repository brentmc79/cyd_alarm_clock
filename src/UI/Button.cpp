#include "Button.h"

Button::Button(int32_t x, int32_t y, int32_t w, int32_t h, const char* label)
    : UIElement(x, y, w, h),
      _callback(nullptr),
      _bgColor(TFT_BLUE),
      _fgColor(TFT_WHITE),
      _pressedColor(TFT_DARKGREY),
      _cornerRadius(8),
      _pressed(false),
      _wasPressed(false) {
    setLabel(label);
}

void Button::setLabel(const char* label) {
    strncpy(_label, label ? label : "", sizeof(_label) - 1);
    _label[sizeof(_label) - 1] = '\0';
}

void Button::setColors(uint32_t bgColor, uint32_t fgColor, uint32_t pressedColor) {
    _bgColor = bgColor;
    _fgColor = fgColor;
    _pressedColor = pressedColor;
}

void Button::draw(DisplayManager* display) {
    if (!_visible || !display) return;

    // Draw button background
    uint32_t currentBgColor = _pressed ? _pressedColor : _bgColor;

    if (_cornerRadius > 0) {
        display->fillRoundRect(_x, _y, _width, _height, _cornerRadius, currentBgColor);
        display->drawRoundRect(_x, _y, _width, _height, _cornerRadius, _fgColor);
    } else {
        display->fillRect(_x, _y, _width, _height, currentBgColor);
        display->drawRect(_x, _y, _width, _height, _fgColor);
    }

    // Draw label text (centered)
    if (strlen(_label) > 0) {
        display->setTextColor(_fgColor);
        display->setTextDatum(MC_DATUM);  // Middle-center
        display->setTextFont(4);  // Medium font

        int32_t centerX = _x + _width / 2;
        int32_t centerY = _y + _height / 2;

        display->drawString(_label, centerX, centerY);
    }
}

bool Button::onTouch(TouchPoint touch) {
    if (!_visible || !_enabled) return false;

    // Check if touch is within button bounds
    bool hit = hitTest(touch.x, touch.y);

    // Update pressed state
    _wasPressed = _pressed;
    _pressed = hit && touch.pressed;

    if (_pressed && !_wasPressed) {
        Serial.printf("%d < %d < %d => %d\n", _x, touch.x, _x + _width, _pressed);
        Serial.printf("%d < %d < %d => %d\n", _y, touch.y, _y + _height, _pressed);
        Serial.printf("pressed: %d\n", ((touch.x >= _x && touch.x < _x + _width) && (touch.y >= _y && touch.y < _y + _height)));
    }

    // Trigger callback on release (rising edge of release)
    if (_wasPressed && !_pressed && _callback) {
        _callback();
        return true;  // Event consumed
    }

    return hit;  // Return true if we're handling this touch
}
