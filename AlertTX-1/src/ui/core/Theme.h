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

// Default FlipperZero-inspired theme (current working theme)
extern const Theme THEME_DEFAULT PROGMEM;

// High contrast theme for better visibility
extern const Theme THEME_HIGH_CONTRAST PROGMEM;

// Green terminal theme (retro computing style)
extern const Theme THEME_TERMINAL PROGMEM;

// Amber CRT theme (classic amber monitor style)
extern const Theme THEME_AMBER PROGMEM;

/**
 * Theme Manager - Simple singleton for global theme access
 * Most memory efficient approach for embedded systems
 */
class ThemeManager {
private:
    static const Theme* currentTheme;
    
public:
    // Get current active theme
    static const Theme* getTheme() { return currentTheme; }
    
    // Set new theme (pass PROGMEM theme)
    static void setTheme(const Theme* theme) { currentTheme = theme; }
    
    // Initialize with default theme
    static void begin() { currentTheme = &THEME_DEFAULT; }
    
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
