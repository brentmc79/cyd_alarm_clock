#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "LGFX_CrowPanel.h"

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();

    // Initialization
    bool begin();

    // Backlight control
    void setBrightness(uint8_t brightness);  // 0-255
    uint8_t getBrightness() const;
    void backlightOn();
    void backlightOff();

    // Display control
    void clear(uint32_t color = TFT_BLACK);
    void fillScreen(uint32_t color);

    // Drawing primitives
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color);

    // Text rendering
    void setTextColor(uint32_t color);
    void setTextColor(uint32_t fgColor, uint32_t bgColor);
    void setTextSize(float size);
    void setCursor(int32_t x, int32_t y);
    void setTextDatum(uint8_t datum);
    void print(const char* text);
    void print(int value);
    void println(const char* text);
    void println(int value);
    void drawString(const char* text, int32_t x, int32_t y);
    void drawCentreString(const char* text, int32_t x, int32_t y);
    void drawRightString(const char* text, int32_t x, int32_t y);

    // Font control
    void setFont(const lgfx::IFont* font);
    void setTextFont(uint8_t font);
    int16_t textWidth(const char* text);
    int16_t fontHeight();

    // Advanced features
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* data);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* data);

    // Display dimensions
    int32_t width() const;
    int32_t height() const;

    // Get underlying LGFX object for advanced operations
    LGFX* getLGFX();

private:
    LGFX* _display;
    uint8_t _brightness;
    bool _initialized;
};

#endif // DISPLAY_MANAGER_H
