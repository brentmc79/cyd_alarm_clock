#ifndef BUTTON_H
#define BUTTON_H

#include "UIElement.h"

// Button callback type
typedef void (*ButtonCallback)(void);

class Button : public UIElement {
public:
    Button(int32_t x, int32_t y, int32_t w, int32_t h, const char* label = "");

    // UIElement interface
    void draw(DisplayManager* display) override;
    bool onTouch(TouchPoint touch) override;

    // Button-specific methods
    void setLabel(const char* label);
    const char* getLabel() const { return _label; }

    void setCallback(ButtonCallback callback) { _callback = callback; }

    void setColors(uint32_t bgColor, uint32_t fgColor, uint32_t pressedColor);
    void setRoundedCorners(int32_t radius) { _cornerRadius = radius; }

    bool isPressed() const { return _pressed; }

private:
    char _label[32];
    ButtonCallback _callback;

    // Visual properties
    uint32_t _bgColor;
    uint32_t _fgColor;
    uint32_t _pressedColor;
    int32_t _cornerRadius;

    // State
    bool _pressed;
    bool _wasPressed;  // For edge detection
};

#endif // BUTTON_H
