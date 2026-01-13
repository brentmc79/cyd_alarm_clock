#ifndef QRCODE_WIDGET_H
#define QRCODE_WIDGET_H

#include "UIElement.h"
#include <qrcode.h>

// QR Code display widget
class QRCodeWidget : public UIElement {
public:
    QRCodeWidget(int32_t x, int32_t y, int32_t size);
    ~QRCodeWidget();

    // UIElement interface
    void draw(DisplayManager* display) override;
    bool onTouch(TouchPoint touch) override;  // No-op for QR codes

    // QR code specific
    bool generate(const char* text);
    void setScale(uint8_t scale);  // Pixel multiplier (1-10), default 4
    void setColors(uint32_t fg, uint32_t bg);

private:
    QRCode _qrcode;
    uint8_t* _qrcodeData;
    uint8_t _scale;
    uint32_t _fgColor;
    uint32_t _bgColor;
    bool _generated;
    uint8_t _version;  // QR code version (determines size)
};

#endif // QRCODE_WIDGET_H
