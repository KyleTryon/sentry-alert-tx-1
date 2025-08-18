#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Preferences.h>
#include <Arduino.h>

/**
 * SettingsManager
 * 
 * Provides persistent storage for user settings using ESP32 NVS (Non-Volatile Storage).
 * All settings are automatically saved and loaded across power cycles.
 * 
 * Features:
 * - Theme preference persistence
 * - Graceful fallback for corrupted/missing data
 * - Validation of saved values
 * - Debug logging for troubleshooting
 * 
 * Technical Details:
 * - Uses ESP32 Preferences library (official Espressif)
 * - NVS namespace: "alerttx1" (under 15 char limit)
 * - Automatic wear leveling and corruption protection
 * - Minimal memory footprint (~200 bytes)
 */

class SettingsManager {
private:
    static Preferences prefs;
    
    // NVS Configuration (must be under 15 characters)
    static const char* NAMESPACE;    // "alerttx1"
    static const char* THEME_KEY;    // "theme_idx"
    // Network/MQTT keys
    // (Network settings now build-time only; keys retained for future optional use)
    static const char* WIFI_SSID_KEY;       // "wifi_ssid"
    static const char* WIFI_PASSWORD_KEY;   // "wifi_pass"
    static const char* MQTT_BROKER_KEY;     // "mqtt_host"
    static const char* MQTT_PORT_KEY;       // "mqtt_port"
    static const char* MQTT_CLIENT_ID_KEY;  // "mqtt_cid"
    static const char* MQTT_SUB_TOPIC_KEY;  // "mqtt_sub"
    static const char* MQTT_PUB_TOPIC_KEY;  // "mqtt_pub"
    // Power management keys
    static const char* PWR_INACT_MS_KEY;     // "pwr_inact_ms"
    static const char* PWR_DIM_GRACE_MS_KEY; // "pwr_dim_ms"
    static const char* PWR_SLEEP_MS_KEY;     // "pwr_sleep_ms"
    
    // Validation constants
    static const int MIN_THEME_INDEX = 0;
    static const int MAX_THEME_INDEX = 4; // 5 themes: 0-4
    
    // Defaults
    static constexpr const char* DEFAULT_WIFI_SSID = "";
    static constexpr const char* DEFAULT_WIFI_PASSWORD = "";
    static constexpr const char* DEFAULT_MQTT_BROKER = "localhost";
    static const int DEFAULT_MQTT_PORT = 1883;
    static constexpr const char* DEFAULT_MQTT_CLIENT_ID = "AlertTX1";
    static constexpr const char* DEFAULT_MQTT_SUB_TOPIC = "alerttx1/messages";
    static constexpr const char* DEFAULT_MQTT_PUB_TOPIC = "alerttx1/status";
    
public:
    /**
     * Initialize the settings manager and open NVS namespace
     * Call this once during startup before any other operations
     */
    static void begin();
    
    /**
     * Close the NVS namespace (optional - auto-closes on reboot)
     * Can be called to free resources if needed
     */
    static void end();
    
    /**
     * Get the saved theme index with graceful fallback
     * @return Theme index (0-3) or 0 if invalid/not found
     */
    static int getThemeIndex();
    
    /**
     * Save theme index to persistent storage
     * @param index Theme index to save (0-3), validated before saving
     * @return true if successfully saved, false if invalid index
     */
    static bool setThemeIndex(int index);
    
    /**
     * Check if settings have been initialized (any key exists)
     * @return true if settings exist, false if first run
     */
    static bool isInitialized();
    
    /**
     * Reset all settings to defaults (clears all keys in namespace)
     * Use with caution - this will erase all saved preferences
     */
    static void resetToDefaults();
    
    /**
     * Get debug information about NVS usage
     * Prints available space, key count, etc. to Serial
     */
    static void printDebugInfo();
    
private:
    /**
     * Validate theme index is within acceptable range
     * @param index Theme index to validate
     * @return true if valid (0-3), false otherwise
     */
    static bool isValidThemeIndex(int index);

    // WiFi/MQTT persisted configuration
public:
    // Getters
    // Network getters are still available for read-only display if needed
    static String getWifiSsid();
    static String getWifiPassword();
    static String getMqttBroker();
    static int getMqttPort();
    static String getMqttClientId();
    static String getMqttSubscribeTopic();
    static String getMqttPublishTopic();

    // Setters
    // Setters no longer needed for immutable build-time config, but kept for potential future UI
    static void setWifiSsid(const String& ssid);
    static void setWifiPassword(const String& password);
    static void setMqttBroker(const String& broker);
    static void setMqttPort(int port);
    static void setMqttClientId(const String& clientId);
    static void setMqttSubscribeTopic(const String& topic);
    static void setMqttPublishTopic(const String& topic);

    // Power management (read-only for now; configurable later via UI)
    static uint32_t getInactivityTimeoutMs();
    static uint32_t getDimGraceMs();
    static uint32_t getDeepSleepIntervalMs();
};

#endif // SETTINGSMANAGER_H
