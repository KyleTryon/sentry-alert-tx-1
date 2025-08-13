#include "Theme.h"
#include "../../config/SettingsManager.h"

// ThemeManager static member definitions
const Theme* ThemeManager::currentTheme = &THEME_DEFAULT;
int ThemeManager::currentThemeIndex = 0;

// Theme Definitions (stored in PROGMEM for memory efficiency)

// Default FlipperZero-inspired theme
// Black background, white text, yellow selection - matches current working design
const Theme THEME_DEFAULT PROGMEM = {
    .background = ST77XX_BLACK,         // Main screen background
    .surfaceBackground = ST77XX_BLACK,  // Component backgrounds  
    .primaryText = ST77XX_WHITE,        // Main text color
    .secondaryText = 0x8410,            // Gray text (RGB565: 128,128,128)
    .selectedText = ST77XX_BLACK,       // Text color when selected (black on yellow)
    .accent = ST77XX_YELLOW,            // Selection rectangle color
    .accentDark = 0xC618,               // Darker yellow for pressed state
    .border = 0x4208                    // Dark gray for borders
};

// High contrast theme for better visibility
const Theme THEME_HIGH_CONTRAST PROGMEM = {
    .background = ST77XX_BLACK,         // Pure black background
    .surfaceBackground = ST77XX_BLACK,  // Pure black surfaces
    .primaryText = ST77XX_WHITE,        // Pure white text
    .secondaryText = 0xC618,            // Light gray text
    .selectedText = ST77XX_BLACK,       // Black text on white selection
    .accent = ST77XX_WHITE,             // White selection rectangle
    .accentDark = 0xE71C,               // Light gray for pressed state
    .border = ST77XX_WHITE              // White borders
};

// Green terminal theme (retro computing style)
const Theme THEME_TERMINAL PROGMEM = {
    .background = 0x0200,               // Very dark green background
    .surfaceBackground = 0x0200,        // Very dark green surfaces
    .primaryText = 0x07E0,              // Bright green text (classic terminal)
    .secondaryText = 0x0400,            // Medium green text
    .selectedText = 0x0200,             // Dark green text on bright selection
    .accent = 0x07E0,                   // Bright green selection
    .accentDark = 0x0400,               // Medium green for pressed state
    .border = 0x0400                    // Medium green borders
};

// Amber CRT theme (classic amber monitor style)
const Theme THEME_AMBER PROGMEM = {
    .background = 0x4100,               // Very dark amber background (like terminal theme)
    .surfaceBackground = 0x4100,        // Very dark amber surfaces
    .primaryText = 0xFD20,              // Amber text (RGB565: 255,165,0)
    .secondaryText = 0x8200,            // Dimmed amber text
    .selectedText = 0x4100,             // Dark amber text on amber selection
    .accent = 0xFD20,                   // Amber selection rectangle
    .accentDark = 0x8200,               // Darker amber for pressed state
    .border = 0x8200                    // Darker amber borders
};

// Sentry purple theme (company branding colors)
const Theme THEME_SENTRY PROGMEM = {
    .background = 0x18A6,               // Dark purple background (#1F1633)
    .surfaceBackground = 0x18A6,        // Dark purple surfaces
    .primaryText = ST77XX_WHITE,        // White text for high contrast
    .secondaryText = 0xC618,            // Light gray secondary text
    .selectedText = ST77XX_YELLOW,      // Yellow text on purple selection for visibility
    .accent = 0x6AF8,                   // Sentry purple selection (#6A5FC1)
    .accentDark = 0x5232,               // Darker purple for pressed state
    .border = 0x6AF8                    // Sentry purple borders
};

/**
 * Color Reference (RGB565 format):
 * 
 * ST77XX_BLACK  = 0x0000 (0,0,0)
 * ST77XX_WHITE  = 0xFFFF (255,255,255)  
 * ST77XX_YELLOW = 0xFFE0 (255,255,0)
 * 
 * Custom colors:
 * 0x8410 = RGB(128,128,128) - Medium gray
 * 0x4208 = RGB(64,64,64)    - Dark gray
 * 0xC618 = RGB(192,192,192) - Light gray
 * 0xE71C = RGB(224,224,224) - Very light gray
 * 
 * Green terminal colors:
 * 0x0200 = RGB(0,32,0)      - Very dark green
 * 0x0400 = RGB(0,64,0)      - Medium dark green
 * 0x07E0 = RGB(0,255,0)     - Bright green
 * 
 * Amber colors:
 * 0x4100 = RGB(32,16,0)     - Very dark amber
 * 0x8200 = RGB(128,80,0)    - Dimmed amber
 * 0xFD20 = RGB(255,165,0)   - Classic amber
 * 
 * Purple colors (Sentry theme):
 * 0x18A6 = RGB(31,22,51)    - Very dark purple background (#1F1633)
 * 0x5232 = RGB(80,70,150)   - Darker purple for pressed state
 * 0x6AF8 = RGB(106,95,193)  - Sentry purple (#6A5FC1)
 */

// Theme registry for index-based access and metadata
const Theme* ThemeManager::themes[5] = {
    &THEME_DEFAULT,      // Index 0
    &THEME_HIGH_CONTRAST,// Index 1
    &THEME_TERMINAL,     // Index 2
    &THEME_AMBER,        // Index 3
    &THEME_SENTRY        // Index 4
};

const char* ThemeManager::themeNames[5] = {
    "Default",
    "High Contrast", 
    "Terminal",
    "Amber",
    "Sentry"
};

const char* ThemeManager::themeDescriptions[5] = {
    "FlipperZero style with yellow accents",
    "Pure black and white for accessibility",
    "Retro green terminal computing style",
    "Classic amber CRT with dark amber background",
    "Sentry purple theme with company branding"
};

// Enhanced ThemeManager Implementation

void ThemeManager::loadFromSettings() {
    int savedIndex = SettingsManager::getThemeIndex();
    setThemeByIndex(savedIndex, false); // Don't persist again
    Serial.printf("ThemeManager: Loaded theme '%s' from settings\n", getCurrentThemeName());
}

void ThemeManager::setThemeByIndex(int index, bool persist) {
    if (!isValidThemeIndex(index)) {
        Serial.printf("ThemeManager: Invalid theme index %d, keeping current theme\n", index);
        return;
    }
    
    // Update current theme
    currentTheme = themes[index];
    currentThemeIndex = index;
    
    Serial.printf("ThemeManager: Applied theme '%s' (index %d)\n", themeNames[index], index);
    
    // Save to persistent storage if requested
    if (persist) {
        SettingsManager::setThemeIndex(index);
        Serial.println("ThemeManager: Theme preference saved");
    }
}

const char* ThemeManager::getCurrentThemeName() {
    return getThemeName(currentThemeIndex);
}

const char* ThemeManager::getThemeName(int index) {
    if (!isValidThemeIndex(index)) {
        return "Unknown";
    }
    return themeNames[index];
}

const char* ThemeManager::getThemeDescription(int index) {
    if (!isValidThemeIndex(index)) {
        return "Invalid theme index";
    }
    return themeDescriptions[index];
}

const Theme* ThemeManager::getThemeByIndex(int index) {
    if (!isValidThemeIndex(index)) {
        return &THEME_DEFAULT; // Safe fallback
    }
    return themes[index];
}

bool ThemeManager::isValidThemeIndex(int index) {
    return (index >= 0 && index < THEME_COUNT);
}
