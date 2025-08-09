#ifndef MENU_H
#define MENU_H

#include "../core/StatefulComponent.h"
#include "../renderer/Renderer.h"
#include <String.h>
#include <vector>
#include <functional>

/**
 * Menu item structure.
 */
struct MenuItem {
    String text;
    std::function<void()> onSelect;
    bool enabled = true;
    bool visible = true;
    
    MenuItem(const String& text, std::function<void()> callback = nullptr, bool enabled = true)
        : text(text), onSelect(callback), enabled(enabled) {}
};

/**
 * Menu component for displaying selectable lists in the Alert TX-1 UI.
 */
class Menu : public StatefulComponent {
private:
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    int scrollOffset = 0;
    int visibleItems = 4;  // Number of items visible at once
    
    uint16_t textColor = COLOR_WHITE;
    uint16_t selectedColor = COLOR_BLUE;
    uint16_t disabledColor = COLOR_GRAY;
    uint16_t backgroundColor = COLOR_BLACK;
    
    int itemHeight = 16;
    bool showScrollIndicator = true;
    bool wrapAround = true;

public:
    Menu(int x, int y, int w, int h) : StatefulComponent(x, y, w, h) {
        calculateVisibleItems();
    }

    /**
     * Add a menu item.
     */
    void addItem(const String& text, std::function<void()> onSelect = nullptr, bool enabled = true) {
        setState([this, &text, onSelect, enabled]() {
            items.emplace_back(text, onSelect, enabled);
        });
    }
    
    /**
     * Insert a menu item at a specific position.
     */
    void insertItem(int index, const String& text, std::function<void()> onSelect = nullptr, bool enabled = true) {
        if (index < 0 || index > items.size()) return;
        
        setState([this, index, &text, onSelect, enabled]() {
            items.insert(items.begin() + index, MenuItem(text, onSelect, enabled));
        });
    }
    
    /**
     * Remove a menu item by index.
     */
    void removeItem(int index) {
        if (index < 0 || index >= items.size()) return;
        
        setState([this, index]() {
            items.erase(items.begin() + index);
            if (selectedIndex >= items.size() && items.size() > 0) {
                selectedIndex = items.size() - 1;
            }
            updateScrollOffset();
        });
    }
    
    /**
     * Clear all menu items.
     */
    void clearItems() {
        setState([this]() {
            items.clear();
            selectedIndex = 0;
            scrollOffset = 0;
        });
    }
    
    /**
     * Set the selected item index.
     */
    void setSelectedIndex(int index) {
        if (index < 0 || index >= items.size()) return;
        
        setState([this, index]() {
            selectedIndex = index;
            updateScrollOffset();
        });
    }
    
    /**
     * Get the selected item index.
     */
    int getSelectedIndex() const {
        return selectedIndex;
    }
    
    /**
     * Get the selected item text.
     */
    String getSelectedText() const {
        if (selectedIndex >= 0 && selectedIndex < items.size()) {
            return items[selectedIndex].text;
        }
        return "";
    }
    
    /**
     * Move selection up.
     */
    void selectPrevious() {
        if (items.empty()) return;
        
        setState([this]() {
            if (wrapAround) {
                selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
            } else {
                selectedIndex = max(0, selectedIndex - 1);
            }
            updateScrollOffset();
        });
    }
    
    /**
     * Move selection down.
     */
    void selectNext() {
        if (items.empty()) return;
        
        setState([this]() {
            if (wrapAround) {
                selectedIndex = (selectedIndex + 1) % items.size();
            } else {
                selectedIndex = min((int)items.size() - 1, selectedIndex + 1);
            }
            updateScrollOffset();
        });
    }
    
    /**
     * Activate the selected item.
     */
    void activateSelected() {
        if (selectedIndex >= 0 && selectedIndex < items.size()) {
            const MenuItem& item = items[selectedIndex];
            if (item.enabled && item.onSelect) {
                item.onSelect();
            }
        }
    }
    
    /**
     * Set menu colors.
     */
    void setColors(uint16_t text, uint16_t selected, uint16_t disabled = COLOR_GRAY, uint16_t background = COLOR_BLACK) {
        setState([this, text, selected, disabled, background]() {
            textColor = text;
            selectedColor = selected;
            disabledColor = disabled;
            backgroundColor = background;
        });
    }
    
    /**
     * Set item height.
     */
    void setItemHeight(int height) {
        if (itemHeight != height && height > 0) {
            setState([this, height]() {
                itemHeight = height;
                calculateVisibleItems();
                updateScrollOffset();
            });
        }
    }
    
    /**
     * Enable/disable wrap around navigation.
     */
    void setWrapAround(bool wrap) {
        if (wrapAround != wrap) {
            setState([this, wrap]() {
                wrapAround = wrap;
            });
        }
    }
    
    /**
     * Handle input events.
     */
    void handleEvent(int eventType, int eventData = 0) override {
        switch (eventType) {
            case 1: // Up
                selectPrevious();
                break;
                
            case 2: // Down
                selectNext();
                break;
                
            case 3: // Select/Enter
                activateSelected();
                break;
                
            case 4: // Page up
                for (int i = 0; i < visibleItems && selectedIndex > 0; i++) {
                    selectPrevious();
                }
                break;
                
            case 5: // Page down
                for (int i = 0; i < visibleItems && selectedIndex < items.size() - 1; i++) {
                    selectNext();
                }
                break;
        }
    }
    
    /**
     * Get the number of items.
     */
    size_t getItemCount() const {
        return items.size();
    }
    
    /**
     * Render the menu.
     */
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible()) return;
        
        // Clear background
        gfx.fillRect(x, y, w, h, backgroundColor);
        
        // Draw visible items
        for (int i = 0; i < visibleItems && (scrollOffset + i) < items.size(); i++) {
            int itemIndex = scrollOffset + i;
            const MenuItem& item = items[itemIndex];
            
            if (!item.visible) continue;
            
            int itemY = y + (i * itemHeight);
            bool selected = (itemIndex == selectedIndex);
            
            uint16_t itemTextColor = textColor;
            if (!item.enabled) {
                itemTextColor = disabledColor;
            }
            
            // Use renderer to draw menu item
            Renderer::drawMenuItem(gfx, x, itemY, w, itemHeight, 
                                 item.text.c_str(), selected, itemTextColor, selectedColor);
        }
        
        // Draw scroll indicators if needed
        if (showScrollIndicator && items.size() > visibleItems) {
            // Up arrow
            if (scrollOffset > 0) {
                Renderer::drawIcon(gfx, x + w - 10, y + 2, 6, "up", COLOR_WHITE);
            }
            
            // Down arrow
            if (scrollOffset + visibleItems < items.size()) {
                Renderer::drawIcon(gfx, x + w - 10, y + h - 8, 6, "down", COLOR_WHITE);
            }
        }
        
        // Draw border
        gfx.drawRect(x, y, w, h, COLOR_GRAY);
    }

private:
    /**
     * Calculate how many items can be visible at once.
     */
    void calculateVisibleItems() {
        if (itemHeight > 0) {
            visibleItems = h / itemHeight;
        }
    }
    
    /**
     * Update scroll offset to keep selected item visible.
     */
    void updateScrollOffset() {
        if (selectedIndex < scrollOffset) {
            scrollOffset = selectedIndex;
        } else if (selectedIndex >= scrollOffset + visibleItems) {
            scrollOffset = selectedIndex - visibleItems + 1;
        }
        
        // Ensure scroll offset is within bounds
        scrollOffset = max(0, min(scrollOffset, (int)items.size() - visibleItems));
    }
};

#endif // MENU_H