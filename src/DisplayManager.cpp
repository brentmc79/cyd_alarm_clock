#include "DisplayManager.h"
#include <Arduino.h>

DisplayManager::DisplayManager()
    : _brightness(255), _initialized(false) {
    _display = nullptr;
}

DisplayManager::~DisplayManager() {
    if (_display) {
        delete _display;
    }
}

bool DisplayManager::begin() {
    Serial.println("DisplayManager::begin() - Starting initialization");

    if (_initialized) {
        Serial.println("Display already initialized");
        return true;
    }

    // Check PSRAM availability
    if (!psramFound()) {
        Serial.println("ERROR: PSRAM not found! RGB display requires PSRAM.");
        return false;
    }

    Serial.printf("PSRAM found: %d bytes free\n", ESP.getFreePsram());
    Serial.printf("Heap free: %d bytes\n", ESP.getFreeHeap());

    // Create LGFX object now that PSRAM is ready
    Serial.println("Creating LGFX object...");
    _display = new LGFX();

    if (!_display) {
        Serial.println("ERROR: Failed to allocate LGFX object");
        return false;
    }

    // Initialize display
    Serial.println("Calling _display->init()...");
    _display->init();

    Serial.println("Display init complete, setting rotation...");
    _display->setRotation(1);  // Landscape mode

    Serial.println("Filling screen black...");
    _display->fillScreen(TFT_BLACK);

    Serial.println("Setting brightness...");
    _display->setBrightness(_brightness);

    Serial.println("DisplayManager initialized successfully!");
    Serial.printf("Display size: %d x %d\n", _display->width(), _display->height());
    Serial.printf("PSRAM free after init: %d bytes\n", ESP.getFreePsram());

    _initialized = true;
    return true;
}

void DisplayManager::setBrightness(uint8_t brightness) {
    _brightness = brightness;
    if (_display) {
        _display->setBrightness(brightness);
    }
}

uint8_t DisplayManager::getBrightness() const {
    return _brightness;
}

void DisplayManager::backlightOn() {
    setBrightness(255);
}

void DisplayManager::backlightOff() {
    setBrightness(0);
}

void DisplayManager::clear(uint32_t color) {
    if (_display) _display->fillScreen(color);
}

void DisplayManager::fillScreen(uint32_t color) {
    if (_display) _display->fillScreen(color);
}

void DisplayManager::drawPixel(int32_t x, int32_t y, uint32_t color) {
    _display->drawPixel(x, y, color);
}

void DisplayManager::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    _display->drawLine(x0, y0, x1, y1, color);
}

void DisplayManager::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    _display->drawRect(x, y, w, h, color);
}

void DisplayManager::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    _display->fillRect(x, y, w, h, color);
}

void DisplayManager::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    _display->drawCircle(x, y, r, color);
}

void DisplayManager::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    _display->fillCircle(x, y, r, color);
}

void DisplayManager::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color) {
    _display->drawRoundRect(x, y, w, h, radius, color);
}

void DisplayManager::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color) {
    _display->fillRoundRect(x, y, w, h, radius, color);
}

void DisplayManager::setTextColor(uint32_t color) {
    _display->setTextColor(color);
}

void DisplayManager::setTextColor(uint32_t fgColor, uint32_t bgColor) {
    _display->setTextColor(fgColor, bgColor);
}

void DisplayManager::setTextSize(float size) {
    _display->setTextSize(size);
}

void DisplayManager::setCursor(int32_t x, int32_t y) {
    _display->setCursor(x, y);
}

void DisplayManager::setTextDatum(uint8_t datum) {
    _display->setTextDatum(datum);
}

void DisplayManager::print(const char* text) {
    _display->print(text);
}

void DisplayManager::print(int value) {
    _display->print(value);
}

void DisplayManager::println(const char* text) {
    _display->println(text);
}

void DisplayManager::println(int value) {
    _display->println(value);
}

void DisplayManager::drawString(const char* text, int32_t x, int32_t y) {
    _display->drawString(text, x, y);
}

void DisplayManager::drawCentreString(const char* text, int32_t x, int32_t y) {
    _display->drawCentreString(text, x, y);
}

void DisplayManager::drawRightString(const char* text, int32_t x, int32_t y) {
    _display->drawRightString(text, x, y);
}

void DisplayManager::setFont(const lgfx::IFont* font) {
    _display->setFont(font);
}

void DisplayManager::setTextFont(uint8_t font) {
    // LovyanGFX uses different font system
    // Map common font sizes to LovyanGFX fonts
    switch(font) {
        case 1: _display->setFont(&fonts::Font0); break;
        case 2: _display->setFont(&fonts::Font2); break;
        case 4: _display->setFont(&fonts::Font4); break;
        case 6: _display->setFont(&fonts::Font6); break;
        case 7: _display->setFont(&fonts::Font7); break;
        case 8: _display->setFont(&fonts::Font8); break;
        default: _display->setFont(&fonts::Font4); break;
    }
}

int16_t DisplayManager::textWidth(const char* text) {
    return _display->textWidth(text);
}

int16_t DisplayManager::fontHeight() {
    return _display->fontHeight();
}

void DisplayManager::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* data) {
    _display->pushImage(x, y, w, h, data);
}

void DisplayManager::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* data) {
    _display->pushImage(x, y, w, h, data);
}

int32_t DisplayManager::width() const {
    return _display->width();
}

int32_t DisplayManager::height() const {
    return _display->height();
}

LGFX* DisplayManager::getLGFX() {
    return _display;
}
