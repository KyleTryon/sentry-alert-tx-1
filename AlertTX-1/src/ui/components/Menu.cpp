#include "Menu.h"
#include <Arduino.h>

Menu::Menu() : items(nullptr), itemCount(0) {
    // Default initialization
}

Menu::Menu(MenuItem* items, int count) {
    setItems(items, count);
}

void Menu::setItems(MenuItem* items, int count) {
    this->items = items;
    this->itemCount = count;
    this->selectedIndex = 0;
    this->scrollOffset = 0;
    this->needsRedraw = true;
    updateScrollOffset();
}

void Menu::draw(DisplayRenderer* renderer, const Theme& theme, int yOffset) {
    if (!renderer || !items || itemCount == 0) return;
    
    int startY = yOffset;
    
    // Calculate visible range
    int startIndex = scrollOffset;
    int endIndex = min(itemCount, startIndex + visibleItems);
    
    // Clear menu area
    renderer->fillRect(0, startY, 240, visibleItems * itemHeight, theme.background);
    
    // Draw menu items
    for (int i = startIndex; i < endIndex; i++) {
        int itemY = startY + (i - startIndex) * itemHeight;
        bool isSelected = (i == selectedIndex);
        
        drawMenuItem(renderer, items[i], 0, itemY, isSelected, theme);
    }
    
    // Draw scroll indicators if needed
    if (itemCount > visibleItems) {
        drawScrollIndicators(renderer, 230, startY, visibleItems * itemHeight, theme);
    }
    
    needsRedraw = false;
}

void Menu::navigateUp() {
    if (selectedIndex > 0) {
        selectedIndex--;
        lastSelectionTime = millis();
        selectionHighlighted = true;
        updateScrollOffset();
        needsRedraw = true;
    }
}

void Menu::navigateDown() {
    if (selectedIndex < itemCount - 1) {
        selectedIndex++;
        lastSelectionTime = millis();
        selectionHighlighted = true;
        updateScrollOffset();
        needsRedraw = true;
    }
}

void Menu::select() {
    if (isValidIndex(selectedIndex) && items[selectedIndex].enabled) {
        // Execute action if available
        if (items[selectedIndex].action) {
            items[selectedIndex].action();
        }
        
        // Provide visual feedback
        lastSelectionTime = millis();
        selectionHighlighted = true;
        needsRedraw = true;
    }
}

void Menu::handleInput(ButtonAction action) {
    switch (action) {
        case ACTION_UP:
            navigateUp();
            break;
        case ACTION_DOWN:
            navigateDown();
            break;
        case ACTION_SELECT:
            select();
            break;
        default:
            break;
    }
}

void Menu::handleEvent(const UIEvent& event) {
    // Menu only handles ButtonAction events via handleInput()
    // Raw UIEvent events are ignored to prevent double-handling
    // This prevents the menu from being triggered twice when a button is pressed
}

MenuItem* Menu::getSelectedItem() {
    if (isValidIndex(selectedIndex)) {
        return &items[selectedIndex];
    }
    return nullptr;
}

void Menu::setSelectedIndex(int index) {
    if (isValidIndex(index)) {
        selectedIndex = index;
        updateScrollOffset();
        needsRedraw = true;
    }
}

void Menu::drawMenuItem(DisplayRenderer* renderer, const MenuItem& item, 
                       int x, int y, bool isSelected, const Theme& theme) {
    uint16_t textColor = theme.foreground;
    uint16_t backgroundColor = theme.background;
    uint16_t iconColor = theme.foreground;
    
    // Handle selection highlighting
    if (isSelected) {
        // Check if we should show selection animation
        unsigned long currentTime = millis();
        if ((currentTime - lastSelectionTime) < SELECTION_HIGHLIGHT_DURATION) {
            backgroundColor = theme.accent;
            textColor = theme.background;
            iconColor = theme.background;
        } else {
            // Normal selection appearance
            backgroundColor = theme.accent;
            textColor = theme.background;
            iconColor = theme.background;
        }
        
        // Draw selection background
        renderer->fillRect(x, y, 240, itemHeight, backgroundColor);
    }
    
    // Handle disabled items
    if (!item.enabled) {
        textColor = theme.secondary;
        iconColor = theme.secondary;
    }
    
    // Draw selection arrow
    if (isSelected) {
        renderer->setTextColor(textColor);
        renderer->setTextSize(1);
        renderer->setCursor(x + 2, y + 5);
        renderer->print(">");
    }
    
    // Draw icon
    if (item.icon && IconRenderer::isValidIcon(item.icon)) {
        IconRenderer::drawIcon(renderer, *item.icon, x + ICON_X, y + 1, iconColor);
    }
    
    // Draw text
    if (item.label) {
        renderer->setTextColor(textColor);
        renderer->setTextSize(1);
        renderer->setCursor(x + TEXT_X, y + 5);
        renderer->print(item.label);
    }
}

void Menu::drawSelectionIndicator(DisplayRenderer* renderer, int x, int y, const Theme& theme) {
    // Draw animated selection indicator (simple flash)
    unsigned long currentTime = millis();
    if ((currentTime - lastSelectionTime) < SELECTION_HIGHLIGHT_DURATION) {
        bool flash = ((currentTime - lastSelectionTime) / 50) % 2;
        if (flash) {
            renderer->drawRect(x, y, 240, itemHeight, theme.accent);
        }
    }
}

void Menu::drawScrollIndicators(DisplayRenderer* renderer, int x, int y, int height, const Theme& theme) {
    // Draw scroll indicators on the right edge
    if (scrollOffset > 0) {
        // Up arrow
        renderer->fillTriangle(x, y + 5, x + 4, y, x + 8, y + 5, theme.secondary);
    }
    
    if (scrollOffset + visibleItems < itemCount) {
        // Down arrow
        int bottomY = y + height;
        renderer->fillTriangle(x, bottomY - 5, x + 4, bottomY, x + 8, bottomY - 5, theme.secondary);
    }
    
    // Draw scroll bar
    if (itemCount > visibleItems) {
        int barHeight = (height * visibleItems) / itemCount;
        int barY = y + (height * scrollOffset) / itemCount;
        renderer->fillRect(x + 4, barY, 2, barHeight, theme.secondary);
    }
}

void Menu::updateScrollOffset() {
    if (itemCount <= visibleItems) {
        scrollOffset = 0;
        return;
    }
    
    // Keep selected item visible
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + visibleItems) {
        scrollOffset = selectedIndex - visibleItems + 1;
    }
    
    // Ensure scroll offset is within bounds
    scrollOffset = max(0, min(scrollOffset, itemCount - visibleItems));
}

void Menu::animateSelection() {
    // This could be expanded for more complex animations
    // Currently handled in drawMenuItem()
}

bool Menu::isValidIndex(int index) const {
    return (index >= 0 && index < itemCount && items != nullptr);
}
