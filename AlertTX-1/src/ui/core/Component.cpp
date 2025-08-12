#include "Component.h"

// Screen dimensions for bounds checking
static const int SCREEN_WIDTH = 240;
static const int SCREEN_HEIGHT = 135;

Component::Component(Adafruit_ST7789* display, const char* name)
    : display(display), x(0), y(0), width(0), height(0), componentName(name) {
    
    if (!display) {
        Serial.println("ERROR: Component created with null display!");
    }
}

void Component::setBounds(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    markDirty();
    
    // Validate bounds and warn if off-screen
    if (!isOnScreen()) {
        Serial.printf("WARNING: Component '%s' bounds may be off-screen: (%d,%d,%d,%d)\n", 
                     componentName, x, y, width, height);
    }
}

void Component::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
    markDirty();
}

void Component::setSize(int w, int h) {
    this->width = w;
    this->height = h;
    markDirty();
}

void Component::setVisible(bool visible) {
    if (this->visible != visible) {
        this->visible = visible;
        markDirty();
    }
}

bool Component::containsPoint(int px, int py) const {
    return (px >= x && px < x + width && py >= y && py < y + height);
}

bool Component::intersects(int rx, int ry, int rw, int rh) const {
    return !(x >= rx + rw || rx >= x + width || y >= ry + rh || ry >= y + height);
}

uint16_t Component::getThemeColor(const char* colorType) const {
    // Use ThemeManager to get current theme colors
    if (strcmp(colorType, "background") == 0) {
        return ThemeManager::getBackground();
    } else if (strcmp(colorType, "surface") == 0) {
        return ThemeManager::getSurfaceBackground();
    } else if (strcmp(colorType, "primary") == 0) {
        return ThemeManager::getPrimaryText();
    } else if (strcmp(colorType, "secondary") == 0) {
        return ThemeManager::getSecondaryText();
    } else if (strcmp(colorType, "selected") == 0) {
        return ThemeManager::getSelectedText();
    } else if (strcmp(colorType, "accent") == 0) {
        return ThemeManager::getAccent();
    } else if (strcmp(colorType, "accentDark") == 0) {
        return ThemeManager::getAccentDark();
    } else if (strcmp(colorType, "border") == 0) {
        return ThemeManager::getBorder();
    }
    
    // Default to primary text color
    return ThemeManager::getPrimaryText();
}

void Component::printBounds() const {
    Serial.printf("Component '%s': bounds=(%d,%d,%d,%d), visible=%s, dirty=%s\n",
                 componentName, x, y, width, height, 
                 visible ? "true" : "false", 
                 needsRedraw ? "true" : "false");
}

bool Component::validate() const {
    if (!display) {
        Serial.printf("ERROR: Component '%s' has null display!\n", componentName);
        return false;
    }
    
    if (!hasValidBounds()) {
        Serial.printf("WARNING: Component '%s' has invalid bounds!\n", componentName);
        return false;
    }
    
    return true;
}

// Protected helper methods for subclasses
void Component::drawRect(int x, int y, int w, int h, uint16_t color) {
    if (display && visible) {
        display->drawRect(x, y, w, h, color);
    }
}

void Component::fillRect(int x, int y, int w, int h, uint16_t color) {
    if (display && visible) {
        display->fillRect(x, y, w, h, color);
    }
}

void Component::drawText(const char* text, int x, int y, uint16_t color, int size) {
    if (display && visible && text) {
        display->setTextColor(color);
        display->setTextSize(size);
        display->setCursor(x, y);
        display->print(text);
    }
}

// Private validation helpers
bool Component::isOnScreen() const {
    // Check if component is at least partially on screen
    return intersects(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool Component::hasValidBounds() const {
    return (width > 0 && height > 0);
}
