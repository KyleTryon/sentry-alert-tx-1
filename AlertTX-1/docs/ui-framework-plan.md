# Alert TX-1 UI Framework Plan
## FlipperZero-Inspired Menu System

### Overview

This document outlines the plan for implementing a fast, non-blocking, monochrome UI system inspired by the FlipperZero interface. The system will use the Arduino_GFX library with a 240x135 TFT display and three physical buttons for navigation.

**Target Platform:** Adafruit ESP32-S3 Reverse TFT Feather (240x135 TFT)
**Graphics Library:** [Arduino_GFX](https://github.com/moononournation/Arduino_GFX)
**Display Resolution:** 240x135 pixels
**Color Depth:** 16-bit (65,536 colors) - configured for monochrome themes

---

## 🎯 Design Goals

### Core Requirements
- **Fast & Non-blocking:** 30+ FPS refresh rate, no blocking operations
- **Simple & Clean:** Minimalist FlipperZero-inspired design
- **Monochrome:** Configurable color themes (black/white, green/black, amber/black)
- **Responsive:** Immediate button feedback with proper debouncing
- **Memory Efficient:** Optimized for ESP32-S3 constraints

### UI Components
- **Status Bar:** Top 16px showing battery, WiFi, time
- **Menu System:** Scrollable list with selection indicator
- **Navigation:** Up/Down cycling with visual feedback
- **Icons:** 16x16 pixel icons for menu items

---

## 🏗️ Architecture Overview

### File Structure
```
src/ui/
├── core/
│   ├── UIManager.h/cpp          # Main UI orchestrator
│   ├── Screen.h                 # Base screen class
│   ├── Theme.h                  # Color theme definitions
│   └── EventSystem.h/cpp        # Event queue management
├── components/
│   ├── StatusBar.h/cpp          # Top status bar
│   ├── Menu.h/cpp               # Menu list component
│   ├── MenuItem.h               # Menu item structure
│   └── IconRenderer.h/cpp       # Icon drawing utilities
├── screens/
│   ├── MainMenuScreen.h/cpp     # Main menu screen
│   ├── AlertsScreen.h/cpp       # Alerts screen
│   ├── GamesScreen.h/cpp        # Games screen
│   └── SettingsScreen.h/cpp     # Settings screen
└── renderer/
    └── DisplayRenderer.h/cpp    # Arduino_GFX wrapper

src/hardware/
├── ButtonManager.h/cpp          # Button state management (moved from UI)
├── PowerManager.h/cpp           # Power management (existing)
├── LED.h/cpp                    # LED control (existing)
└── Buzzer.h/cpp                 # Audio control (existing)
```

---

## 🎨 Theme System

### Color Palette Configuration
```cpp
// src/ui/core/Theme.h
struct Theme {
    uint16_t background;      // Main background color
    uint16_t foreground;      // Text and icon color
    uint16_t accent;          // Selection/highlight color
    uint16_t secondary;       // Secondary text color
    uint16_t border;          // Border/divider color
};

// Predefined themes
extern const Theme THEME_MONOCHROME;    // Black background, white text
extern const Theme THEME_GREEN;         // Very dark green background, bright green text (Pip-Boy style)
extern const Theme THEME_AMBER;         // Amber background, black text (FlipperZero style)
extern const Theme THEME_INVERTED;      // White background, black text
```

### Color Conversion Utilities
```cpp
// RGB565 color helpers for monochrome themes
#define RGB565_BLACK     0x0000
#define RGB565_WHITE     0xFFFF
#define RGB565_GREEN     0x07E0
#define RGB565_AMBER     0xFD20
#define RGB565_GRAY      0x8410

// FlipperZero-style colors (black text on amber background)
#define RGB565_FLIPPER_AMBER    0xFD20  // Bright amber/orange background
#define RGB565_FLIPPER_BG       0x0000  // Pure black text

// Pip-Boy style colors (bright green on very dark green)
#define RGB565_PIPBOY_GREEN     0x07E0  // Bright green (close to white)
#define RGB565_PIPBOY_BG        0x0200  // Very dark green (nearly black)
```

### Theme Implementation Examples
```cpp
// src/ui/core/Theme.cpp
const Theme THEME_MONOCHROME = {
    .background = RGB565_BLACK,
    .foreground = RGB565_WHITE,
    .accent = RGB565_GRAY,
    .secondary = RGB565_GRAY,
    .border = RGB565_GRAY
};

const Theme THEME_GREEN = {
    .background = RGB565_PIPBOY_BG,      // Very dark green (nearly black)
    .foreground = RGB565_PIPBOY_GREEN,   // Bright green text
    .accent = RGB565_WHITE,              // White for highlights
    .secondary = RGB565_GRAY,            // Gray for secondary text
    .border = RGB565_PIPBOY_GREEN        // Green borders
};

const Theme THEME_AMBER = {
    .background = RGB565_FLIPPER_AMBER,  // Bright amber background
    .foreground = RGB565_FLIPPER_BG,     // Pure black text
    .accent = RGB565_BLACK,              // Black for highlights
    .secondary = RGB565_GRAY,            // Gray for secondary text
    .border = RGB565_BLACK               // Black borders
};

const Theme THEME_INVERTED = {
    .background = RGB565_WHITE,
    .foreground = RGB565_BLACK,
    .accent = RGB565_GRAY,
    .secondary = RGB565_GRAY,
    .border = RGB565_GRAY
};

---

## 🎮 Button Management System

### Button Configuration
Based on official ESP32-S3 Reverse TFT Feather documentation and [settings.h](src/config/settings.h):

```cpp
// src/hardware/ButtonManager.h
class ButtonManager {
private:
    // Button pins (from settings.h and official documentation)
    static const int BUTTON_A_PIN = 0;  // GPIO0 - Built-in D0/BOOT button
    static const int BUTTON_B_PIN = 1;  // GPIO1 - Built-in D1 button  
    static const int BUTTON_C_PIN = 2;  // GPIO2 - Built-in D2 button
    
    // Button states (from official ESP32-S3 Reverse TFT Feather documentation)
    // Button A (D0/BOOT): Pulled HIGH by default, goes LOW when pressed
    // Button B (D1): Pulled LOW by default, goes HIGH when pressed  
    // Button C (D2): Pulled LOW by default, goes HIGH when pressed
    
    // Debouncing configuration (from settings.h)
    static const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce
    static const unsigned long LONG_PRESS_DELAY = 1000; // 1 second for long press (from settings.h)
    
    struct ButtonState {
        bool currentState;
        bool lastState;
        bool pressed;
        bool released;
        bool longPressed;
        unsigned long lastDebounceTime;
        unsigned long pressStartTime;
    };
    
    ButtonState buttons[3];  // A, B, C

public:
    void begin();
    void update();  // Call in main loop
    
    // Button state queries
    bool isPressed(int buttonIndex);
    bool wasPressed(int buttonIndex);
    bool isLongPressed(int buttonIndex);
    
    // Button constants
    static const int BUTTON_A = 0;
    static const int BUTTON_B = 1;
    static const int BUTTON_C = 2;
};
```

### Button Mapping Strategy
```cpp
// Navigation mapping (FlipperZero style)
// Button A (D0/BOOT): Select/Enter
// Button B (D1): Up/Previous  
// Button C (D2): Down/Next

enum ButtonAction {
    ACTION_NONE,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_SELECT,
    ACTION_BACK,
    ACTION_LONG_PRESS
};
```

### Hardware Integration
The ButtonManager integrates with existing hardware components:

```cpp
// src/hardware/ButtonManager.h - Integration with existing hardware
#include "../config/settings.h"
#include "PowerManager.h"
#include "LED.h"
#include "Buzzer.h"

class ButtonManager {
private:
    PowerManager* powerManager;
    LED* statusLED;
    Buzzer* buzzer;
    
    // Button wake-up support for deep sleep
    void setupWakeSources() {
        // Configure buttons for deep sleep wake-up
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);  // D0/BOOT - wake on LOW
        esp_sleep_enable_ext1_wakeup(
            ((1ULL << GPIO_NUM_1) | (1ULL << GPIO_NUM_2)), 
            ESP_EXT1_WAKEUP_ANY_HIGH  // D1/D2 - wake on HIGH
        );
    }
    
public:
    void begin(PowerManager* pm, LED* led, Buzzer* bz) {
        powerManager = pm;
        statusLED = led;
        buzzer = bz;
        setupWakeSources();
    }
    
    // Provide haptic feedback on button press
    void provideFeedback(int buttonId) {
        if (buzzer) {
            buzzer->playTone(800, 50);  // Short beep
        }
        if (statusLED) {
            statusLED->blink(100);      // Brief LED flash
        }
    }
};
```

---

## 📱 UI Component System

### 1. Status Bar Component
```cpp
// src/ui/components/StatusBar.h
class StatusBar {
private:
    int y = 0;  // Top of screen
    int height = 16;
    
public:
    void draw(Arduino_GFX* display, const Theme& theme);
    void updateBatteryLevel(int percentage);
    void updateWiFiStatus(bool connected);
    void updateTime(const char* timeStr);
    
private:
    void drawBatteryIcon(int x, int y, int level);
    void drawWiFiIcon(int x, int y, bool connected);
    void drawTime(int x, int y, const char* time);
};
```

**Status Bar Layout (240px wide):**
```
┌─────────────────────────────────────────────────────────────────┐
│ [WiFi] [Battery] [Time]                                        │
│ 16px height                                                     │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Menu Component
```cpp
// src/ui/components/Menu.h
struct MenuItem {
    const char* label;
    const Icon* icon;
    int id;
    void (*action)();  // Callback function
};

class Menu {
private:
    MenuItem* items;
    int itemCount;
    int selectedIndex = 0;
    int visibleItems = 6;  // 240x135 display can show ~6 items
    int itemHeight = 18;   // 16px icon + 2px padding
    
public:
    Menu(MenuItem* items, int count);
    void draw(Arduino_GFX* display, const Theme& theme, int yOffset);
    void navigateUp();
    void navigateDown();
    void select();
    int getSelectedIndex() const { return selectedIndex; }
};
```

**Menu Layout:**
```
┌─────────────────────────────────────────────────────────────────┐
│ [WiFi] [Battery] [Time]                                        │
├─────────────────────────────────────────────────────────────────┤
│ ▶ Alerts                    [alert icon]                       │
│   Games                     [game icon]                        │
│   Settings                  [settings icon]                    │
│   Ringtones                 [music icon]                       │
│   BeeperHero                [play icon]                        │
│   System Info               [info icon]                        │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Icon Renderer
```cpp
// src/ui/components/IconRenderer.h
class IconRenderer {
public:
    // Draw icon at position with theme colors
    static void drawIcon(Arduino_GFX* display, const Icon& icon, 
                        int x, int y, const Theme& theme);
    
    // Draw icon with custom colors
    static void drawIcon(Arduino_GFX* display, const Icon& icon,
                        int x, int y, uint16_t color);
    
    // Draw icon with transparency support
    static void drawIconTransparent(Arduino_GFX* display, const Icon& icon,
                                   int x, int y, uint16_t color, uint16_t bgColor);
};
```

---

## 🖥️ Screen Management

### Base Screen Class
```cpp
// src/ui/core/Screen.h
class Screen {
protected:
    Arduino_GFX* display;
    Theme theme;
    
public:
    virtual ~Screen() = default;
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleInput(ButtonAction action) = 0;
    
    void setDisplay(Arduino_GFX* d) { display = d; }
    void setTheme(const Theme& t) { theme = t; }
};
```

### Main Menu Screen
```cpp
// src/ui/screens/MainMenuScreen.h
class MainMenuScreen : public Screen {
private:
    StatusBar statusBar;
    Menu menu;
    
    // Menu items
    MenuItem menuItems[6] = {
        {"Alerts", &alert_icon, 1, nullptr},
        {"Games", &game_icon, 2, nullptr},
        {"Settings", &settings_icon, 3, nullptr},
        {"Ringtones", &music_icon, 4, nullptr},
        {"BeeperHero", &play_icon, 5, nullptr},
        {"System Info", &info_icon, 6, nullptr}
    };
    
public:
    MainMenuScreen();
    void begin() override;
    void update() override;
    void draw() override;
    void handleInput(ButtonAction action) override;
};
```

---

## 🎮 Event System & Component Lifecycle

### Event-Driven Architecture
The UI system uses an event-driven architecture with a centralized event queue and state management:

```cpp
// src/ui/core/EventSystem.h
enum class UIEventType {
    BUTTON_PRESS,
    BUTTON_RELEASE,
    BUTTON_LONG_PRESS,
    MENU_SELECT,
    MENU_NAVIGATE,
    SCREEN_CHANGE,
    RENDER_REQUEST,
    THEME_CHANGE
};

struct UIEvent {
    UIEventType type;
    int buttonId;           // For button events
    int menuIndex;          // For menu events
    void* data;             // Additional event data
    unsigned long timestamp;
};

class EventSystem {
private:
    static const int MAX_EVENTS = 16;
    UIEvent eventQueue[MAX_EVENTS];
    int queueHead = 0;
    int queueTail = 0;
    
public:
    void pushEvent(UIEventType type, int id = 0, void* data = nullptr);
    bool popEvent(UIEvent& event);
    void clear();
    bool isEmpty() const;
};
```

### Button Event Lifecycle
```cpp
// src/hardware/ButtonManager.cpp - Detailed Implementation
class ButtonManager {
private:
    struct ButtonState {
        bool currentState;
        bool lastState;
        bool pressed;
        bool released;
        bool longPressed;
        bool longPressTriggered;
        unsigned long lastDebounceTime;
        unsigned long pressStartTime;
        unsigned long lastRepeatTime;
        int repeatCount;
    };
    
    ButtonState buttons[3];
    EventSystem* eventSystem;
    
    // Button-specific configurations (from settings.h)
    static const unsigned long DEBOUNCE_DELAY = 50;
    static const unsigned long LONG_PRESS_DELAY = 1000;
    static const unsigned long REPEAT_DELAY = 200;  // For held buttons
    
public:
    void update() {
        unsigned long currentTime = millis();
        
        for (int i = 0; i < 3; i++) {
            ButtonState& btn = buttons[i];
            
            // Read current button state (from official ESP32-S3 Reverse TFT Feather documentation)
            bool reading;
            switch (i) {
                case 0: // Button A (D0/BOOT) - pulled HIGH, goes LOW when pressed
                    reading = (digitalRead(BUTTON_A_PIN) == LOW);
                    break;
                case 1: // Button B (D1) - pulled LOW, goes HIGH when pressed
                    reading = (digitalRead(BUTTON_B_PIN) == HIGH);
                    break;
                case 2: // Button C (D2) - pulled LOW, goes HIGH when pressed
                    reading = (digitalRead(BUTTON_C_PIN) == HIGH);
                    break;
            }
            
            // Debouncing
            if (reading != btn.lastState) {
                btn.lastDebounceTime = currentTime;
            }
            
            if ((currentTime - btn.lastDebounceTime) > DEBOUNCE_DELAY) {
                if (reading != btn.currentState) {
                    btn.currentState = reading;
                    
                    if (btn.currentState) {
                        // Button pressed
                        btn.pressed = true;
                        btn.pressStartTime = currentTime;
                        btn.repeatCount = 0;
                        
                        // Generate button press event
                        eventSystem->pushEvent(UIEventType::BUTTON_PRESS, i);
                        
                    } else {
                        // Button released
                        btn.released = true;
                        btn.longPressTriggered = false;
                        
                        // Generate button release event
                        eventSystem->pushEvent(UIEventType::BUTTON_RELEASE, i);
                    }
                }
            }
            
            // Long press detection
            if (btn.currentState && !btn.longPressTriggered) {
                if ((currentTime - btn.pressStartTime) > LONG_PRESS_DELAY) {
                    btn.longPressed = true;
                    btn.longPressTriggered = true;
                    
                    // Generate long press event
                    eventSystem->pushEvent(UIEventType::BUTTON_LONG_PRESS, i);
                }
            }
            
            // Button repeat (for held buttons)
            if (btn.currentState && btn.longPressTriggered) {
                if ((currentTime - btn.lastRepeatTime) > REPEAT_DELAY) {
                    btn.lastRepeatTime = currentTime;
                    btn.repeatCount++;
                    
                    // Generate repeat event
                    eventSystem->pushEvent(UIEventType::BUTTON_PRESS, i);
                }
            }
            
            btn.lastState = reading;
        }
    }
};
```

### Menu Selection Lifecycle
```cpp
// src/ui/components/Menu.cpp - Selection Management
class Menu {
private:
    MenuItem* items;
    int itemCount;
    int selectedIndex = 0;
    int visibleItems = 6;
    int itemHeight = 18;
    bool selectionChanged = false;
    
    // Selection animation
    unsigned long lastSelectionTime = 0;
    bool selectionHighlighted = false;
    
public:
    void handleInput(UIEvent& event) {
        switch (event.type) {
            case UIEventType::BUTTON_PRESS:
                switch (event.buttonId) {
                    case 1: // Button B - Up
                        navigateUp();
                        break;
                    case 2: // Button C - Down
                        navigateDown();
                        break;
                    case 0: // Button A - Select
                        select();
                        break;
                }
                break;
        }
    }
    
    void navigateUp() {
        if (selectedIndex > 0) {
            selectedIndex--;
            selectionChanged = true;
            lastSelectionTime = millis();
            
            // Generate menu navigation event
            EventSystem::getInstance().pushEvent(
                UIEventType::MENU_NAVIGATE, 
                selectedIndex
            );
        }
    }
    
    void navigateDown() {
        if (selectedIndex < itemCount - 1) {
            selectedIndex++;
            selectionChanged = true;
            lastSelectionTime = millis();
            
            // Generate menu navigation event
            EventSystem::getInstance().pushEvent(
                UIEventType::MENU_NAVIGATE, 
                selectedIndex
            );
        }
    }
    
    void select() {
        if (items[selectedIndex].action) {
            // Generate menu selection event
            EventSystem::getInstance().pushEvent(
                UIEventType::MENU_SELECT, 
                selectedIndex,
                &items[selectedIndex]
            );
            
            // Execute menu item action
            items[selectedIndex].action();
        }
    }
};
```

## 🎨 Component Rendering System

### Rendering Pipeline
```cpp
// src/ui/renderer/DisplayRenderer.cpp - Rendering Pipeline
class DisplayRenderer {
private:
    Arduino_Canvas canvas;
    Arduino_GFX* display;
    bool needsFullRedraw = true;
    Rectangle dirtyAreas[8];
    int dirtyAreaCount = 0;
    
public:
    void begin(Arduino_GFX* display) {
        this->display = display;
        canvas.begin(display);
        canvas.setRotation(1);
    }
    
    void markDirty(int x, int y, int w, int h) {
        if (dirtyAreaCount < 8) {
            dirtyAreas[dirtyAreaCount] = {x, y, w, h};
            dirtyAreaCount++;
        }
    }
    
    void clearDirtyAreas() {
        dirtyAreaCount = 0;
    }
    
    void render() {
        if (needsFullRedraw) {
            // Full screen redraw
            canvas.fillScreen(currentTheme.background);
            needsFullRedraw = false;
        } else {
            // Partial redraw of dirty areas
            for (int i = 0; i < dirtyAreaCount; i++) {
                Rectangle& area = dirtyAreas[i];
                canvas.fillRect(area.x, area.y, area.w, area.h, currentTheme.background);
            }
        }
        
        // Render all components
        renderStatusBar();
        renderMenu();
        renderSelectionIndicator();
        
        // Flush canvas to display
        canvas.flush();
        
        clearDirtyAreas();
    }
};
```

### Status Bar Rendering
```cpp
// src/ui/components/StatusBar.cpp - Detailed Rendering
class StatusBar {
private:
    int y = 0;
    int height = 16;
    bool needsRedraw = true;
    
    // Status data
    int batteryLevel = 100;
    bool wifiConnected = false;
    char timeString[9] = "00:00:00";
    
public:
    void draw(Arduino_Canvas& canvas, const Theme& theme) {
        if (!needsRedraw) return;
        
        // Clear status bar area
        canvas.fillRect(0, y, DISPLAY_WIDTH, height, theme.background);
        
        // Draw battery icon
        drawBatteryIcon(5, y + 2, batteryLevel, theme);
        
        // Draw WiFi icon
        drawWiFiIcon(30, y + 2, wifiConnected, theme);
        
        // Draw time
        drawTime(DISPLAY_WIDTH - 60, y + 2, timeString, theme);
        
        needsRedraw = false;
    }
    
private:
    void drawBatteryIcon(int x, int y, int level, const Theme& theme) {
        // Battery outline
        canvas.drawRect(x, y, 12, 6, theme.foreground);
        canvas.drawRect(x + 12, y + 2, 2, 2, theme.foreground);
        
        // Battery level
        int fillWidth = (level * 10) / 100;
        if (fillWidth > 0) {
            canvas.fillRect(x + 1, y + 1, fillWidth, 4, theme.foreground);
        }
    }
    
    void drawWiFiIcon(int x, int y, bool connected, const Theme& theme) {
        uint16_t color = connected ? theme.foreground : theme.secondary;
        
        // Simple WiFi icon (3 arcs)
        for (int i = 0; i < 3; i++) {
            int radius = 3 - i;
            canvas.drawCircle(x + 6, y + 6, radius, color);
        }
    }
    
    void drawTime(int x, int y, const char* time, const Theme& theme) {
        canvas.setTextColor(theme.foreground);
        canvas.setTextSize(1);
        canvas.setCursor(x, y);
        canvas.print(time);
    }
};
```

### Menu Rendering with Selection
```cpp
// src/ui/components/Menu.cpp - Detailed Menu Rendering
class Menu {
public:
    void draw(Arduino_Canvas& canvas, const Theme& theme, int yOffset) {
        int startY = yOffset + 16; // Below status bar
        
        // Calculate visible range
        int startIndex = max(0, selectedIndex - 2);
        int endIndex = min(itemCount, startIndex + visibleItems);
        
        // Draw menu items
        for (int i = startIndex; i < endIndex; i++) {
            int itemY = startY + (i - startIndex) * itemHeight;
            bool isSelected = (i == selectedIndex);
            
            drawMenuItem(canvas, items[i], itemY, isSelected, theme);
        }
        
        // Draw selection indicator
        if (selectionChanged) {
            drawSelectionIndicator(canvas, startY, theme);
        }
    }
    
private:
    void drawMenuItem(Arduino_Canvas& canvas, const MenuItem& item, 
                     int y, bool isSelected, const Theme& theme) {
        // Background for selected item
        if (isSelected) {
            canvas.fillRect(0, y, DISPLAY_WIDTH, itemHeight, theme.accent);
        }
        
        // Selection arrow
        if (isSelected) {
            canvas.setTextColor(theme.background);
            canvas.setTextSize(1);
            canvas.setCursor(2, y + 4);
            canvas.print(">");
        }
        
        // Icon
        if (item.icon) {
            IconRenderer::drawIcon(canvas, *item.icon, 20, y + 1, 
                                 isSelected ? theme.background : theme.foreground);
        }
        
        // Text
        canvas.setTextColor(isSelected ? theme.background : theme.foreground);
        canvas.setTextSize(1);
        canvas.setCursor(40, y + 4);
        canvas.print(item.label);
    }
    
    void drawSelectionIndicator(Arduino_Canvas& canvas, int startY, const Theme& theme) {
        // Animated selection indicator
        unsigned long currentTime = millis();
        bool highlight = ((currentTime - lastSelectionTime) < 200);
        
        if (highlight) {
            int indicatorY = startY + (selectedIndex * itemHeight);
            canvas.drawRect(0, indicatorY, DISPLAY_WIDTH, itemHeight, theme.accent);
        }
    }
};
```

## 🔄 UI Manager (Main Orchestrator)

### Core UI Manager
```cpp
// src/ui/core/UIManager.h
class UIManager {
private:
    Arduino_GFX* display;
    ButtonManager* buttonManager;  // Pointer to hardware ButtonManager
    EventSystem eventSystem;
    DisplayRenderer renderer;
    Screen* currentScreen;
    Theme currentTheme;
    
    // Screen instances
    MainMenuScreen mainMenuScreen;
    AlertsScreen alertsScreen;
    GamesScreen gamesScreen;
    SettingsScreen settingsScreen;
    
    // Performance tracking
    unsigned long lastFrameTime;
    unsigned long frameCount;
    float fps;
    
    // State management
    bool needsRedraw = true;
    unsigned long lastUpdateTime = 0;
    static const unsigned long FRAME_RATE_LIMIT = 33; // ~30 FPS
    
public:
    void begin(Arduino_GFX* display);
    void update();  // Call in main loop
    void setTheme(const Theme& theme);
    void switchScreen(Screen* screen);
    
    // Performance monitoring
    float getFPS() const { return fps; }
    void printStats();
    
private:
    void processEvents();
    void updateScreen();
    void renderFrame();
    void calculateFPS();
};
```

### Complete UI Lifecycle
```cpp
// src/ui/core/UIManager.cpp - Complete Implementation
void UIManager::begin(Arduino_GFX* display, ButtonManager* btnManager) {
    this->display = display;
    this->buttonManager = btnManager;
    
    // Initialize subsystems
    renderer.begin(display);
    
    // Set default theme
    currentTheme = THEME_AMBER; // FlipperZero style
    
    // Initialize main menu as default screen
    currentScreen = &mainMenuScreen;
    currentScreen->setDisplay(display);
    currentScreen->setTheme(currentTheme);
    currentScreen->begin();
    
    // Performance tracking
    lastFrameTime = millis();
    frameCount = 0;
    fps = 0.0f;
}

void UIManager::update() {
    unsigned long currentTime = millis();
    
    // Frame rate limiting
    if ((currentTime - lastUpdateTime) < FRAME_RATE_LIMIT) {
        return;
    }
    lastUpdateTime = currentTime;
    
    // 1. Update button states and generate events
    if (buttonManager) {
        buttonManager->update();
    }
    
    // 2. Process all pending events
    processEvents();
    
    // 3. Update current screen
    updateScreen();
    
    // 4. Render frame if needed
    if (needsRedraw) {
        renderFrame();
        needsRedraw = false;
    }
    
    // 5. Calculate performance metrics
    calculateFPS();
}

## 🔄 Complete Interaction Lifecycle

### User Interaction Flow
```
1. User presses Button B (Up)
   ↓
2. ButtonManager detects physical button press
   ↓
3. Debouncing (50ms delay)
   ↓
4. Button state change confirmed
   ↓
5. EventSystem generates BUTTON_PRESS event
   ↓
6. UIManager processes event in next update cycle
   ↓
7. Event passed to current screen (MainMenuScreen)
   ↓
8. MainMenuScreen handles event in Menu component
   ↓
9. Menu.navigateUp() called
   ↓
10. selectedIndex decremented
   ↓
11. MENU_NAVIGATE event generated
   ↓
12. needsRedraw flag set to true
   ↓
13. Next frame: renderFrame() called
   ↓
14. Menu.draw() renders with new selection
   ↓
15. Canvas flushed to display
   ↓
16. User sees selection moved up
```

### Menu Selection States
```cpp
// Menu selection has multiple visual states:
enum class SelectionState {
    NORMAL,         // Regular menu item
    SELECTED,       // Currently selected item
    HIGHLIGHTED,    // Just selected (brief animation)
    PRESSED         // Button pressed on this item
};

// Visual feedback for each state:
// NORMAL:     Default text color, no background
// SELECTED:   Inverted colors (background = accent, text = background)
// HIGHLIGHTED: Selection arrow ">" + brief border flash
// PRESSED:    Brief color flash + haptic feedback
```

### Button Response Timing
```cpp
// Button response timeline:
// 0ms:    Physical button press
// 50ms:   Debouncing complete
// 51ms:   Event generated
// 66ms:   Next frame (30 FPS = 33ms intervals)
// 67ms:   Event processed
// 68ms:   UI updated
// 99ms:   Frame rendered and displayed
// Total:  ~50ms response time (excellent for embedded UI)
```

### Memory Management During Interaction
```cpp
// Memory usage during typical interaction:
// - Event queue: 16 events × 16 bytes = 256 bytes
// - Button states: 3 buttons × 24 bytes = 72 bytes
// - Menu state: ~64 bytes
// - Canvas buffer: 240×135×2 = 64KB
// - Total UI memory: ~65KB
// - Available ESP32-S3 RAM: 512KB
// - Memory efficiency: ~13% of total RAM
```

void UIManager::processEvents() {
    UIEvent event;
    
    while (eventSystem.popEvent(event)) {
        // Handle system-level events
        switch (event.type) {
            case UIEventType::THEME_CHANGE:
                setTheme(*(Theme*)event.data);
                break;
                
            case UIEventType::SCREEN_CHANGE:
                switchScreen((Screen*)event.data);
                break;
                
            default:
                // Pass event to current screen
                if (currentScreen) {
                    currentScreen->handleInput(event);
                }
                break;
        }
    }
}

void UIManager::updateScreen() {
    if (currentScreen) {
        currentScreen->update();
        
        // Check if screen needs redraw
        if (currentScreen->needsRedraw()) {
            needsRedraw = true;
        }
    }
}

void UIManager::renderFrame() {
    // Clear canvas with theme background
    renderer.clear(currentTheme);
    
    // Render current screen
    if (currentScreen) {
        currentScreen->draw(renderer.getCanvas(), currentTheme);
    }
    
    // Flush to display
    renderer.flush();
    
    frameCount++;
}

void UIManager::calculateFPS() {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - lastFrameTime;
    
    if (elapsed >= 1000) { // Every second
        fps = (float)frameCount * 1000.0f / elapsed;
        frameCount = 0;
        lastFrameTime = currentTime;
    }
}
```

### Main Loop Integration
```cpp
// In main Arduino sketch
#include "src/config/settings.h"
#include "src/ui/core/UIManager.h"

UIManager uiManager;
Arduino_GFX* display;

void setup() {
    // Initialize hardware components
    PowerManager powerManager;
    LED statusLED;
    Buzzer buzzer;
    ButtonManager buttonManager;
    
    powerManager.begin();
    statusLED.begin(LED_PIN);
    buzzer.begin(BUZZER_PIN);
    buttonManager.begin(&powerManager, &statusLED, &buzzer);
    
    // Initialize display using settings.h configuration
    display = new Arduino_GFX_240x135_ST7789();
    display->begin();
    display->setRotation(1);  // Landscape orientation
    
    // Configure display using settings.h constants
    #ifdef USE_ST7789_DISPLAY
        pinMode(TFT_BACKLIGHT, OUTPUT);
        digitalWrite(TFT_BACKLIGHT, HIGH);  // Turn on backlight
    #endif
    
    // Initialize UI with display dimensions from settings.h
    uiManager.begin(display, &buttonManager);
    uiManager.setTheme(THEME_AMBER);  // FlipperZero style (black text on amber background)
}

void loop() {
    uiManager.update();  // Handles all UI updates and input
    
    // Other non-blocking tasks can run here
    // Audio processing, WiFi, etc.
}
```

---

## ⚙️ Configuration Integration

### Settings.h Integration
The UI framework integrates with the centralized configuration system defined in `src/config/settings.h`:

```cpp
// UI components use settings.h constants
#include "src/config/settings.h"

// Display configuration
const int DISPLAY_WIDTH = 240;    // From settings.h
const int DISPLAY_HEIGHT = 135;   // From settings.h

// Button configuration
const int BUTTON_A_PIN = 0;       // From settings.h
const int BUTTON_B_PIN = 1;       // From settings.h  
const int BUTTON_C_PIN = 2;       // From settings.h

// Timing configuration
const unsigned long LONG_PRESS_THRESHOLD_MS = 1000;  // From settings.h
const unsigned long INACTIVITY_TIMEOUT_MS = 60000;   // From settings.h

// Hardware configuration
const int BUZZER_PIN = 15;        // From settings.h
const int LED_PIN = 13;           // From settings.h
```

### Configuration Benefits
- **Centralized Management:** All pin definitions in one place
- **Easy Maintenance:** Change hardware configuration without touching UI code
- **Consistency:** Ensures UI and other systems use same pin assignments
- **Documentation:** Settings.h serves as hardware reference

## 🎯 Performance Optimization

### Rendering Strategy
1. **Dirty Rectangle Tracking:** Only redraw changed areas
2. **Double Buffering:** Use Arduino_Canvas for smooth rendering
3. **Icon Caching:** Pre-render common icons to memory
4. **Frame Rate Limiting:** Cap at 30 FPS to save power

### Memory Management
```cpp
// Use Arduino_Canvas for efficient rendering
// Display dimensions from settings.h
Arduino_Canvas canvas(DISPLAY_WIDTH, DISPLAY_HEIGHT, display);

// Memory usage optimization
// - 240x135x2 bytes = ~64KB for canvas buffer
// - Icon data stored in PROGMEM
// - Theme colors as constants
```

### Canvas Usage
```cpp
// src/ui/renderer/DisplayRenderer.h
class DisplayRenderer {
private:
    Arduino_Canvas canvas;
    
public:
    void begin(Arduino_GFX* display);
    void clear(const Theme& theme);
    void drawStatusBar(const StatusBar& statusBar, const Theme& theme);
    void drawMenu(const Menu& menu, const Theme& theme);
    void flush();  // Copy canvas to display
};
```

---

## 🎨 Visual Design Specifications

### Typography
- **Font:** Built-in Arduino_GFX fonts (6x8, 8x12, 12x16)
- **Primary Text:** 8x12 font for menu items
- **Status Text:** 6x8 font for status bar
- **Headers:** 12x16 font for screen titles

### Layout Grid
```
Display: 240x135 pixels (from settings.h DISPLAY_WIDTH/DISPLAY_HEIGHT)
├── Status Bar: 240x16 (y: 0-15)
├── Content Area: 240x119 (y: 16-134)
└── Menu Items: 18px height each (16px icon + 2px padding)
```

### Icon Integration
Based on [example_icon_usage.ino](example_icon_usage.ino):
- **Icon Size:** 16x16 pixels (standardized)
- **Storage:** PROGMEM arrays (RGB565 format)
- **Rendering:** Direct pixel drawing with theme colors
- **Positioning:** Left-aligned in menu items

---

## 🔧 Implementation Phases

### Phase 1: Core Infrastructure (Week 1)
- [ ] ButtonManager implementation in hardware/ directory with proper debouncing
- [ ] Hardware integration with PowerManager, LED, and Buzzer
- [ ] Theme system and color definitions
- [ ] Basic Screen and UIManager classes
- [ ] DisplayRenderer with Arduino_Canvas

### Phase 2: UI Components (Week 2)
- [ ] StatusBar component
- [ ] Menu component with navigation
- [ ] IconRenderer integration
- [ ] Basic MainMenuScreen

### Phase 3: Screen System (Week 3)
- [ ] Complete screen hierarchy
- [ ] Screen transitions
- [ ] Input handling for all screens
- [ ] Performance optimization

### Phase 4: Polish & Testing (Week 4)
- [ ] Theme switching
- [ ] Performance monitoring
- [ ] Memory optimization
- [ ] User testing and refinement

---

## 📋 Technical Requirements

### Hardware Dependencies
- **Display:** Built-in 240x135 TFT (ST7789 driver) - configured in settings.h
- **Buttons:** 3 built-in buttons (GPIO0, GPIO1, GPIO2) - defined in settings.h
- **Audio:** GPIO15 buzzer pin - defined in settings.h
- **LED:** GPIO13 external LED - defined in settings.h
- **NeoPixel:** Built-in RGB LED (GPIO33) - for status indicators
- **Memory:** ~64KB for canvas buffer + program memory

### Additional Hardware Features
Based on ESP32-S3 Reverse TFT Feather documentation:
- **NeoPixel LED:** Addressable RGB LED for status indicators and bootloader feedback
- **Red LED (#13):** Built-in red LED for operational status
- **Reset Button:** Hardware reset and bootloader entry
- **Deep Sleep Support:** Buttons configured for wake-up from deep sleep
- **Bootloader Entry:** D0/BOOT button + Reset for ROM bootloader access

### Software Dependencies
- **Arduino_GFX Library:** Graphics and display driver
- **Icon System:** Generated 16x16 icon headers
- **Button System:** Custom debouncing and state management
- **Configuration:** Centralized settings.h for all pin definitions and constants

### Performance Targets
- **Frame Rate:** 30 FPS minimum
- **Response Time:** <50ms button response
- **Memory Usage:** <100KB total UI system
- **Boot Time:** <2 seconds to first screen

---

## 🎯 Success Criteria

### Functional Requirements
- ✅ Smooth 30+ FPS navigation
- ✅ Responsive button input with proper debouncing
- ✅ Clean monochrome visual design
- ✅ Configurable color themes
- ✅ Non-blocking operation
- ✅ Memory efficient implementation

### User Experience
- ✅ Intuitive FlipperZero-style navigation
- ✅ Immediate visual feedback
- ✅ Consistent visual language
- ✅ Fast screen transitions
- ✅ Clear status information

### Technical Quality
- ✅ Clean, maintainable code structure
- ✅ Proper separation of concerns
- ✅ Efficient memory usage
- ✅ Comprehensive error handling
- ✅ Easy theme customization

---

## 📚 References

- [Arduino_GFX Library](https://github.com/moononournation/Arduino_GFX)
- [Arduino_GFX Canvas Class](https://github.com/moononournation/Arduino_GFX/wiki/Canvas-Class)
- [Arduino_GFX Display Class](https://github.com/moononournation/Arduino_GFX/wiki/Display-Class)
- [ESP32-S3 Reverse TFT Feather Pinouts](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/pinouts)
- [Hardware Setup Guide](hardware-setup.md)
- [Pinout Reference](pinout-reference.md)
- [Icon Usage Example](example_icon_usage.ino)
- [Project Settings](src/config/settings.h)

---

*This plan provides a comprehensive roadmap for implementing a professional-grade UI system that meets all specified requirements while maintaining simplicity and performance.*
