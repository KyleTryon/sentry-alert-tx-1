#include "MenuItem.h"

MenuItem::MenuItem(Adafruit_ST7789* display, const char* label, int id)
    : Component(display, "MenuItem"), label(label), id(id) {
    
    // Set default size - width will be set by parent Menu
    setSize(0, DEFAULT_HEIGHT);
    
    if (!label) {
        Serial.println("WARNING: MenuItem created with null label");
        this->label = "NULL";
    }
    
    Serial.printf("MenuItem created: '%s' (ID:%d)\n", this->label, id);
}

void MenuItem::draw() {
    if (!display || !visible) return;
    
    // Draw background
    drawBackground();
    
    // Draw selection indicator and text
    if (selected) {
        drawSelectionIndicator();
    }
    
    // Draw the text
    drawText();
    
    // Draw pressed state overlay if needed
    if (pressed) {
        drawPressedState();
    }
}

void MenuItem::update() {
    // MenuItem doesn't need continuous updates
    // Could add animations here in the future
}

void MenuItem::onClick() {
    Serial.printf("MenuItem clicked: '%s' (ID:%d)\n", label, id);
    
    if (onSelectCallback) {
        onSelectCallback();
    } else {
        Serial.printf("WARNING: No callback set for MenuItem '%s'\n", label);
    }
}

void MenuItem::onPress() {
    pressed = true;
    markDirty();
    Serial.printf("MenuItem pressed: '%s'\n", label);
}

void MenuItem::onRelease() {
    pressed = false;
    markDirty();
    Serial.printf("MenuItem released: '%s'\n", label);
}

void MenuItem::setLabel(const char* newLabel) {
    if (newLabel && strcmp(label, newLabel) != 0) {
        label = newLabel;
        markDirty();
        Serial.printf("MenuItem label changed to: '%s'\n", label);
    }
}

void MenuItem::setSelected(bool isSelected) {
    if (selected != isSelected) {
        selected = isSelected;
        markDirty();
        Serial.printf("MenuItem '%s' selection: %s\n", label, isSelected ? "true" : "false");
    }
}

void MenuItem::setOnSelect(std::function<void()> callback) {
    onSelectCallback = callback;
}

void MenuItem::setOnSelect(void (*callback)()) {
    if (callback) {
        onSelectCallback = std::function<void()>(callback);
    } else {
        onSelectCallback = nullptr;
    }
}

int MenuItem::getPreferredWidth() const {
    // Calculate width based on text + padding + arrow space
    return getTextWidth() + TEXT_PADDING * 2 + ARROW_WIDTH;
}

bool MenuItem::validate() const {
    if (!Component::validate()) {
        return false;
    }
    
    if (!label) {
        Serial.printf("ERROR: MenuItem has null label!\n");
        return false;
    }
    
    if (height != DEFAULT_HEIGHT) {
        Serial.printf("WARNING: MenuItem height (%d) differs from default (%d)\n", 
                     height, DEFAULT_HEIGHT);
    }
    
    return true;
}

// Private drawing helpers

void MenuItem::drawBackground() {
    uint16_t bgColor = getBackgroundColor();
    fillRect(x, y, width, height, bgColor);
    
    // Draw border if selected
    if (selected) {
        uint16_t borderColor = getBorderColor();
        drawRect(x, y, width, height, borderColor);
    }
}

void MenuItem::drawSelectionIndicator() {
    // Draw the ">" arrow indicator
    uint16_t textColor = getTextColor();
    Component::drawText(">", x + TEXT_PADDING, y + (height - 8) / 2, textColor, 1);
}

void MenuItem::drawText() {
    uint16_t textColor = getTextColor();
    int textX = x + TEXT_PADDING;
    
    // Offset text if selected (to account for ">" indicator)
    if (selected) {
        textX += ARROW_WIDTH;
    } else {
        textX += 2; // Small offset for alignment
    }
    
    int textY = y + (height - 8) / 2;  // Center vertically (8px text height)
    Component::drawText(label, textX, textY, textColor, 1);
}

void MenuItem::drawPressedState() {
    // Draw subtle pressed overlay
    uint16_t pressedColor = getThemeColor("accentDark");
    
    // Draw semi-transparent overlay effect by drawing thin lines
    for (int i = 0; i < height; i += 2) {
        drawRect(x, y + i, width, 1, pressedColor);
    }
}

// Color helpers

uint16_t MenuItem::getBackgroundColor() const {
    if (selected) {
        return getThemeColor("accent");
    } else {
        return getThemeColor("surface");
    }
}

uint16_t MenuItem::getTextColor() const {
    if (selected) {
        return getThemeColor("selected");
    } else {
        return getThemeColor("primary");
    }
}

uint16_t MenuItem::getBorderColor() const {
    if (pressed) {
        return getThemeColor("accentDark");
    } else {
        return getThemeColor("border");
    }
}

int MenuItem::getTextWidth() const {
    // Approximate text width calculation
    // Each character is roughly 6 pixels wide at size 1
    return strlen(label) * 6;
}

// MenuItemFactory implementation

MenuItem* MenuItemFactory::createMenuItem(Adafruit_ST7789* display, 
                                         const char* label, 
                                         int id,
                                         std::function<void()> callback) {
    MenuItem* item = new MenuItem(display, label, id);
    item->setOnSelect(callback);
    return item;
}

MenuItem* MenuItemFactory::createMenuItem(Adafruit_ST7789* display,
                                         const char* label,
                                         int id,
                                         void (*callback)()) {
    MenuItem* item = new MenuItem(display, label, id);
    item->setOnSelect(callback);
    return item;
}

MenuItem* MenuItemFactory::createNavigationItem(Adafruit_ST7789* display,
                                               const char* label,
                                               std::function<void()> callback) {
    // Navigation items could have special styling in the future
    return createMenuItem(display, label, -1, callback);  // ID -1 for navigation
}

MenuItem* MenuItemFactory::createActionItem(Adafruit_ST7789* display,
                                           const char* label,
                                           std::function<void()> callback) {
    // Action items could have special styling in the future
    return createMenuItem(display, label, 0, callback);
}
