#ifndef MENUITEM_H
#define MENUITEM_H

#include "../core/Component.h"
#include "Clickable.h"
#include <functional>

/**
 * MenuItem Component
 * 
 * Individual menu item component with clickable behavior.
 * Replaces the simple MenuItem struct with a full component
 * that can handle its own rendering, state, and interactions.
 * 
 * Features:
 * - Theme-aware rendering
 * - Selection state management
 * - Clickable behavior with callbacks
 * - Hover/press visual feedback
 * - Automatic layout within parent Menu
 */

class MenuItem : public Component, public Clickable {
private:
    const char* label;
    int id;
    bool selected = false;
    bool pressed = false;
    
    // Callback functions
    std::function<void()> onSelectCallback;
    
    // Visual properties
    static const int DEFAULT_HEIGHT = 25;  // Matches current menu layout
    static const int TEXT_PADDING = 8;     // Horizontal text padding
    static const int ARROW_WIDTH = 12;     // Space for ">" indicator
    
public:
    MenuItem(Adafruit_ST7789* display, const char* label, int id = 0);
    virtual ~MenuItem() = default;
    
    // Component interface implementation
    void draw() override;
    void update() override;
    
    // Clickable interface implementation
    void onClick() override;
    void onPress() override;
    void onRelease() override;
    bool isPressed() const override { return pressed; }
    
    // MenuItem specific interface
    void setLabel(const char* newLabel);
    void setSelected(bool isSelected);
    void setOnSelect(std::function<void()> callback);
    void setOnSelect(void (*callback)());  // C-style function pointer
    
    // State getters
    const char* getLabel() const { return label; }
    int getId() const { return id; }
    bool isSelected() const { return selected; }
    
    // Layout helpers
    static int getDefaultHeight() { return DEFAULT_HEIGHT; }
    int getPreferredWidth() const;  // Calculate width based on text
    
    // Validation
    bool validate() const override;
    
private:
    // Drawing helpers
    void drawBackground();
    void drawSelectionIndicator();
    void drawText();
    void drawPressedState();
    
    // Color helpers
    uint16_t getBackgroundColor() const;
    uint16_t getTextColor() const;
    uint16_t getBorderColor() const;
    
    // Text measurement
    int getTextWidth() const;
};

/**
 * MenuItemFactory
 * 
 * Utility class for creating MenuItem components with common configurations.
 * Helps maintain consistency across the application.
 */
class MenuItemFactory {
public:
    // Create standard menu item
    static MenuItem* createMenuItem(Adafruit_ST7789* display, 
                                   const char* label, 
                                   int id,
                                   std::function<void()> callback);
    
    // Create menu item with C-style callback
    static MenuItem* createMenuItem(Adafruit_ST7789* display,
                                   const char* label,
                                   int id,
                                   void (*callback)());
    
    // Create navigation item (back, next, etc.)
    static MenuItem* createNavigationItem(Adafruit_ST7789* display,
                                         const char* label,
                                         std::function<void()> callback);
    
    // Create action item (settings, games, etc.)
    static MenuItem* createActionItem(Adafruit_ST7789* display,
                                     const char* label,
                                     std::function<void()> callback);
};

#endif // MENUITEM_H
