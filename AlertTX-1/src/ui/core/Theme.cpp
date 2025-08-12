#include "Theme.h"

// ThemeManager static member definition
const Theme* ThemeManager::currentTheme = &THEME_DEFAULT;

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
    .background = 0x0000,               // Black background (like old CRT)
    .surfaceBackground = 0x0000,        // Black surfaces
    .primaryText = 0xFD20,              // Amber text (RGB565: 255,165,0)
    .secondaryText = 0x8200,            // Dimmed amber text
    .selectedText = 0x0000,             // Black text on amber selection
    .accent = 0xFD20,                   // Amber selection rectangle
    .accentDark = 0x8200,               // Darker amber for pressed state
    .border = 0x8200                    // Darker amber borders
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
 * 0xFD20 = RGB(255,165,0)   - Classic amber
 * 0x8200 = RGB(128,80,0)    - Dimmed amber
 */
