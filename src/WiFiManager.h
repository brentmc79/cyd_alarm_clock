#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiProv.h>
#include "StorageManager.h"

// WiFi connection states
enum WiFiState {
    WIFI_IDLE,              // Not initialized
    WIFI_CHECKING_CREDS,    // Reading NVS for saved credentials
    WIFI_CONNECTING,        // Attempting WiFi connection
    WIFI_CONNECTED,         // Successfully connected
    WIFI_PROVISIONING,      // BLE provisioning active
    WIFI_PROV_SUCCESS,      // Credentials received via BLE
    WIFI_FAILED,            // Connection failed
    WIFI_RECONNECTING       // Lost connection, attempting reconnect
};

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();

    // Manager pattern
    bool begin();
    void update();  // Non-blocking state machine (call in loop)

    // State queries
    WiFiState getState() const;
    bool isConnected() const;
    bool isProvisioning() const;
    String getSSID() const;
    int8_t getRSSI() const;
    IPAddress getIP() const;
    const char* getStateString() const;

    // Actions
    void startProvisioning();
    void stopProvisioning();
    void resetCredentials();  // Clear NVS, restart provisioning
    void reconnect();  // Manual reconnect attempt

    // Callbacks for UI updates
    typedef void (*StateChangeCallback)(WiFiState newState);
    void onStateChange(StateChangeCallback callback);

private:
    WiFiState _state;
    WiFiState _previousState;
    StateChangeCallback _stateCallback;

    bool _initialized;
    unsigned long _lastConnectAttempt;
    uint8_t _connectRetries;
    static const uint8_t MAX_CONNECT_RETRIES = 3;
    static const unsigned long CONNECT_TIMEOUT_MS = 10000;  // 10 seconds

    String _savedSSID;
    String _savedPassword;

    // Internal methods
    bool loadCredentials();
    bool attemptConnection(uint32_t timeout_ms);
    void handleConnecting();
    void handleReconnecting();

    // Static callback for WiFi events
    static void wifiEventHandler(arduino_event_t* event);
    static WiFiManager* _instance;  // For static callback
};

#endif // WIFI_MANAGER_H
