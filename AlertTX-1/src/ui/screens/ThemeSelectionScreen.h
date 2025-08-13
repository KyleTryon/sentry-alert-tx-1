#ifndef THEMESELECTIONSCREEN_H
#define THEMESELECTIONSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../core/Theme.h"
#include "../components/MenuContainer.h"

/**
 * ThemeSelectionScreen
 * 
 * Dedicated screen for theme selection with individual theme options,
 * real-time preview, and persistent storage.
 * 
 * Features:
 * - Dynamic theme list from ThemeManager
 * - Immediate theme preview on selection
 * - Current theme indicator ([CURRENT] status)
 * - Automatic persistence to NVS storage
 * - Long press for back navigation
 * 
 * Navigation:
 * - A=Up, B=Down: Navigate through theme options
 * - C=Select: Apply selected theme and save to storage
 * - Long Press Any Button: Return to settings (theme persisted)
 * 
 * Visual Layout:
 * ┌─────────────────────────────┐
 * │           Themes            │ ← Title
 * ├─────────────────────────────┤
 * │ > Default      [CURRENT]    │ ← Clean menu list
 * │   High Contrast             │
 * │   Terminal                  │
 * │   Amber                     │ 
 * └─────────────────────────────┘
 */

class ThemeSelectionScreen : public Screen {
private:
    MenuContainer* themeMenu;
    
    // Theme selection state
    int selectedThemeIndex;
    int originalThemeIndex;  // For potential revert functionality
    
    // Static instance for callbacks
    static ThemeSelectionScreen* instance;
    
public:
    ThemeSelectionScreen(Adafruit_ST7789* display);
    ~ThemeSelectionScreen();
    
    // Screen lifecycle
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    // Input handling
    void handleButtonPress(int button) override;
    
    // Theme selection actions
    void onThemeSelected(int themeIndex);
    void applySelectedTheme();
    
private:
    // Helper methods
    void setupMenu();
    void createThemeMenuItems();
    void applyThemeImmediately(int themeIndex);
    
    // Dynamic callback creation for theme count
    void createThemeCallback(int themeIndex);
    
    // Status indicators
    bool isCurrentTheme(int themeIndex);
};

#endif // THEMESELECTIONSCREEN_H
