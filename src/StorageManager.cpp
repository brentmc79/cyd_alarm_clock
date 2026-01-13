#include "StorageManager.h"

// Static member definitions
const char* StorageManager::WIFI_NAMESPACE = "wifi_config";
const char* StorageManager::KEY_SSID = "ssid";
const char* StorageManager::KEY_PASSWORD = "password";
const char* StorageManager::KEY_PROVISIONED = "provisioned";

bool StorageManager::saveWiFiCredentials(const String& ssid, const String& password) {
    Serial.println("StorageManager::saveWiFiCredentials()");

    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, false)) {  // false = read/write mode
        Serial.println("ERROR: Failed to open NVS namespace for writing");
        return false;
    }

    // Save SSID and password
    size_t ssidLen = prefs.putString(KEY_SSID, ssid);
    size_t passLen = prefs.putString(KEY_PASSWORD, password);
    prefs.putBool(KEY_PROVISIONED, true);

    prefs.end();

    if (ssidLen == 0 || passLen == 0) {
        Serial.println("ERROR: Failed to write credentials to NVS");
        return false;
    }

    Serial.printf("Credentials saved - SSID: %s (length: %d)\n", ssid.c_str(), ssid.length());
    // Don't log password for security

    return true;
}

bool StorageManager::loadWiFiCredentials(String& ssid, String& password) {
    Serial.println("StorageManager::loadWiFiCredentials()");

    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, true)) {  // true = read-only mode
        Serial.println("ERROR: Failed to open NVS namespace for reading");
        return false;
    }

    // Check if provisioned
    if (!prefs.getBool(KEY_PROVISIONED, false)) {
        Serial.println("Device not provisioned");
        prefs.end();
        return false;
    }

    // Load SSID and password
    ssid = prefs.getString(KEY_SSID, "");
    password = prefs.getString(KEY_PASSWORD, "");

    prefs.end();

    if (ssid.length() == 0) {
        Serial.println("ERROR: SSID not found or empty");
        return false;
    }

    Serial.printf("Credentials loaded - SSID: %s\n", ssid.c_str());
    // Don't log password for security

    return true;
}

void StorageManager::clearWiFiCredentials() {
    Serial.println("StorageManager::clearWiFiCredentials()");

    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, false)) {
        Serial.println("ERROR: Failed to open NVS namespace for clearing");
        return;
    }

    prefs.clear();  // Clear all keys in namespace
    prefs.end();

    Serial.println("WiFi credentials cleared");
}

bool StorageManager::isProvisioned() {
    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, true)) {
        return false;
    }

    bool provisioned = prefs.getBool(KEY_PROVISIONED, false);
    prefs.end();

    return provisioned;
}

void StorageManager::setProvisioned(bool provisioned) {
    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, false)) {
        Serial.println("ERROR: Failed to open NVS namespace");
        return;
    }

    prefs.putBool(KEY_PROVISIONED, provisioned);
    prefs.end();

    Serial.printf("Provisioned flag set to: %d\n", provisioned);
}

// Generic NVS helpers
bool StorageManager::saveString(const char* ns, const char* key, const String& value) {
    Preferences prefs;
    if (!prefs.begin(ns, false)) {
        return false;
    }

    size_t len = prefs.putString(key, value);
    prefs.end();

    return len > 0;
}

String StorageManager::loadString(const char* ns, const char* key, const String& defaultValue) {
    Preferences prefs;
    if (!prefs.begin(ns, true)) {
        return defaultValue;
    }

    String value = prefs.getString(key, defaultValue);
    prefs.end();

    return value;
}

bool StorageManager::saveBool(const char* ns, const char* key, bool value) {
    Preferences prefs;
    if (!prefs.begin(ns, false)) {
        return false;
    }

    size_t len = prefs.putBool(key, value);
    prefs.end();

    return len > 0;
}

bool StorageManager::loadBool(const char* ns, const char* key, bool defaultValue) {
    Preferences prefs;
    if (!prefs.begin(ns, true)) {
        return defaultValue;
    }

    bool value = prefs.getBool(key, defaultValue);
    prefs.end();

    return value;
}

bool StorageManager::saveUInt8(const char* ns, const char* key, uint8_t value) {
    Preferences prefs;
    if (!prefs.begin(ns, false)) {
        return false;
    }

    size_t len = prefs.putUChar(key, value);
    prefs.end();

    return len > 0;
}

uint8_t StorageManager::loadUInt8(const char* ns, const char* key, uint8_t defaultValue) {
    Preferences prefs;
    if (!prefs.begin(ns, true)) {
        return defaultValue;
    }

    uint8_t value = prefs.getUChar(key, defaultValue);
    prefs.end();

    return value;
}
