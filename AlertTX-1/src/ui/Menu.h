#ifndef MENU_H
#define MENU_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include "core/Theme.h"

/**
 * Enhanced Menu Class
 * Features:
 * - Theme system integration for consistent colors
 * - Full-width menu items (220px on 240px screen) 
 * - Larger menu items (30px height vs previous 15px)
 * - Better spacing and visual design
 * - Up/Down navigation with button controls (not touch)
 * - Built-in CP437 symbols for arrows if needed
 */

struct MenuItem {
    const char* label;
    int id;
    void (*action)();  // Simple callback function
};

class Menu {
private:
    Adafruit_ST7789* display;
    MenuItem* items;
    int itemCount;
    int selectedIndex;
    
    // Enhanced layout - full width, optimized for 135px screen height
    static const int MENU_PADDING = 10;      // Edge padding from screen edges
    static const int ITEM_HEIGHT = 25;       // Optimized height for 3-4 items on 135px screen
    static const int ITEM_SPACING = 2;       // Space between menu items
    static const int SCREEN_HEIGHT = 135;    // ESP32-S3 Reverse TFT screen height
    
    // Calculated layout (240x135 screen)
    int startX = MENU_PADDING;                        // 10px from left edge
    int startY = 50;                                  // Moved up to better utilize screen space
    int itemHeight = ITEM_HEIGHT;                     // 25px per item
    int menuWidth = 240 - (2 * MENU_PADDING);        // 220px (full width minus padding)
    
    // Theme system replaces hardcoded colors
    // Colors now come from ThemeManager::getTheme()
    
public:
    Menu(Adafruit_ST7789* tft);
    
    // Setup
    void setItems(MenuItem* menuItems, int count);
    void setPosition(int x, int y);
    
    // Theme management (uses global ThemeManager)
    // Colors automatically update when ThemeManager::setTheme() is called
    
    // Navigation
    void moveUp();
    void moveDown();
    void select();
    int getSelectedIndex() const { return selectedIndex; }
    int getSelectedId() const;
    
    // Drawing
    void draw();
    void clear();
    
    // Utility
    void reset() { selectedIndex = 0; }
    
    // Screen bounds validation
    bool validateLayout() const;
    int getMaxVisibleItems() const;
    int getTotalMenuHeight() const;
};

#endif // MENU_H
