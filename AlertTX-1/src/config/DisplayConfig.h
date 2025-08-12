#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H

/**
 * DisplayConfig.h
 * 
 * Centralized display hardware configuration and specifications.
 * This should be the SINGLE source of truth for all display dimensions,
 * capabilities, and hardware-specific settings.
 */

// =============================================================================
// DISPLAY HARDWARE SPECIFICATIONS
// =============================================================================

// Current Hardware: Adafruit ESP32-S3 Reverse TFT Feather
#define DISPLAY_ESP32S3_REVERSE_TFT

#ifdef DISPLAY_ESP32S3_REVERSE_TFT
    // Physical display dimensions
    static const int DISPLAY_WIDTH = 240;
    static const int DISPLAY_HEIGHT = 135;
    
    // Display driver type
    #define DISPLAY_DRIVER_ST7789
    
    // Hardware rotation (landscape)
    static const int DISPLAY_ROTATION = 3;
    
    // Display capabilities
    static const bool DISPLAY_HAS_TOUCH = false;
    static const bool DISPLAY_HAS_BACKLIGHT_CONTROL = true;
    static const int DISPLAY_COLOR_DEPTH = 16; // RGB565
    
    // Safe drawing area (accounting for potential bezels/margins)
    static const int SAFE_AREA_WIDTH = DISPLAY_WIDTH;
    static const int SAFE_AREA_HEIGHT = DISPLAY_HEIGHT;
    
    // Common layout constants derived from display size
    static const int HEADER_HEIGHT = 30;
    static const int FOOTER_HEIGHT = 20;
    static const int CONTENT_HEIGHT = DISPLAY_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT;
    
    // Standard margins and spacing
    static const int MARGIN_SMALL = 5;
    static const int MARGIN_MEDIUM = 10;
    static const int MARGIN_LARGE = 20;
    
    // Text sizing guidelines for this display
    static const int TEXT_SIZE_SMALL = 1;
    static const int TEXT_SIZE_MEDIUM = 2;
    static const int TEXT_SIZE_LARGE = 3;
    static const int TEXT_SIZE_TITLE = 2;
    
    // Menu-specific layout (optimized for this display size)
    static const int MENU_ITEM_HEIGHT = 25;     // Optimized for 3-4 items
    static const int MENU_ITEM_SPACING = 2;
    static const int MENU_START_Y = 50;         // Below title area
    static const int MENU_PADDING = MARGIN_MEDIUM;
    static const int MENU_WIDTH = DISPLAY_WIDTH - (2 * MENU_PADDING);
#endif

// =============================================================================
// DISPLAY UTILITY MACROS
// =============================================================================

// Center calculations
#define CENTER_X(width) ((DISPLAY_WIDTH - (width)) / 2)
#define CENTER_Y(height) ((DISPLAY_HEIGHT - (height)) / 2)

// Bounds checking
#define IS_ON_SCREEN(x, y) ((x) >= 0 && (x) < DISPLAY_WIDTH && (y) >= 0 && (y) < DISPLAY_HEIGHT)
#define CLAMP_X(x) (max(0, min((x), DISPLAY_WIDTH - 1)))
#define CLAMP_Y(y) (max(0, min((y), DISPLAY_HEIGHT - 1)))

// Common positions
#define TITLE_X CENTER_X(0)  // Center horizontally (width calculated at runtime)
#define TITLE_Y 20

#endif // DISPLAYCONFIG_H
