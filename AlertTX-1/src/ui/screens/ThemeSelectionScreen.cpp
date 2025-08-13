#include "ThemeSelectionScreen.h"
#include "../core/ScreenManager.h"
#include "../../config/SettingsManager.h"

// Static instance for callbacks
ThemeSelectionScreen* ThemeSelectionScreen::instance = nullptr;

ThemeSelectionScreen::ThemeSelectionScreen(Adafruit_ST7789* display)
    : Screen(display, "ThemeSelection", 2), selectedThemeIndex(0), originalThemeIndex(0) {
    
    instance = this;  // Set static instance for callbacks
    
    // Create theme menu container
    themeMenu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(themeMenu);
    
    // Setup menu items
    setupMenu();
    
    Serial.println("ThemeSelectionScreen created");
}

ThemeSelectionScreen::~ThemeSelectionScreen() {
    // Components are automatically cleaned up by Screen destructor
    instance = nullptr;
    Serial.println("ThemeSelectionScreen destroyed");
}

void ThemeSelectionScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("THEME_SELECTION");
    Serial.println("Entered ThemeSelectionScreen");
    
    // Store original theme for potential revert
    originalThemeIndex = ThemeManager::getCurrentThemeIndex();
    selectedThemeIndex = originalThemeIndex;
    
    // Reset menu selection to current theme
    themeMenu->setSelectedIndex(selectedThemeIndex);
    
    Serial.printf("ThemeSelectionScreen: Current theme is '%s' (index %d)\n", 
                  ThemeManager::getCurrentThemeName(), originalThemeIndex);
}

void ThemeSelectionScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("THEME_SELECTION");
    Serial.println("Exited ThemeSelectionScreen");
}

void ThemeSelectionScreen::update() {
    Screen::update();
    
    // Check if selected theme has changed for immediate preview
    int currentSelected = themeMenu->getSelectedIndex();
    if (currentSelected != selectedThemeIndex) {
        selectedThemeIndex = currentSelected;
        Serial.printf("ThemeSelectionScreen: Selected theme changed to %d ('%s')\n", 
                      selectedThemeIndex, ThemeManager::getThemeName(selectedThemeIndex));
        
        // Apply theme immediately for live preview (no persistence yet)
        ThemeManager::setThemeByIndex(selectedThemeIndex, false);
        markForFullRedraw();
    }
}

void ThemeSelectionScreen::draw() {
    Screen::draw();
    
    // Draw title
    DisplayUtils::drawTitle(display, "Themes");
    
    // Components are drawn by Screen::draw()
}

void ThemeSelectionScreen::handleButtonPress(int button) {
    Serial.printf("ThemeSelectionScreen: Button %d pressed\n", button);
    
    // Route to menu for navigation (A=Up, B=Down, C=Select)
    if (themeMenu) {
        themeMenu->handleButtonPress(button);
    }
}

// Theme selection actions

void ThemeSelectionScreen::onThemeSelected(int themeIndex) {
    Serial.printf("ThemeSelectionScreen: Theme %d ('%s') selected and saved\n", 
                  themeIndex, ThemeManager::getThemeName(themeIndex));
    
    // Apply theme with persistence (permanent selection)
    applyThemeImmediately(themeIndex);
    
    // Force full redraw with new theme
    markForFullRedraw();
}

void ThemeSelectionScreen::applySelectedTheme() {
    onThemeSelected(selectedThemeIndex);
}

// Dynamic callback handling
void ThemeSelectionScreen::createThemeCallback(int themeIndex) {
    // For now, we'll use a simple approach with lambda callbacks
    // This will be handled in createThemeMenuItems()
}

// Private helper methods

void ThemeSelectionScreen::setupMenu() {
    if (!themeMenu) return;
    
    createThemeMenuItems();
    
    Serial.println("ThemeSelectionScreen menu setup complete");
}

void ThemeSelectionScreen::createThemeMenuItems() {
    // Clear existing items
    themeMenu->clear();
    
    // Dynamically create menu items from ThemeManager
    for (int i = 0; i < ThemeManager::THEME_COUNT; i++) {
        const char* themeName = ThemeManager::getThemeName(i);
        
        // Create lambda callback that captures the theme index
        auto callback = [this, i]() {
            this->onThemeSelected(i);
        };
        
        // Cast lambda to std::function<void()> to resolve ambiguity
        themeMenu->addMenuItem(themeName, i, static_cast<std::function<void()>>(callback));
        
        Serial.printf("Added theme item: %s (index %d)\n", themeName, i);
    }
    
    // Auto-layout the menu
    themeMenu->autoLayout();
    
    Serial.printf("Created %d dynamic theme menu items\n", ThemeManager::THEME_COUNT);
}



void ThemeSelectionScreen::applyThemeImmediately(int themeIndex) {
    if (!ThemeManager::isValidThemeIndex(themeIndex)) {
        Serial.printf("ThemeSelectionScreen: Invalid theme index %d\n", themeIndex);
        return;
    }
    
    // Apply theme with persistence
    ThemeManager::setThemeByIndex(themeIndex, true);
    
    Serial.printf("ThemeSelectionScreen: Applied and saved theme '%s'\n", 
                  ThemeManager::getCurrentThemeName());
}

// Status helper methods

bool ThemeSelectionScreen::isCurrentTheme(int themeIndex) {
    return (themeIndex == ThemeManager::getCurrentThemeIndex());
}
