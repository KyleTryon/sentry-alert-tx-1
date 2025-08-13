#include "Screen.h"

Screen::Screen(Adafruit_ST7789* display, const char* name, int id)
    : display(display), screenName(name), screenId(id) {
    
    // Initialize component array
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        components[i] = nullptr;
    }
    
    if (!display) {
        Serial.printf("ERROR: Screen '%s' created with null display!\n", name);
    }
    
    Serial.printf("Screen '%s' (ID:%d) created\n", name, id);
}

Screen::~Screen() {
    clearComponents();
    Serial.printf("Screen '%s' destroyed\n", screenName);
}

void Screen::enter() {
    active = true;
    needsFullRedraw = true;
    
    Serial.printf("Entering screen: %s\n", screenName);
    
    // Validate all components on screen entry
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && !components[i]->validate()) {
            Serial.printf("WARNING: Component %d failed validation on screen '%s'\n", i, screenName);
        }
    }
}

void Screen::exit() {
    active = false;
    Serial.printf("Exiting screen: %s\n", screenName);
}

void Screen::update() {
    if (!active) return;
    
    // Update all components
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && components[i]->isVisible()) {
            components[i]->update();
        }
    }
}

void Screen::draw() {
    if (!active || !display) return;
    
    // Full screen redraw if needed
    if (needsFullRedraw) {
        clearScreen();
        needsFullRedraw = false;
        
        // Mark all components dirty for redraw
        for (int i = 0; i < componentCount; i++) {
            if (components[i]) {
                components[i]->markDirty();
            }
        }
    }
    
    // Draw all visible components that need redrawing
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && components[i]->isVisible() && components[i]->isDirty()) {
            components[i]->draw();
            components[i]->clearDirty();
        }
    }
}

bool Screen::addComponent(Component* component) {
    if (!component) {
        Serial.printf("ERROR: Attempted to add null component to screen '%s'\n", screenName);
        return false;
    }
    
    if (componentCount >= MAX_COMPONENTS) {
        Serial.printf("ERROR: Screen '%s' component limit (%d) exceeded!\n", screenName, MAX_COMPONENTS);
        return false;
    }
    
    // Check for duplicate components
    if (findComponentIndex(component) >= 0) {
        Serial.printf("WARNING: Component already exists in screen '%s'\n", screenName);
        return false;
    }
    
    components[componentCount] = component;
    componentCount++;
    
    Serial.printf("Added component '%s' to screen '%s' (%d/%d)\n", 
                 component->getName(), screenName, componentCount, MAX_COMPONENTS);
    
    return true;
}

bool Screen::removeComponent(Component* component) {
    int index = findComponentIndex(component);
    if (index < 0) {
        Serial.printf("WARNING: Component not found in screen '%s'\n", screenName);
        return false;
    }
    
    // Shift remaining components down
    for (int i = index; i < componentCount - 1; i++) {
        components[i] = components[i + 1];
    }
    
    componentCount--;
    components[componentCount] = nullptr;
    
    Serial.printf("Removed component from screen '%s' (%d/%d remaining)\n", 
                 screenName, componentCount, MAX_COMPONENTS);
    
    return true;
}

void Screen::clearComponents() {
    Serial.printf("Clearing %d components from screen '%s'\n", componentCount, screenName);
    
    for (int i = 0; i < componentCount; i++) {
        components[i] = nullptr;  // Don't delete - components owned elsewhere
    }
    componentCount = 0;
}

Component* Screen::getComponent(int index) const {
    if (index >= 0 && index < componentCount) {
        return components[index];
    }
    return nullptr;
}

void Screen::setActive(bool active) {
    if (this->active != active) {
        this->active = active;
        if (active) {
            enter();
        } else {
            exit();
        }
    }
}

bool Screen::validate() const {
    if (!display) {
        Serial.printf("ERROR: Screen '%s' has null display!\n", screenName);
        return false;
    }
    
    // Validate all components
    bool allValid = true;
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && !components[i]->validate()) {
            allValid = false;
        }
    }
    
    // Check for overlapping components (warning only)
    if (hasOverlappingComponents()) {
        Serial.printf("WARNING: Screen '%s' has overlapping components\n", screenName);
    }
    
    return allValid;
}

void Screen::printComponents() const {
    Serial.printf("Screen '%s' components (%d/%d):\n", screenName, componentCount, MAX_COMPONENTS);
    for (int i = 0; i < componentCount; i++) {
        if (components[i]) {
            Serial.printf("  [%d] %s\n", i, components[i]->getName());
            components[i]->printBounds();
        }
    }
}

// Protected helper methods

void Screen::clearScreen() {
    if (display) {
        display->fillScreen(ThemeManager::getBackground());
    }
}

void Screen::drawTitle(const char* title, int x, int y) {
    if (display && title) {
        display->setTextColor(ThemeManager::getPrimaryText());
        display->setTextSize(2);
        display->setCursor(x, y);
        display->print(title);
    }
}

Component* Screen::findComponentByName(const char* name) const {
    if (!name) return nullptr;
    
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && strcmp(components[i]->getName(), name) == 0) {
            return components[i];
        }
    }
    return nullptr;
}

int Screen::findComponentIndex(Component* component) const {
    for (int i = 0; i < componentCount; i++) {
        if (components[i] == component) {
            return i;
        }
    }
    return -1;
}

bool Screen::hasOverlappingComponents() const {
    for (int i = 0; i < componentCount; i++) {
        for (int j = i + 1; j < componentCount; j++) {
            if (components[i] && components[j]) {
                if (components[i]->intersects(
                    components[j]->getX(), components[j]->getY(),
                    components[j]->getWidth(), components[j]->getHeight())) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Screen::optimizeComponentOrder() {
    // Simple optimization: move invisible components to end
    // More complex sorting could be added here for draw order optimization
    int writeIndex = 0;
    
    // First pass: visible components
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && components[i]->isVisible()) {
            if (writeIndex != i) {
                components[writeIndex] = components[i];
            }
            writeIndex++;
        }
    }
    
    // Second pass: invisible components
    for (int i = 0; i < componentCount; i++) {
        if (components[i] && !components[i]->isVisible()) {
            if (writeIndex != i) {
                components[writeIndex] = components[i];
            }
            writeIndex++;
        }
    }
}
