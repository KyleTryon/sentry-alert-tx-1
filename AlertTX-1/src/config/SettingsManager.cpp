#include "SettingsManager.h"

// Static member definitions
Preferences SettingsManager::prefs;
const char* SettingsManager::NAMESPACE = "alerttx1";
const char* SettingsManager::THEME_KEY = "theme_idx";

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
    
    Serial.printf("Available methods: getInt, putInt, isKey, clear\n");
    Serial.println("==================================");
}

bool SettingsManager::isValidThemeIndex(int index) {
    return (index >= MIN_THEME_INDEX && index <= MAX_THEME_INDEX);
}
