#include "MenuContainer.h"

MenuContainer::MenuContainer(Adafruit_ST7789* display, int x, int y)
    : Component(display, "MenuContainer") {
    
    // Initialize menu item array
    for (int i = 0; i < MAX_MENU_ITEMS; i++) {
        menuItems[i] = nullptr;
    }
    
    // Set default bounds
    setBounds(x, y, 220, 108);  // Width matches current menu, height for ~4 items
    
    // Calculate visible items based on height
    visibleItemCount = (height - ITEM_SPACING) / (MenuItem::getDefaultHeight() + ITEM_SPACING);
    if (visibleItemCount > MAX_MENU_ITEMS) {
        visibleItemCount = MAX_MENU_ITEMS;
    }
    
    Serial.printf("MenuContainer created at (%d,%d) with %d visible items\n", 
                 x, y, visibleItemCount);
}

MenuContainer::~MenuContainer() {
    clear();
}

void MenuContainer::draw() {
    if (!display || !visible) return;
    
    // Draw background
    drawBackground();
    
    // Draw visible menu items
    int startIndex = scrollOffset;
    int endIndex = std::min(scrollOffset + visibleItemCount, itemCount);
    
    for (int i = startIndex; i < endIndex; i++) {
        if (menuItems[i] && menuItems[i]->isVisible()) {
            menuItems[i]->draw();
        }
    }
    
    // Draw scroll indicators if needed
    if (needsScrolling()) {
        drawScrollIndicators();
    }
}

void MenuContainer::update() {
    // Update visible menu items
    int startIndex = scrollOffset;
    int endIndex = std::min(scrollOffset + visibleItemCount, itemCount);
    
    for (int i = startIndex; i < endIndex; i++) {
        if (menuItems[i]) {
            menuItems[i]->update();
        }
    }
    
    // Validate scroll state
    validateScrollState();
}

bool MenuContainer::validate() const {
    if (!Component::validate()) {
        return false;
    }
    
    // Validate all menu items
    bool allValid = true;
    for (int i = 0; i < itemCount; i++) {
        if (menuItems[i] && !menuItems[i]->validate()) {
            Serial.printf("ERROR: MenuItem %d failed validation in MenuContainer\n", i);
            allValid = false;
        }
    }
    
    // Validate selection index
    if (selectedIndex < 0 || selectedIndex >= itemCount) {
        if (itemCount > 0) {  // Only error if we have items
            Serial.printf("ERROR: Invalid selectedIndex %d (itemCount: %d)\n", 
                         selectedIndex, itemCount);
            allValid = false;
        }
    }
    
    return allValid;
}

bool MenuContainer::addMenuItem(MenuItem* item) {
    if (!item) {
        Serial.println("ERROR: Attempted to add null MenuItem");
        return false;
    }
    
    if (itemCount >= MAX_MENU_ITEMS) {
        Serial.printf("ERROR: MenuContainer item limit (%d) exceeded\n", MAX_MENU_ITEMS);
        return false;
    }
    
    menuItems[itemCount] = item;
    itemCount++;
    
    // Auto-layout items
    autoLayout();
    
    Serial.printf("Added MenuItem '%s' to MenuContainer (%d/%d)\n", 
                 item->getLabel(), itemCount, MAX_MENU_ITEMS);
    
    return true;
}

bool MenuContainer::addMenuItem(const char* label, int id, std::function<void()> callback) {
    MenuItem* item = MenuItemFactory::createMenuItem(display, label, id, callback);
    return addMenuItem(item);
}

bool MenuContainer::addMenuItem(const char* label, int id, void (*callback)()) {
    MenuItem* item = MenuItemFactory::createMenuItem(display, label, id, callback);
    return addMenuItem(item);
}

bool MenuContainer::removeMenuItem(int index) {
    if (!isValidIndex(index)) {
        Serial.printf("ERROR: Invalid index %d for removeMenuItem\n", index);
        return false;
    }
    
    // Delete the menu item
    delete menuItems[index];
    
    // Shift remaining items
    for (int i = index; i < itemCount - 1; i++) {
        menuItems[i] = menuItems[i + 1];
    }
    
    itemCount--;
    menuItems[itemCount] = nullptr;
    
    // Adjust selection if needed
    if (selectedIndex >= itemCount && itemCount > 0) {
        selectedIndex = itemCount - 1;
    }
    
    // Re-layout items
    autoLayout();
    
    Serial.printf("Removed MenuItem at index %d (%d remaining)\n", index, itemCount);
    return true;
}

void MenuContainer::clear() {
    Serial.printf("Clearing %d menu items from MenuContainer\n", itemCount);
    
    for (int i = 0; i < itemCount; i++) {
        delete menuItems[i];
        menuItems[i] = nullptr;
    }
    
    itemCount = 0;
    selectedIndex = 0;
    scrollOffset = 0;
}

void MenuContainer::moveUp() {
    if (!navigationEnabled || itemCount == 0) return;
    
    int oldIndex = selectedIndex;
    selectedIndex = (selectedIndex - 1 + itemCount) % itemCount;
    
    updateSelection();
    scrollToSelected();
    markDirty();  // Force screen redraw
    
    Serial.printf("Menu navigation: %d -> %d (up)\n", oldIndex, selectedIndex);
}

void MenuContainer::moveDown() {
    if (!navigationEnabled || itemCount == 0) return;
    
    int oldIndex = selectedIndex;
    selectedIndex = (selectedIndex + 1) % itemCount;
    
    updateSelection();
    scrollToSelected();
    markDirty();  // Force screen redraw
    
    Serial.printf("Menu navigation: %d -> %d (down)\n", oldIndex, selectedIndex);
}

void MenuContainer::selectCurrent() {
    if (!navigationEnabled || itemCount == 0) return;
    
    MenuItem* current = getSelectedItem();
    if (current) {
        inputHandler.setTarget(current);
        current->onClick();
    }
}

void MenuContainer::setSelectedIndex(int index) {
    if (isValidIndex(index)) {
        selectedIndex = index;
        updateSelection();
        scrollToSelected();
    }
}



void MenuContainer::handleButtonPress(int button) {
    switch (button) {
        case 0:  // Button A - Up
            moveUp();
            break;
        case 1:  // Button B - Down
            moveDown();
            break;
        case 2:  // Button C - Select
            selectCurrent();
            break;
    }
}

MenuItem* MenuContainer::getSelectedItem() const {
    if (isValidIndex(selectedIndex)) {
        return menuItems[selectedIndex];
    }
    return nullptr;
}

MenuItem* MenuContainer::getItem(int index) const {
    if (isValidIndex(index)) {
        return menuItems[index];
    }
    return nullptr;
}

void MenuContainer::setVisibleItemCount(int count) {
    if (count > 0 && count <= MAX_MENU_ITEMS) {
        visibleItemCount = count;
        
        // Adjust height to fit visible items
        int newHeight = (visibleItemCount * MenuItem::getDefaultHeight()) + 
                       ((visibleItemCount - 1) * ITEM_SPACING);
        setSize(width, newHeight);
        
        autoLayout();
    }
}

void MenuContainer::autoLayout() {
    layoutItems();
    updateScrolling();
    markDirty();
}

void MenuContainer::updateScrolling() {
    // Update scroll state to ensure selected item is visible
    scrollToSelected();
}

void MenuContainer::setItemSpacing(int spacing) {
    if (spacing >= 0) {
        // We can't modify the static ITEM_SPACING, but we could make it non-static
        // For now, just trigger a re-layout
        autoLayout();
    }
}

void MenuContainer::printMenuState() const {
    Serial.printf("MenuContainer state:\n");
    Serial.printf("  Items: %d/%d\n", itemCount, MAX_MENU_ITEMS);
    Serial.printf("  Selected: %d\n", selectedIndex);
    Serial.printf("  Scroll: %d\n", scrollOffset);
    Serial.printf("  Visible: %d\n", visibleItemCount);
    Serial.printf("  Bounds: (%d,%d,%d,%d)\n", x, y, width, height);
    
    for (int i = 0; i < itemCount; i++) {
        if (menuItems[i]) {
            Serial.printf("  [%d] %s %s\n", i, menuItems[i]->getLabel(),
                         (i == selectedIndex) ? "*" : "");
        }
    }
}

// Private implementation methods

void MenuContainer::layoutItems() {
    int currentY = y;
    
    for (int i = 0; i < itemCount; i++) {
        if (menuItems[i]) {
            // Set item position and size
            menuItems[i]->setBounds(x, currentY, width, MenuItem::getDefaultHeight());
            currentY += MenuItem::getDefaultHeight() + ITEM_SPACING;
        }
    }
}

int MenuContainer::calculateTotalHeight() const {
    if (itemCount == 0) return 0;
    return (itemCount * MenuItem::getDefaultHeight()) + ((itemCount - 1) * ITEM_SPACING);
}

bool MenuContainer::needsScrolling() const {
    return itemCount > visibleItemCount;
}

void MenuContainer::drawScrollIndicators() {
    // Draw simple scroll indicators on the right edge
    uint16_t indicatorColor = getThemeColor("border");
    
    // Up arrow if can scroll up
    if (scrollOffset > 0) {
        drawText("^", x + width - 10, y + 2, indicatorColor, 1);
    }
    
    // Down arrow if can scroll down
    if (scrollOffset + visibleItemCount < itemCount) {
        drawText("v", x + width - 10, y + height - 10, indicatorColor, 1);
    }
}

void MenuContainer::drawBackground() {
    fillRect(x, y, width, height, getThemeColor("surface"));
    drawRect(x, y, width, height, getThemeColor("border"));
}

void MenuContainer::updateSelection() {
    // Update selection state of all items
    for (int i = 0; i < itemCount; i++) {
        if (menuItems[i]) {
            menuItems[i]->setSelected(i == selectedIndex);
        }
    }
}

void MenuContainer::scrollToSelected() {
    if (!needsScrolling()) {
        scrollOffset = 0;
        return;
    }
    
    // Scroll to keep selected item visible
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + visibleItemCount) {
        scrollOffset = selectedIndex - visibleItemCount + 1;
    }
    
    validateScrollState();
}

bool MenuContainer::isItemVisible(int index) const {
    return (index >= scrollOffset && index < scrollOffset + visibleItemCount);
}

bool MenuContainer::isValidIndex(int index) const {
    return (index >= 0 && index < itemCount);
}

void MenuContainer::validateScrollState() {
    // Ensure scroll offset is valid
    if (scrollOffset < 0) {
        scrollOffset = 0;
    }
    
    if (needsScrolling()) {
        int maxScroll = itemCount - visibleItemCount;
        if (scrollOffset > maxScroll) {
            scrollOffset = maxScroll;
        }
    } else {
        scrollOffset = 0;
    }
}

// MenuBuilder implementation

MenuBuilder::MenuBuilder(Adafruit_ST7789* display, int x, int y)
    : display(display) {
    menu = new MenuContainer(display, x, y);
}

MenuBuilder& MenuBuilder::addItem(const char* label, std::function<void()> callback) {
    menu->addMenuItem(label, 0, callback);
    return *this;
}

MenuBuilder& MenuBuilder::addItem(const char* label, void (*callback)()) {
    menu->addMenuItem(label, 0, callback);
    return *this;
}

MenuBuilder& MenuBuilder::addSeparator() {
    // Could add a special separator item here
    return *this;
}

MenuBuilder& MenuBuilder::setPosition(int x, int y) {
    menu->setPosition(x, y);
    return *this;
}

MenuBuilder& MenuBuilder::setSize(int w, int h) {
    menu->setSize(w, h);
    return *this;
}

MenuBuilder& MenuBuilder::setVisibleItems(int count) {
    menu->setVisibleItemCount(count);
    return *this;
}

MenuContainer* MenuBuilder::build() {
    menu->autoLayout();
    return menu;
}

// Static convenience methods
MenuContainer* MenuBuilder::createMainMenu(Adafruit_ST7789* display) {
    MenuBuilder builder(display);
    builder.addItem("Alerts", std::function<void()>([]() { Serial.println("Alerts selected"); }));
    builder.addItem("Games", std::function<void()>([]() { Serial.println("Games selected"); }));
    builder.addItem("Settings", std::function<void()>([]() { Serial.println("Settings selected"); }));
    return builder.build();
}

MenuContainer* MenuBuilder::createSettingsMenu(Adafruit_ST7789* display) {
    MenuBuilder builder(display);
    builder.addItem("WiFi Config", std::function<void()>([]() { Serial.println("WiFi Config"); }));
    builder.addItem("Display", std::function<void()>([]() { Serial.println("Display Settings"); }));
    builder.addItem("Audio", std::function<void()>([]() { Serial.println("Audio Settings"); }));
    builder.addItem("Back", std::function<void()>([]() { Serial.println("Back to main"); }));
    return builder.build();
}

MenuContainer* MenuBuilder::createGamesMenu(Adafruit_ST7789* display) {
    MenuBuilder builder(display);
    builder.addItem("Snake", std::function<void()>([]() { Serial.println("Snake game"); }));
    builder.addItem("Pong", std::function<void()>([]() { Serial.println("Pong game"); }));
    builder.addItem("BeeperHero", std::function<void()>([]() { Serial.println("BeeperHero game"); }));
    builder.addItem("Back", std::function<void()>([]() { Serial.println("Back to main"); }));
    return builder.build();
}
