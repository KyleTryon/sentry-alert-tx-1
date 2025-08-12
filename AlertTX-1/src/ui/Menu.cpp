#include "Menu.h"

Menu::Menu(Adafruit_ST7789* tft) : display(tft), items(nullptr), itemCount(0), selectedIndex(0) {
    // Initialize ThemeManager if not already done
    // This ensures we have a valid theme even if begin() wasn't called
    if (ThemeManager::getTheme() == nullptr) {
        ThemeManager::begin();
    }
}

void Menu::setItems(MenuItem* menuItems, int count) {
    items = menuItems;
    itemCount = count;
    selectedIndex = 0;  // Reset to first item
    
    // Validate layout and provide helpful warnings
    if (!validateLayout()) {
        int maxItems = getMaxVisibleItems();
        Serial.printf("LAYOUT WARNING: %d items may not fit. Max visible items: %d\n", count, maxItems);
        Serial.printf("Total menu height: %dpx, Available height: %dpx\n", 
                     getTotalMenuHeight(), SCREEN_HEIGHT - startY);
        Serial.println("Consider reducing item count or adjusting layout parameters.");
    } else {
        Serial.printf("Layout OK: %d items fit within screen bounds\n", count);
    }
}

void Menu::setPosition(int x, int y) {
    startX = x;
    startY = y;
}

// setColors method removed - now uses ThemeManager for consistent theming

void Menu::moveUp() {
    if (itemCount > 0) {
        selectedIndex = (selectedIndex - 1 + itemCount) % itemCount;
    }
}

void Menu::moveDown() {
    if (itemCount > 0) {
        selectedIndex = (selectedIndex + 1) % itemCount;
    }
}

void Menu::select() {
    if (items && selectedIndex >= 0 && selectedIndex < itemCount) {
        MenuItem& item = items[selectedIndex];
        if (item.action) {
            item.action();  // Call the callback function
        }
    }
}

int Menu::getSelectedId() const {
    if (items && selectedIndex >= 0 && selectedIndex < itemCount) {
        return items[selectedIndex].id;
    }
    return -1;
}

void Menu::clear() {
    if (!display || !items) return;
    
    // Clear the menu area using theme background color
    int totalHeight = itemCount * (itemHeight + ITEM_SPACING) + 10;  // Include spacing + extra padding
    display->fillRect(startX - 5, startY - 5, menuWidth + 10, totalHeight, ThemeManager::getBackground());
}

void Menu::draw() {
    if (!display || !items) return;
    
    // Set up text properties
    display->setTextSize(1);
    
    for (int i = 0; i < itemCount; i++) {
        // Calculate item position with spacing
        int itemY = startY + (i * (itemHeight + ITEM_SPACING));
        
        if (i == selectedIndex) {
            // Draw selection rectangle with theme accent color
            // Full width rectangle with rounded appearance
            display->fillRect(startX, itemY - 2, menuWidth, itemHeight, ThemeManager::getAccent());
            display->drawRect(startX, itemY - 2, menuWidth, itemHeight, ThemeManager::getBorder());
            
            // Draw selected text with ">" indicator using theme colors
            display->setTextColor(ThemeManager::getSelectedText());
            display->setCursor(startX + 8, itemY + 6);  // Centered in 25px item (was 8 for 30px)
            display->print("> ");
            display->print(items[i].label);
        } else {
            // Clear background for unselected items
            display->fillRect(startX, itemY - 2, menuWidth, itemHeight, ThemeManager::getSurfaceBackground());
            
            // Draw normal text with padding for alignment using theme colors
            display->setTextColor(ThemeManager::getPrimaryText());
            display->setCursor(startX + 8, itemY + 6);  // Centered in 25px item (was 8 for 30px)
            display->print("  ");  // Space for alignment with ">" indicator
            display->print(items[i].label);
        }
    }
}

// Screen bounds validation and utility methods

bool Menu::validateLayout() const {
    int totalHeight = getTotalMenuHeight();
    int endY = startY + totalHeight;
    
    // Check if menu fits within screen bounds
    if (endY > SCREEN_HEIGHT) {
        Serial.printf("WARNING: Menu extends beyond screen! EndY=%d, ScreenHeight=%d\n", endY, SCREEN_HEIGHT);
        return false;
    }
    
    return true;
}

int Menu::getMaxVisibleItems() const {
    // Calculate how many items can fit on screen
    int availableHeight = SCREEN_HEIGHT - startY - 10; // 10px bottom margin
    int itemSizeWithSpacing = itemHeight + ITEM_SPACING;
    return availableHeight / itemSizeWithSpacing;
}

int Menu::getTotalMenuHeight() const {
    if (itemCount == 0) return 0;
    return (itemCount * itemHeight) + ((itemCount - 1) * ITEM_SPACING);
}

/**
 * CP437 Extended Characters for Future Use:
 * 
 * With display->cp437(true) enabled, these characters are available:
 * 0x10 (16)  = ► (right triangle)
 * 0x11 (17)  = ◄ (left triangle) 
 * 0x1E (30)  = ▲ (up triangle)
 * 0x1F (31)  = ▼ (down triangle)
 * 0xF8 (248) = ° (degree symbol)
 * 0xFE (254) = ■ (solid square)
 * 
 * Example usage for arrows:
 * display->write(0x10); // Print ►
 * 
 * For now we use simple ">" character for FlipperZero-like appearance
 */
