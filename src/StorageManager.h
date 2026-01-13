#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

// Static utility class for NVS (Non-Volatile Storage) operations
class StorageManager {
public:
    // WiFi credentials management
    static bool saveWiFiCredentials(const String& ssid, const String& password);
    static bool loadWiFiCredentials(String& ssid, String& password);
    static void clearWiFiCredentials();
    static bool isProvisioned();
    static void setProvisioned(bool provisioned);

    // Generic NVS helpers for future use
    static bool saveString(const char* ns, const char* key, const String& value);
    static String loadString(const char* ns, const char* key, const String& defaultValue = "");
    static bool saveBool(const char* ns, const char* key, bool value);
    static bool loadBool(const char* ns, const char* key, bool defaultValue = false);
    static bool saveUInt8(const char* ns, const char* key, uint8_t value);
    static uint8_t loadUInt8(const char* ns, const char* key, uint8_t defaultValue = 0);

private:
    static const char* WIFI_NAMESPACE;
    static const char* KEY_SSID;
    static const char* KEY_PASSWORD;
    static const char* KEY_PROVISIONED;
};

#endif // STORAGE_MANAGER_H
