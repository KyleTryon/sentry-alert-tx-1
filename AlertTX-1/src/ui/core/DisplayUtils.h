#ifndef DISPLAYUTILS_H
#define DISPLAYUTILS_H

#include <Adafruit_ST7789.h>
#include "../../config/DisplayConfig.h"
#include "Theme.h"

#if __has_include("../../icons/Icon.h")
#include "../../icons/Icon.h"
#endif

/**
 * DisplayUtils
 * 
 * Extended graphics utilities for Adafruit_GFX displays.
 * Think of this as an extension to the Adafruit_GFX library
 * with convenience methods for common UI operations.
 * 
 * All methods are static - this is a utility class, not a manager.
 */

class DisplayUtils {
public:
    // =============================================================================
    // TEXT RENDERING UTILITIES (extends Adafruit_GFX)
    // =============================================================================
    
    /**
     * Center text horizontally at specified Y position
     * @param display Graphics display instance
     * @param text Text to center
     * @param textSize Text size (1-3)
     * @param y Y position for text baseline
     */
    static void centerText(Adafruit_ST7789* display, const char* text, int textSize, int y);
    
    /**
     * Draw centered text with automatic width calculation
     * @param display Graphics display instance  
     * @param text Text to center
     * @param textSize Text size (1-3)
     * @param y Y position for text baseline
     * @param color Text color (RGB565)
     */
    static void centerTextWithColor(Adafruit_ST7789* display, const char* text, int textSize, int y, uint16_t color);
    
    /**
     * Draw title using standard positioning and theme colors
     * @param display Graphics display instance
     * @param title Title text
     */
    static void drawTitle(Adafruit_ST7789* display, const char* title);
    
    /**
     * Calculate text width for given string and size
     * @param display Graphics display instance
     * @param text Text to measure
     * @param textSize Text size
     * @return Width in pixels
     */
    static int getTextWidth(Adafruit_ST7789* display, const char* text, int textSize);
    
    // =============================================================================
    // SHAPE AND DRAWING UTILITIES
    // =============================================================================
    
    /**
     * Draw a centered rectangle
     * @param display Graphics display instance
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Fill color
     */
    static void drawCenteredRect(Adafruit_ST7789* display, int width, int height, uint16_t color);
    
    /**
     * Draw a full-width separator line
     * @param display Graphics display instance
     * @param y Y position
     * @param color Line color
     */
    static void drawSeparatorLine(Adafruit_ST7789* display, int y, uint16_t color);
    
    // =============================================================================
    // LAYOUT UTILITIES
    // =============================================================================
    
    /**
     * Check if coordinates are within display bounds
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if on screen
     */
    static bool isOnScreen(int x, int y);
    
    /**
     * Check if rectangle fits on screen
     * @param x X position
     * @param y Y position
     * @param width Rectangle width
     * @param height Rectangle height
     * @return true if rectangle fits entirely on screen
     */
    static bool fitsOnScreen(int x, int y, int width, int height);
    
    /**
     * Clamp coordinates to screen bounds
     * @param x Reference to X coordinate (will be modified)
     * @param y Reference to Y coordinate (will be modified)
     */
    static void clampToScreen(int& x, int& y);
    
    // =============================================================================
    // DEBUG UTILITIES (only active in debug builds)
    // =============================================================================
    
    /**
     * Print debug message for screen enter event
     * @param screenName Name of screen being entered
     */
    static void debugScreenEnter(const char* screenName);
    
    /**
     * Print debug message for screen exit event
     * @param screenName Name of screen being exited
     */
    static void debugScreenExit(const char* screenName);
    
    /**
     * Print debug message for screen action
     * @param screenName Name of screen
     * @param action Action being performed
     */
    static void debugScreenAction(const char* screenName, const char* action);
    
    /**
     * Draw debug overlay showing screen dimensions and safe areas
     * @param display Graphics display instance
     */
    static void drawDebugOverlay(Adafruit_ST7789* display);

    #if __has_include("../../icons/Icon.h")
    /**
     * Draw an Icon (RGB565 in PROGMEM) at the given coordinates
     */
    static void drawIcon(Adafruit_ST7789* display, const Icon& icon, int x, int y);
    #endif
};

#endif // DISPLAYUTILS_H