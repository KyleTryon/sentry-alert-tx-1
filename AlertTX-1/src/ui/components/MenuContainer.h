#ifndef MENUCONTAINER_H
#define MENUCONTAINER_H

#include "../core/Component.h"
#include "MenuItem.h"
#include "Clickable.h"

/**
 * MenuContainer Component
 * 
 * Enhanced menu component that manages a collection of MenuItem components.
 * This is the new component-based menu system that will eventually replace
 * the original Menu class.
 * 
 * Features:
 * - Component-based architecture
 * - Automatic layout management
 * - Keyboard navigation
 * - Theme integration
 * - Memory efficient (fixed arrays)
 * - Scroll support for large menus
 */

class MenuContainer : public Component {
private:
    // Component management (fixed array for memory efficiency)
    static const int MAX_MENU_ITEMS = 8;  // Configurable limit
    MenuItem* menuItems[MAX_MENU_ITEMS];
    int itemCount = 0;
    int selectedIndex = 0;
    
    // Layout configuration (matches DisplayConfig constants)
    static const int ITEM_SPACING = 2;    // Space between items (MENU_ITEM_SPACING)
    static const int MENU_PADDING = 10;   // Edge padding (MARGIN_MEDIUM)
    
    // Scroll management
    int scrollOffset = 0;
    int visibleItemCount = 3;  // How many items visible at once (default for title layouts)
    
    // Navigation state
    bool navigationEnabled = true;
    InputHandler inputHandler;
    
    // Callback fired whenever the selected index changes
    std::function<void(int)> selectionChangedCallback;
    
public:
    MenuContainer(Adafruit_ST7789* display, int x = 10, int y = 50);
    virtual ~MenuContainer();
    
    // Component interface implementation
    void draw() override;
    void update() override;
    bool validate() const override;
    
    // Menu management
    bool addMenuItem(MenuItem* item);
    bool addMenuItem(const char* label, int id, std::function<void()> callback);
    bool addMenuItem(const char* label, int id, void (*callback)());
    bool removeMenuItem(int index);
    void clear();
    
    // Navigation
    void moveUp();
    void moveDown();
    void selectCurrent();
    void setSelectedIndex(int index);
    
    // Selection change callback
    void setOnSelectionChanged(std::function<void(int)> callback) { selectionChangedCallback = callback; }
    
    // Input handling
    void handleButtonPress(int button);
    void setNavigationEnabled(bool enabled) { navigationEnabled = enabled; }
    
    // State getters
    int getSelectedIndex() const { return selectedIndex; }
    int getItemCount() const { return itemCount; }
    MenuItem* getSelectedItem() const;
    MenuItem* getItem(int index) const;
    
    // Layout management
    void setVisibleItemCount(int count);
    void autoLayout();  // Automatically position all items
    void updateScrolling();
    
    // Visual configuration
    void setItemSpacing(int spacing);
    
    // Debug utilities
    void printMenuState() const;
    
private:
    // Layout helpers
    void layoutItems();
    void layoutVisibleItems();  // Positions visible items based on scroll offset
    int calculateTotalHeight() const;
    bool needsScrolling() const;
    
    // Drawing helpers
    void drawScrollIndicators();
    void drawBackground();
    
    // Navigation helpers
    void updateSelection();
    void scrollToSelected();
    bool isItemVisible(int index) const;
    
    // Validation helpers
    bool isValidIndex(int index) const;
    void validateScrollState();
};

/**
 * MenuBuilder
 * 
 * Utility class for building menus with a fluent interface.
 * Makes it easy to create complex menus programmatically.
 */
class MenuBuilder {
private:
    MenuContainer* menu;
    Adafruit_ST7789* display;
    
public:
    MenuBuilder(Adafruit_ST7789* display, int x = 10, int y = 50);
    ~MenuBuilder() = default;
    
    // Fluent interface for building menus
    MenuBuilder& addItem(const char* label, std::function<void()> callback);
    MenuBuilder& addItem(const char* label, void (*callback)());
    MenuBuilder& addSeparator();  // Could add visual separators
    MenuBuilder& setPosition(int x, int y);
    MenuBuilder& setSize(int w, int h);
    MenuBuilder& setVisibleItems(int count);
    
    // Build the final menu
    MenuContainer* build();
    
    // Static convenience methods
    static MenuContainer* createMainMenu(Adafruit_ST7789* display);
    static MenuContainer* createSettingsMenu(Adafruit_ST7789* display);
    static MenuContainer* createGamesMenu(Adafruit_ST7789* display);
};

#endif // MENUCONTAINER_H
