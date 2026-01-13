#include "QRCodeWidget.h"

QRCodeWidget::QRCodeWidget(int32_t x, int32_t y, int32_t size)
    : UIElement(x, y, size, size),
      _qrcodeData(nullptr),
      _scale(4),
      _fgColor(TFT_BLACK),
      _bgColor(TFT_WHITE),
      _generated(false),
      _version(6) {  // Version 6 = 41x41 modules, good for ~70 chars

    // Allocate buffer for QR code data
    // Buffer size formula: qrcode_getBufferSize(version)
    uint16_t bufferSize = qrcode_getBufferSize(_version);
    _qrcodeData = new uint8_t[bufferSize];

    if (!_qrcodeData) {
        Serial.println("ERROR: Failed to allocate QR code buffer");
    }
}

QRCodeWidget::~QRCodeWidget() {
    if (_qrcodeData) {
        delete[] _qrcodeData;
        _qrcodeData = nullptr;
    }
}

bool QRCodeWidget::generate(const char* text) {
    if (!_qrcodeData) {
        Serial.println("ERROR: QR code buffer not allocated");
        return false;
    }

    Serial.printf("Generating QR code for text (length %d)\n", strlen(text));

    // Generate QR code
    int8_t result = qrcode_initText(&_qrcode, _qrcodeData, _version, ECC_LOW, text);

    if (result != 0) {
        Serial.printf("ERROR: QR code generation failed with code %d\n", result);
        _generated = false;
        return false;
    }

    Serial.printf("QR code generated successfully - Size: %dx%d\n", _qrcode.size, _qrcode.size);
    _generated = true;

    return true;
}

void QRCodeWidget::setScale(uint8_t scale) {
    if (scale >= 1 && scale <= 10) {
        _scale = scale;
    }
}

void QRCodeWidget::setColors(uint32_t fg, uint32_t bg) {
    _fgColor = fg;
    _bgColor = bg;
}

void QRCodeWidget::draw(DisplayManager* display) {
    if (!_visible || !display || !_generated) {
        return;
    }

    // Draw white background for QR code area
    display->fillRect(_x, _y, _width, _height, _bgColor);

    // Calculate actual QR code size with scaling
    uint16_t qrPixelSize = _qrcode.size * _scale;

    // Center QR code within widget bounds
    int32_t offsetX = _x + (_width - qrPixelSize) / 2;
    int32_t offsetY = _y + (_height - qrPixelSize) / 2;

    // Iterate through QR code modules and draw
    for (uint8_t y = 0; y < _qrcode.size; y++) {
        for (uint8_t x = 0; x < _qrcode.size; x++) {
            // Check if this module is dark (true = black module)
            if (qrcode_getModule(&_qrcode, x, y)) {
                // Draw scaled rectangle for this module
                int32_t px = offsetX + (x * _scale);
                int32_t py = offsetY + (y * _scale);
                display->fillRect(px, py, _scale, _scale, _fgColor);
            }
        }
    }
}

bool QRCodeWidget::onTouch(TouchPoint touch) {
    // QR codes don't respond to touch
    return false;
}
