#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <Arduino.h>
#include "../DisplayManager.h"
#include "../TouchManager.h"

// Base class for all UI elements
class UIElement {
public:
    UIElement(int32_t x, int32_t y, int32_t w, int32_t h)
        : _x(x), _y(y), _width(w), _height(h),
          _visible(true), _enabled(true) {}

    virtual ~UIElement() {}

    // Core methods - must be implemented by derived classes
    virtual void draw(DisplayManager* display) = 0;
    virtual bool onTouch(TouchPoint touch) = 0;

    // Hit testing
    bool hitTest(int32_t x, int32_t y) const {
        if (!_visible || !_enabled) return false;
        return ((x >= _x && x < _x + _width) &&
                (y >= _y && y < _y + _height));
    }

    // Position and size
    void setPosition(int32_t x, int32_t y) { _x = x; _y = y; }
    void setSize(int32_t w, int32_t h) { _width = w; _height = h; }
    void setBounds(int32_t x, int32_t y, int32_t w, int32_t h) {
        _x = x; _y = y; _width = w; _height = h;
    }

    int32_t getX() const { return _x; }
    int32_t getY() const { return _y; }
    int32_t getWidth() const { return _width; }
    int32_t getHeight() const { return _height; }

    // Visibility and enabled state
    void setVisible(bool visible) { _visible = visible; }
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isVisible() const { return _visible; }
    bool isEnabled() const { return _enabled; }

protected:
    int32_t _x, _y;
    int32_t _width, _height;
    bool _visible;
    bool _enabled;
};

#endif // UI_ELEMENT_H
