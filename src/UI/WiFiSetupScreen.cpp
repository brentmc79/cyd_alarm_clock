#include "WiFiSetupScreen.h"

// App Store URL for ESP BLE Provisioning app
const char* WiFiSetupScreen::APP_STORE_URL = "https://apps.apple.com/us/app/esp-ble-provisioning/id1473590141";

WiFiSetupScreen::WiFiSetupScreen()
    : UIElement(0, 0, 800, 480),  // Full screen
      _qrCode(nullptr),
      _retryButton(nullptr),
      _resetButton(nullptr),
      _showQR(true) {

    // Clear text buffers
    memset(_statusText, 0, sizeof(_statusText));
    memset(_errorText, 0, sizeof(_errorText));

    // Create QR code widget (centered, 200x200)
    _qrCode = new QRCodeWidget(300, 120, 200);
    _qrCode->setScale(4);  // 4x4 pixels per module
    _qrCode->setColors(TFT_BLACK, TFT_WHITE);
    _qrCode->generate(APP_STORE_URL);

    // Create Retry button (bottom-left)
    _retryButton = new Button(100, 400, 200, 60, "Retry");
    _retryButton->setColors(TFT_ORANGE, TFT_WHITE, TFT_DARKGREY);

    // Create Reset button (bottom-right)
    _resetButton = new Button(500, 400, 200, 60, "Reset WiFi");
    _resetButton->setColors(TFT_RED, TFT_WHITE, TFT_DARKGREY);
}

WiFiSetupScreen::~WiFiSetupScreen() {
    if (_qrCode) {
        delete _qrCode;
    }
    if (_retryButton) {
        delete _retryButton;
    }
    if (_resetButton) {
        delete _resetButton;
    }
}

void WiFiSetupScreen::draw(DisplayManager* display) {
    if (!_visible || !display) return;

    // Clear screen
    display->clear(TFT_BLACK);

    // Title
    display->setTextFont(4);
    display->setTextColor(TFT_CYAN);
    display->setTextDatum(TC_DATUM);  // Top-center
    display->drawString("WiFi Setup", 400, 20);

    // Instructions
    display->setTextFont(2);
    display->setTextColor(TFT_WHITE);
    display->setTextDatum(TC_DATUM);
    display->drawString("Scan QR code to download provisioning app", 400, 60);

    // QR code
    if (_showQR && _qrCode) {
        _qrCode->draw(display);

        // QR code label
        display->setTextFont(2);
        display->setTextColor(TFT_LIGHTGREY);
        display->setTextDatum(TC_DATUM);
        display->drawString("Download App", 400, 330);
    }

    // Status text
    if (strlen(_statusText) > 0) {
        display->setTextFont(2);
        display->setTextColor(TFT_YELLOW);
        display->setTextDatum(TC_DATUM);
        display->drawString(_statusText, 400, 360);
    }

    // Error text
    if (strlen(_errorText) > 0) {
        display->setTextFont(2);
        display->setTextColor(TFT_RED);
        display->setTextDatum(TC_DATUM);
        display->drawString(_errorText, 400, 380);
    }

    // Buttons
    if (_retryButton) _retryButton->draw(display);
    if (_resetButton) _resetButton->draw(display);
}

bool WiFiSetupScreen::onTouch(TouchPoint touch) {
    if (!_visible || !_enabled) return false;

    bool handled = false;

    // Route touch to buttons
    if (_retryButton) {
        handled |= _retryButton->onTouch(touch);
    }
    if (_resetButton) {
        handled |= _resetButton->onTouch(touch);
    }

    // Note: Button redraws are handled in main.cpp state change callback
    // to avoid null pointer issues

    return handled;
}

void WiFiSetupScreen::setStatus(const char* status) {
    if (status) {
        strncpy(_statusText, status, sizeof(_statusText) - 1);
        _statusText[sizeof(_statusText) - 1] = '\0';
    } else {
        _statusText[0] = '\0';
    }
}

void WiFiSetupScreen::setError(const char* error) {
    if (error) {
        strncpy(_errorText, error, sizeof(_errorText) - 1);
        _errorText[sizeof(_errorText) - 1] = '\0';
    } else {
        _errorText[0] = '\0';
    }
}

void WiFiSetupScreen::showQRCode(bool show) {
    _showQR = show;
}

void WiFiSetupScreen::setRetryCallback(ButtonCallback callback) {
    if (_retryButton) {
        _retryButton->setCallback(callback);
    }
}

void WiFiSetupScreen::setResetCallback(ButtonCallback callback) {
    if (_resetButton) {
        _resetButton->setCallback(callback);
    }
}
