#include "SettingsManager.h"
#if defined(__has_include)
#  if __has_include("generated_secrets.h")
#    include "generated_secrets.h"
#  elif __has_include("src/config/generated_secrets.h")
#    include "src/config/generated_secrets.h"
#  endif
#endif

// Static member definitions
Preferences SettingsManager::prefs;
const char* SettingsManager::NAMESPACE = "alerttx1";
const char* SettingsManager::THEME_KEY = "theme_idx";
const char* SettingsManager::WIFI_SSID_KEY = "wifi_ssid";
const char* SettingsManager::WIFI_PASSWORD_KEY = "wifi_pass";
const char* SettingsManager::MQTT_BROKER_KEY = "mqtt_host";
const char* SettingsManager::MQTT_PORT_KEY = "mqtt_port";
const char* SettingsManager::MQTT_CLIENT_ID_KEY = "mqtt_cid";
const char* SettingsManager::MQTT_SUB_TOPIC_KEY = "mqtt_sub";
const char* SettingsManager::MQTT_PUB_TOPIC_KEY = "mqtt_pub";
const char* SettingsManager::PWR_INACT_MS_KEY = "pwr_inact_ms";
const char* SettingsManager::PWR_DIM_GRACE_MS_KEY = "pwr_dim_ms";
const char* SettingsManager::PWR_SLEEP_MS_KEY = "pwr_sleep_ms";

void SettingsManager::begin() {
    Serial.println("SettingsManager: Initializing NVS...");
    
    // Open namespace in read-write mode
    bool success = prefs.begin(NAMESPACE, false);
    
    if (success) {
        Serial.printf("SettingsManager: NVS namespace '%s' opened successfully\n", NAMESPACE);
        
        // Check if this is first run
        if (!isInitialized()) {
            Serial.println("SettingsManager: First run detected, initializing defaults");
            setThemeIndex(0); // Set default theme

            // Seed WiFi/MQTT from generated_secrets.h if provided at build time
            #ifdef ALERTTX1_ENV_WIFI_SSID
              if (String(ALERTTX1_ENV_WIFI_SSID).length() > 0) setWifiSsid(ALERTTX1_ENV_WIFI_SSID);
            #endif
            #ifdef ALERTTX1_ENV_WIFI_PASSWORD
              setWifiPassword(ALERTTX1_ENV_WIFI_PASSWORD);
            #endif
            #ifdef ALERTTX1_ENV_MQTT_BROKER
              if (String(ALERTTX1_ENV_MQTT_BROKER).length() > 0) setMqttBroker(ALERTTX1_ENV_MQTT_BROKER);
            #endif
            #ifdef ALERTTX1_ENV_MQTT_PORT
              setMqttPort(ALERTTX1_ENV_MQTT_PORT);
            #endif
            #ifdef ALERTTX1_ENV_MQTT_CLIENT_ID
              setMqttClientId(ALERTTX1_ENV_MQTT_CLIENT_ID);
            #endif
            #ifdef ALERTTX1_ENV_MQTT_SUBSCRIBE_TOPIC
              setMqttSubscribeTopic(ALERTTX1_ENV_MQTT_SUBSCRIBE_TOPIC);
            #endif
            #ifdef ALERTTX1_ENV_MQTT_PUBLISH_TOPIC
              setMqttPublishTopic(ALERTTX1_ENV_MQTT_PUBLISH_TOPIC);
            #endif
        }
        
        printDebugInfo();
    } else {
        Serial.println("ERROR: SettingsManager: Failed to open NVS namespace!");
        Serial.println("       Settings will not persist across reboots");
    }
}

void SettingsManager::end() {
    prefs.end();
    Serial.println("SettingsManager: NVS namespace closed");
}

int SettingsManager::getThemeIndex() {
    // Use -1 as impossible value to detect read errors
    int savedIndex = prefs.getInt(THEME_KEY, -1);
    
    if (savedIndex == -1) {
        Serial.println("SettingsManager: No saved theme found, using default (0)");
        return 0; // Default theme
    }
    
    if (!isValidThemeIndex(savedIndex)) {
        Serial.printf("SettingsManager: Invalid saved theme %d, using default (0)\n", savedIndex);
        // Don't save the default here - let the user's next selection save properly
        return 0; // Default theme
    }
    
    Serial.printf("SettingsManager: Loaded saved theme: %d\n", savedIndex);
    return savedIndex;
}

bool SettingsManager::setThemeIndex(int index) {
    if (!isValidThemeIndex(index)) {
        Serial.printf("SettingsManager: Invalid theme index %d, not saving (valid: %d-%d)\n", 
                     index, MIN_THEME_INDEX, MAX_THEME_INDEX);
        return false;
    }
    
    // Save to NVS
    size_t bytesWritten = prefs.putInt(THEME_KEY, index);
    
    if (bytesWritten > 0) {
        Serial.printf("SettingsManager: Theme %d saved to NVS (%d bytes)\n", index, bytesWritten);
        return true;
    } else {
        Serial.printf("SettingsManager: Failed to save theme %d to NVS\n", index);
        return false;
    }
}

bool SettingsManager::isInitialized() {
    // Check if our test key exists
    return prefs.isKey(THEME_KEY);
}

void SettingsManager::resetToDefaults() {
    Serial.println("SettingsManager: Resetting all settings to defaults...");
    
    // Clear all keys in the namespace
    prefs.clear();
    
    // Set default values
    setThemeIndex(0);
    setWifiSsid(DEFAULT_WIFI_SSID);
    setWifiPassword(DEFAULT_WIFI_PASSWORD);
    setMqttBroker(DEFAULT_MQTT_BROKER);
    setMqttPort(DEFAULT_MQTT_PORT);
    setMqttClientId(DEFAULT_MQTT_CLIENT_ID);
    setMqttSubscribeTopic(DEFAULT_MQTT_SUB_TOPIC);
    setMqttPublishTopic(DEFAULT_MQTT_PUB_TOPIC);
    
    Serial.println("SettingsManager: Settings reset complete");
}

void SettingsManager::printDebugInfo() {
    Serial.println("=== SettingsManager Debug Info ===");
    Serial.printf("Namespace: %s\n", NAMESPACE);
    Serial.printf("Initialized: %s\n", isInitialized() ? "Yes" : "No");
    
    if (isInitialized()) {
        Serial.printf("Theme Index: %d\n", getThemeIndex());
    }
    
    // Get number of keys (if available in this ESP32 core version)
    size_t keyCount = 0;
    #ifdef ESP_ARDUINO_VERSION_MAJOR
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
    // Newer ESP32 core versions might have key enumeration
    #endif
    #endif
    
    Serial.printf("Available methods: getInt, putInt, isKey, clear, getString, putString\n");
    Serial.println("==================================");
}

bool SettingsManager::isValidThemeIndex(int index) {
    return (index >= MIN_THEME_INDEX && index <= MAX_THEME_INDEX);
}

// WiFi/MQTT getters
String SettingsManager::getWifiSsid() {
    String v = prefs.getString(WIFI_SSID_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_WIFI_SSID
        return String(ALERTTX1_ENV_WIFI_SSID);
        #else
        return String(DEFAULT_WIFI_SSID);
        #endif
    }
    return v;
}
String SettingsManager::getWifiPassword() {
    String v = prefs.getString(WIFI_PASSWORD_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_WIFI_PASSWORD
        return String(ALERTTX1_ENV_WIFI_PASSWORD);
        #else
        return String(DEFAULT_WIFI_PASSWORD);
        #endif
    }
    return v;
}
String SettingsManager::getMqttBroker() {
    String v = prefs.getString(MQTT_BROKER_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_MQTT_BROKER
        return String(ALERTTX1_ENV_MQTT_BROKER);
        #else
        return String(DEFAULT_MQTT_BROKER);
        #endif
    }
    return v;
}
int SettingsManager::getMqttPort() {
    int v = prefs.getInt(MQTT_PORT_KEY, -1);
    if (v <= 0) {
        #ifdef ALERTTX1_ENV_MQTT_PORT
        return ALERTTX1_ENV_MQTT_PORT;
        #else
        return DEFAULT_MQTT_PORT;
        #endif
    }
    return v;
}
String SettingsManager::getMqttClientId() {
    String v = prefs.getString(MQTT_CLIENT_ID_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_MQTT_CLIENT_ID
        return String(ALERTTX1_ENV_MQTT_CLIENT_ID);
        #else
        return String(DEFAULT_MQTT_CLIENT_ID);
        #endif
    }
    return v;
}
String SettingsManager::getMqttSubscribeTopic() {
    String v = prefs.getString(MQTT_SUB_TOPIC_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_MQTT_SUBSCRIBE_TOPIC
        return String(ALERTTX1_ENV_MQTT_SUBSCRIBE_TOPIC);
        #else
        return String(DEFAULT_MQTT_SUB_TOPIC);
        #endif
    }
    return v;
}
String SettingsManager::getMqttPublishTopic() {
    String v = prefs.getString(MQTT_PUB_TOPIC_KEY, "");
    if (v.length() == 0) {
        #ifdef ALERTTX1_ENV_MQTT_PUBLISH_TOPIC
        return String(ALERTTX1_ENV_MQTT_PUBLISH_TOPIC);
        #else
        return String(DEFAULT_MQTT_PUB_TOPIC);
        #endif
    }
    return v;
}

// WiFi/MQTT setters
void SettingsManager::setWifiSsid(const String& ssid) { prefs.putString(WIFI_SSID_KEY, ssid); }
void SettingsManager::setWifiPassword(const String& password) { prefs.putString(WIFI_PASSWORD_KEY, password); }
void SettingsManager::setMqttBroker(const String& broker) { prefs.putString(MQTT_BROKER_KEY, broker); }
void SettingsManager::setMqttPort(int port) { prefs.putInt(MQTT_PORT_KEY, port); }
void SettingsManager::setMqttClientId(const String& clientId) { prefs.putString(MQTT_CLIENT_ID_KEY, clientId); }
void SettingsManager::setMqttSubscribeTopic(const String& topic) { prefs.putString(MQTT_SUB_TOPIC_KEY, topic); }
void SettingsManager::setMqttPublishTopic(const String& topic) { prefs.putString(MQTT_PUB_TOPIC_KEY, topic); }

// Power management getters with sane defaults if unset
uint32_t SettingsManager::getInactivityTimeoutMs() {
    // Default to settings.h value if present; else 60000
    uint32_t v = (uint32_t)prefs.getULong(PWR_INACT_MS_KEY, 0);
    if (v == 0) {
        #ifdef INACTIVITY_TIMEOUT_MS
        return (uint32_t)INACTIVITY_TIMEOUT_MS;
        #else
        return 60000UL;
        #endif
    }
    return v;
}

uint32_t SettingsManager::getDimGraceMs() {
    uint32_t v = (uint32_t)prefs.getULong(PWR_DIM_GRACE_MS_KEY, 0);
    if (v == 0) {
        return 2000UL;
    }
    return v;
}

uint32_t SettingsManager::getDeepSleepIntervalMs() {
    uint32_t v = (uint32_t)prefs.getULong(PWR_SLEEP_MS_KEY, 0);
    if (v == 0) {
        return 60000UL;
    }
    return v;
}
