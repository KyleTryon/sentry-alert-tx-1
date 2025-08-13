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
    
    // Validation constants
    static const int MIN_THEME_INDEX = 0;
    static const int MAX_THEME_INDEX = 4; // 5 themes: 0-4
    
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
};

#endif // SETTINGSMANAGER_H
