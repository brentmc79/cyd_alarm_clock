#include "WiFiManager.h"

// Static member initialization
WiFiManager* WiFiManager::_instance = nullptr;

WiFiManager::WiFiManager()
    : _state(WIFI_IDLE),
      _previousState(WIFI_IDLE),
      _stateCallback(nullptr),
      _initialized(false),
      _lastConnectAttempt(0),
      _connectRetries(0) {
    _instance = this;  // For static callback
}

WiFiManager::~WiFiManager() {
    if (_initialized) {
        WiFi.disconnect(true);
    }
    _instance = nullptr;
}

bool WiFiManager::begin() {
    Serial.println("WiFiManager::begin() - Starting initialization");

    if (_initialized) {
        Serial.println("WiFi already initialized");
        return true;
    }

    // Register WiFi event handler
    WiFi.onEvent(wifiEventHandler);

    _state = WIFI_CHECKING_CREDS;

    // Check for saved credentials
    if (loadCredentials()) {
        Serial.println("Found saved credentials, attempting connection");
        _state = WIFI_CONNECTING;
        WiFi.mode(WIFI_STA);
        WiFi.begin(_savedSSID.c_str(), _savedPassword.c_str());
        _lastConnectAttempt = millis();
        _connectRetries = 0;
    } else {
        Serial.println("No saved credentials, starting provisioning");
        _state = WIFI_PROVISIONING;
        startProvisioning();
    }

    _initialized = true;
    return true;
}

void WiFiManager::update() {
    if (!_initialized) return;

    // State transition detection
    if (_state != _previousState) {
        Serial.printf("WiFi state change: %d -> %d\n", _previousState, _state);
        if (_stateCallback) {
            _stateCallback(_state);
        }
        _previousState = _state;
    }

    // State machine
    switch (_state) {
        case WIFI_CONNECTING:
            handleConnecting();
            break;

        case WIFI_RECONNECTING:
            handleReconnecting();
            break;

        case WIFI_CONNECTED:
            // Monitor connection
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("Connection lost, reconnecting");
                _state = WIFI_RECONNECTING;
                _lastConnectAttempt = millis();
                _connectRetries = 0;
            }
            break;

        case WIFI_PROVISIONING:
            // Handled by event callbacks
            break;

        case WIFI_FAILED:
            // Waiting for user action
            break;

        default:
            break;
    }
}

void WiFiManager::handleConnecting() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        _state = WIFI_CONNECTED;
        _connectRetries = 0;
        Serial.println("WiFi connected!");
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    } else if (millis() - _lastConnectAttempt > CONNECT_TIMEOUT_MS) {
        // Timeout after 10 seconds
        _connectRetries++;
        Serial.printf("Connection attempt %d/%d failed (status: %d)\n",
                      _connectRetries, MAX_CONNECT_RETRIES, status);

        if (_connectRetries >= MAX_CONNECT_RETRIES) {
            Serial.println("Connection failed after max retries");
            _state = WIFI_FAILED;
            WiFi.disconnect();
        } else {
            Serial.printf("Retrying connection (%d/%d)\n", _connectRetries + 1, MAX_CONNECT_RETRIES);
            WiFi.disconnect();
            delay(100);
            WiFi.begin(_savedSSID.c_str(), _savedPassword.c_str());
            _lastConnectAttempt = millis();
        }
    }
}

void WiFiManager::handleReconnecting() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        _state = WIFI_CONNECTED;
        _connectRetries = 0;
        Serial.println("Reconnected to WiFi");
    } else if (millis() - _lastConnectAttempt > CONNECT_TIMEOUT_MS) {
        _connectRetries++;
        Serial.printf("Reconnection attempt %d/%d failed\n", _connectRetries, MAX_CONNECT_RETRIES);

        if (_connectRetries >= MAX_CONNECT_RETRIES) {
            Serial.println("Reconnection failed, entering failed state");
            _state = WIFI_FAILED;
            WiFi.disconnect();
        } else {
            WiFi.reconnect();
            _lastConnectAttempt = millis();
        }
    }
}

void WiFiManager::startProvisioning() {
    Serial.println("WiFiManager::startProvisioning()");

    // Generate unique device name with chip MAC
    uint64_t chipid = ESP.getEfuseMac();
    String deviceName = "PROV_ALARM_" + String((uint32_t)(chipid >> 32), HEX);

    const char* pop = "abcd1234";  // Proof of possession
    const char* service_name = deviceName.c_str();

    // Generate random UUID for this provisioning session
    uint8_t uuid[16];
    for (int i = 0; i < 16; i++) {
        uuid[i] = random(0, 255);
    }

    Serial.printf("Starting BLE provisioning with device name: %s\n", service_name);
    Serial.printf("Proof of Possession: %s\n", pop);

    // Start WiFi provisioning via BLE
    WiFiProv.beginProvision(
        WIFI_PROV_SCHEME_BLE,
        WIFI_PROV_SCHEME_HANDLER_FREE_BLE,  // Free BT memory after provisioning
        WIFI_PROV_SECURITY_1,               // Encrypted provisioning
        pop,
        service_name,
        NULL,          // service_key = NULL for BLE
        uuid,
        true           // reset_provisioned = true (clear old credentials)
    );

    // Note: WiFiProv.printQR() is not available in this version
    // We're using our own QRCodeWidget to display the App Store URL instead
    Serial.println("BLE provisioning started successfully");

    _state = WIFI_PROVISIONING;
}

void WiFiManager::stopProvisioning() {
    Serial.println("WiFiManager::stopProvisioning()");
    // Note: WiFiProv doesn't have an end() method in Arduino ESP32 v3.x
    // BLE is automatically freed with WIFI_PROV_SCHEME_HANDLER_FREE_BLE
}

void WiFiManager::resetCredentials() {
    Serial.println("WiFiManager::resetCredentials()");

    // Disconnect WiFi
    WiFi.disconnect(true);

    // Clear stored credentials
    StorageManager::clearWiFiCredentials();

    // Restart provisioning
    _state = WIFI_PROVISIONING;
    _connectRetries = 0;
    startProvisioning();
}

void WiFiManager::reconnect() {
    Serial.println("WiFiManager::reconnect()");

    if (_savedSSID.length() == 0) {
        Serial.println("No saved credentials to reconnect with");
        _state = WIFI_PROVISIONING;
        startProvisioning();
        return;
    }

    _state = WIFI_CONNECTING;
    _connectRetries = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(_savedSSID.c_str(), _savedPassword.c_str());
    _lastConnectAttempt = millis();
}

bool WiFiManager::loadCredentials() {
    return StorageManager::loadWiFiCredentials(_savedSSID, _savedPassword);
}

// State queries
WiFiState WiFiManager::getState() const {
    return _state;
}

bool WiFiManager::isConnected() const {
    return _state == WIFI_CONNECTED;
}

bool WiFiManager::isProvisioning() const {
    return _state == WIFI_PROVISIONING;
}

String WiFiManager::getSSID() const {
    if (_state == WIFI_CONNECTED) {
        return WiFi.SSID();
    }
    return _savedSSID;
}

int8_t WiFiManager::getRSSI() const {
    if (_state == WIFI_CONNECTED) {
        return WiFi.RSSI();
    }
    return 0;
}

IPAddress WiFiManager::getIP() const {
    return WiFi.localIP();
}

const char* WiFiManager::getStateString() const {
    switch (_state) {
        case WIFI_IDLE: return "Idle";
        case WIFI_CHECKING_CREDS: return "Checking credentials";
        case WIFI_CONNECTING: return "Connecting";
        case WIFI_CONNECTED: return "Connected";
        case WIFI_PROVISIONING: return "Provisioning";
        case WIFI_PROV_SUCCESS: return "Provisioning success";
        case WIFI_FAILED: return "Failed";
        case WIFI_RECONNECTING: return "Reconnecting";
        default: return "Unknown";
    }
}

void WiFiManager::onStateChange(StateChangeCallback callback) {
    _stateCallback = callback;
}

// Static WiFi event handler
void WiFiManager::wifiEventHandler(arduino_event_t* event) {
    if (!_instance) return;

    switch (event->event_id) {
        case ARDUINO_EVENT_PROV_START:
            Serial.println("[WiFi Event] Provisioning started");
            break;

        case ARDUINO_EVENT_PROV_CRED_RECV:
            Serial.println("[WiFi Event] Received WiFi credentials");
            Serial.printf("  SSID: %s\n", (const char*)event->event_info.prov_cred_recv.ssid);
            // Don't log password for security

            // Save credentials
            _instance->_savedSSID = String((const char*)event->event_info.prov_cred_recv.ssid);
            _instance->_savedPassword = String((const char*)event->event_info.prov_cred_recv.password);

            StorageManager::saveWiFiCredentials(_instance->_savedSSID, _instance->_savedPassword);
            break;

        case ARDUINO_EVENT_PROV_CRED_SUCCESS:
            Serial.println("[WiFi Event] Provisioning successful!");
            _instance->stopProvisioning();
            _instance->_state = WIFI_PROV_SUCCESS;
            // Will transition to WIFI_CONNECTING in next update() cycle
            break;

        case ARDUINO_EVENT_PROV_CRED_FAIL:
            Serial.println("[WiFi Event] Provisioning failed - invalid credentials");
            _instance->_state = WIFI_FAILED;
            break;

        case ARDUINO_EVENT_PROV_END:
            Serial.println("[WiFi Event] Provisioning ended");
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("[WiFi Event] Got IP address");
            Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
            if (_instance->_state == WIFI_PROV_SUCCESS ||
                _instance->_state == WIFI_CONNECTING ||
                _instance->_state == WIFI_RECONNECTING) {
                _instance->_state = WIFI_CONNECTED;
            }
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[WiFi Event] Disconnected from WiFi");
            if (_instance->_state == WIFI_CONNECTED) {
                _instance->_state = WIFI_RECONNECTING;
                _instance->_lastConnectAttempt = millis();
                _instance->_connectRetries = 0;
            }
            break;

        default:
            break;
    }
}
