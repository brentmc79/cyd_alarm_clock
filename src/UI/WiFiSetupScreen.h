#ifndef WIFI_SETUP_SCREEN_H
#define WIFI_SETUP_SCREEN_H

#include "UIElement.h"
#include "Button.h"
#include "QRCodeWidget.h"

// WiFi setup screen with QR code and status
class WiFiSetupScreen : public UIElement {
public:
    WiFiSetupScreen();
    ~WiFiSetupScreen();

    // UIElement interface
    void draw(DisplayManager* display) override;
    bool onTouch(TouchPoint touch) override;

    // Screen-specific methods
    void setStatus(const char* status);
    void setError(const char* error);
    void showQRCode(bool show);

    // Button callbacks
    void setRetryCallback(ButtonCallback callback);
    void setResetCallback(ButtonCallback callback);

private:
    QRCodeWidget* _qrCode;
    Button* _retryButton;
    Button* _resetButton;

    char _statusText[64];
    char _errorText[64];
    bool _showQR;

    static const char* APP_STORE_URL;
};

#endif // WIFI_SETUP_SCREEN_H
