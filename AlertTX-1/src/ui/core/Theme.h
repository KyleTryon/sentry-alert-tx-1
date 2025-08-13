#ifndef THEME_H
#define THEME_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

/**
 * Theme System for Alert TX-1 UI Framework
 * 
 * Provides centralized color management for consistent theming
 * across all UI components. Themes are stored in PROGMEM for 
 * memory efficiency.
 */

struct Theme {
    // Background colors
    uint16_t background;        // Main background (screen background)
    uint16_t surfaceBackground; // Component backgrounds (menu item backgrounds)
    
    // Text colors  
    uint16_t primaryText;       // Main text color
    uint16_t secondaryText;     // Dimmed/secondary text
    uint16_t selectedText;      // Text color when item is selected
    
    // Accent colors
    uint16_t accent;            // Selection/highlight color (selection rectangle)
    uint16_t accentDark;        // Pressed/active state color
    uint16_t border;            // Borders and dividers
};

// Predefined Themes (stored in PROGMEM for memory efficiency)

// Default Theme
extern const Theme THEME_DEFAULT PROGMEM;

// High contrast theme for better visibility
extern const Theme THEME_HIGH_CONTRAST PROGMEM;

// Green terminal theme (retro computing style)
extern const Theme THEME_TERMINAL PROGMEM;

// Amber CRT theme (classic amber monitor style)
extern const Theme THEME_AMBER PROGMEM;

// Sentry purple theme (company branding colors)
extern const Theme THEME_SENTRY PROGMEM;

/**
 * Theme Manager - Enhanced singleton for global theme access with persistence
 * Integrates with SettingsManager for theme preference storage
 */
class ThemeManager {
private:
    static const Theme* currentTheme;
    static int currentThemeIndex;
    
    // Theme registry for index-based access
    static const Theme* themes[5];
    static const char* themeNames[5];
    static const char* themeDescriptions[5];
    
public:
    static const int THEME_COUNT = 5;
    
    // Core theme management
    static const Theme* getTheme() { return currentTheme; }
    static void setTheme(const Theme* theme) { currentTheme = theme; }
    static void begin() { currentTheme = &THEME_DEFAULT; currentThemeIndex = 0; }
    
    // Enhanced theme management with persistence integration
    static void loadFromSettings();
    static void setThemeByIndex(int index, bool persist = false);
    static int getCurrentThemeIndex() { return currentThemeIndex; }
    
    // Theme metadata access
    static const char* getCurrentThemeName();
    static const char* getThemeName(int index);
    static const char* getThemeDescription(int index);
    static const Theme* getThemeByIndex(int index);
    
    // Theme validation
    static bool isValidThemeIndex(int index);
    
    // Convenience getters for current theme colors
    static uint16_t getBackground() { return pgm_read_word(&currentTheme->background); }
    static uint16_t getSurfaceBackground() { return pgm_read_word(&currentTheme->surfaceBackground); }
    static uint16_t getPrimaryText() { return pgm_read_word(&currentTheme->primaryText); }
    static uint16_t getSecondaryText() { return pgm_read_word(&currentTheme->secondaryText); }
    static uint16_t getSelectedText() { return pgm_read_word(&currentTheme->selectedText); }
    static uint16_t getAccent() { return pgm_read_word(&currentTheme->accent); }
    static uint16_t getAccentDark() { return pgm_read_word(&currentTheme->accentDark); }
    static uint16_t getBorder() { return pgm_read_word(&currentTheme->border); }
};

#endif // THEME_H
