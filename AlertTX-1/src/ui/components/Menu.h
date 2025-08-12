#ifndef MENU_H
#define MENU_H

#include "../core/Theme.h"
#include "../core/EventSystem.h"
#include "../renderer/DisplayRenderer.h"
#include "MenuItem.h"
#include "IconRenderer.h"

class Menu {
private:
    MenuItem* items;
    int itemCount;
    int selectedIndex = 0;
    int scrollOffset = 0;
    int visibleItems = 6;     // 240x135 display can show ~6 items (119px / 18px per item)
    int itemHeight = 18;      // 16px icon + 2px padding
    bool needsRedraw = true;
    
    // Selection animation
    unsigned long lastSelectionTime = 0;
    bool selectionHighlighted = false;
    static const unsigned long SELECTION_HIGHLIGHT_DURATION = 200;
    
    // Layout constants
    static const int ARROW_WIDTH = 8;
    static const int ICON_X = 20;
    static const int TEXT_X = 40;
    static const int ITEM_PADDING = 2;
    
public:
    Menu();
    Menu(MenuItem* items, int count);
    
    // Setup
    void setItems(MenuItem* items, int count);
    
    // Drawing
    void draw(DisplayRenderer* renderer, const Theme& theme, int yOffset);
    
    // Navigation
    void navigateUp();
    void navigateDown();
    void select();
    void handleInput(ButtonAction action);
    void handleEvent(const UIEvent& event);
    
    // State management
    int getSelectedIndex() const { return selectedIndex; }
    MenuItem* getSelectedItem();
    void setSelectedIndex(int index);
    void markForRedraw() { needsRedraw = true; }
    bool hasChanged() const { return needsRedraw; }
    
    // Layout info
    int getItemHeight() const { return itemHeight; }
    int getVisibleItemCount() const { return visibleItems; }
    int getTotalHeight() const { return visibleItems * itemHeight; }
    
private:
    void drawMenuItem(DisplayRenderer* renderer, const MenuItem& item, 
                     int x, int y, bool isSelected, const Theme& theme);
    void drawSelectionIndicator(DisplayRenderer* renderer, int x, int y, const Theme& theme);
    void drawScrollIndicators(DisplayRenderer* renderer, int x, int y, int height, const Theme& theme);
    void updateScrollOffset();
    void animateSelection();
    bool isValidIndex(int index) const;
};

#endif // MENU_H
