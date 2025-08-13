#ifndef MENU_H
#define MENU_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include "core/Theme.h"
#include "../config/DisplayConfig.h"

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
    
    // Layout constants from DisplayConfig (centralized display settings)
    // These match the optimized layout for our display size
    
    // Calculated layout using DisplayConfig constants
    int startX = MENU_PADDING;                        // From DisplayConfig
    int startY = MENU_START_Y;                        // From DisplayConfig
    int itemHeight = MENU_ITEM_HEIGHT;                // From DisplayConfig  
    int menuWidth = MENU_WIDTH;                       // From DisplayConfig
    
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
