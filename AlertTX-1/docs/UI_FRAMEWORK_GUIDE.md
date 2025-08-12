# Alert TX-1 UI Framework Guide (Legacy Reference)

## Overview

⚠️ **LEGACY DOCUMENTATION** - This document describes the Phase 1 simple UI system. 

**Current Status:** The Alert TX-1 now uses a modern **Phase 2 Component-Based Architecture**. 
👉 **See [Phase 2 Implementation Complete](PHASE2_IMPLEMENTATION_COMPLETE.md) for current documentation.**

This document is preserved for reference and understanding the evolution from simple to component-based architecture.

---

## 🏗️ Architecture

### Simple Design Philosophy
- **No Complex Wrappers** - Direct use of Adafruit_ST7789 and existing hardware
- **Proven Foundation** - Built on working display initialization and ButtonManager
- **Minimal Components** - Only what's needed for navigation and display

### File Structure
```
AlertTX-1/src/
├── ui/
│   ├── Menu.h              # Simple navigable menu component
│   └── Menu.cpp
├── hardware/               # Existing proven components
│   ├── ButtonManager.h/cpp # Button handling with debouncing
│   ├── LED.h/cpp          # Status LED control
│   └── Buzzer.h/cpp       # Audio feedback
└── config/
    └── settings.h         # Hardware pin definitions
```

---

## 🎮 Hardware Setup

### Display Configuration (ESP32-S3 Reverse TFT Feather)
```cpp
// CRITICAL: Follow this exact initialization sequence
pinMode(TFT_I2C_POWER, OUTPUT);
digitalWrite(TFT_I2C_POWER, HIGH);  // REQUIRED: Enable power first
delay(10);

pinMode(TFT_BACKLITE, OUTPUT);
digitalWrite(TFT_BACKLITE, HIGH);

tft.init(135, 240);    // Portrait dimensions
tft.setRotation(3);    // Rotate to landscape (240x135)
```

⚠️ **Critical:** `TFT_I2C_POWER` must be enabled before display init or it won't work!

### Button Configuration
- **Button A (GPIO0)** - D0/BOOT button, pulled HIGH, active LOW
- **Button B (GPIO1)** - D1 button, pulled LOW, active HIGH  
- **Button C (GPIO2)** - D2 button, pulled LOW, active HIGH

### Navigation Mapping
- **Button A** = Up (navigate up in menu)
- **Button B** = Down (navigate down in menu)
- **Button C** = Select (activate current menu item)

---

## 📋 Quick Start

### 1. Basic Setup
```cpp
#include "src/ui/Menu.h"
#include "src/hardware/ButtonManager.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Create components
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Menu mainMenu(&tft);
ButtonManager buttonManager;

// Define menu actions
void alertsAction() { /* Handle alerts */ }
void gamesAction() { /* Handle games */ }
void settingsAction() { /* Handle settings */ }

// Define menu items
MenuItem menuItems[] = {
  {"Alerts", 1, alertsAction},
  {"Games", 2, gamesAction}, 
  {"Settings", 3, settingsAction}
};

void setup() {
  // Initialize display (CRITICAL sequence)
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);
  
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  
  tft.init(135, 240);
  tft.setRotation(3);
  
  // Setup UI components
  buttonManager.begin();
  mainMenu.setItems(menuItems, 3);
  
  // Initial draw
  drawUI();
}
```

### 2. Main Loop
```cpp
void loop() {
  buttonManager.update();
  
  // Navigation
  if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {  // Up
    mainMenu.moveUp();
    drawUI();
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_B)) {  // Down
    mainMenu.moveDown();
    drawUI();
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_C)) {  // Select
    mainMenu.select();  // Calls the action callback
  }
}
```

### 3. UI Drawing
```cpp
void drawUI() {
  tft.fillScreen(ST77XX_BLACK);
  
  // Title
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 20);
  tft.println("AlertTX-1");
  
  // Menu
  mainMenu.draw();
}
```

---

## 🎨 Visual Design

### FlipperZero Style
- **Selection Indicator:** ">" character at left margin
- **Highlight:** Yellow rectangle behind selected item
- **Colors:** White text on black background
- **Layout:** Clean text-based navigation

### Menu Layout
```
┌─────────────────────────────────────┐
│  AlertTX-1                          │
├─────────────────────────────────────┤
│ ▶ Alerts                           │ ← Selected (yellow background)
│   Games                             │
│   Settings                          │
│   ...                               │
└─────────────────────────────────────┘
```

---

## 🔧 Menu Component API

### MenuItem Structure
```cpp
struct MenuItem {
    const char* label;      // Display text
    int id;                 // Unique identifier
    void (*action)();       // Callback function (can be nullptr)
};
```

### Menu Class Methods
```cpp
class Menu {
public:
    // Setup
    void setItems(MenuItem* items, int count);
    
    // Navigation
    void moveUp();          // Move selection up
    void moveDown();        // Move selection down
    void select();          // Activate current item
    
    // Drawing
    void draw();            // Render menu to display
    
    // State
    int getSelectedIndex() const;
    void setSelectedIndex(int index);
};
```

---

## ⚡ Performance

### Current Specifications
- **Memory Usage:** ~972KB program, ~64KB RAM (19% of available)
- **Response Time:** Immediate button feedback
- **Display:** Smooth navigation with proper debouncing
- **Efficiency:** No blocking operations, optimized rendering

### Button Debouncing
- **Debounce Time:** 50ms (configured in ButtonManager)
- **Long Press:** 1000ms threshold
- **Reliability:** Proven hardware integration

---

## 🔌 Hardware Integration

The UI framework integrates seamlessly with existing hardware components:

### ButtonManager Integration
```cpp
// Proven button handling with proper debouncing
buttonManager.begin();
buttonManager.update();  // Call in main loop

// Check button states
if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {
    // Handle button A press
}
```

### LED and Buzzer Support
```cpp
// Optional feedback integration
#include "src/hardware/LED.h"
#include "src/hardware/Buzzer.h"

LED statusLED;
Buzzer buzzer;

void setup() {
    statusLED.begin();
    buzzer.begin();
}

// Provide feedback on menu navigation
void onMenuChange() {
    statusLED.blink(100);        // Brief flash
    buzzer.playTone(800, 50);    // Short beep
}
```

---

## 📱 Adding Menu Actions

### Simple Actions
```cpp
void alertsAction() {
    // Clear screen and show alerts interface
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10, 50);
    tft.println("Alerts Screen");
    
    // Wait for button to return
    while (!buttonManager.wasPressed(ButtonManager::BUTTON_C)) {
        buttonManager.update();
        delay(10);
    }
    
    // Redraw main menu
    drawUI();
}
```

### Complex Actions
```cpp
void settingsAction() {
    // Create sub-menu for settings
    MenuItem settingsItems[] = {
        {"WiFi Config", 1, wifiConfigAction},
        {"Display", 2, displaySettingsAction},
        {"Back", 3, nullptr}  // Back to main menu
    };
    
    Menu settingsMenu(&tft);
    settingsMenu.setItems(settingsItems, 3);
    
    // Settings navigation loop
    bool inSettings = true;
    drawSettingsUI(settingsMenu);
    
    while (inSettings) {
        buttonManager.update();
        
        if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {
            settingsMenu.moveUp();
            drawSettingsUI(settingsMenu);
        }
        
        if (buttonManager.wasPressed(ButtonManager::BUTTON_B)) {
            settingsMenu.moveDown();
            drawSettingsUI(settingsMenu);
        }
        
        if (buttonManager.wasPressed(ButtonManager::BUTTON_C)) {
            int selected = settingsMenu.getSelectedIndex();
            if (selected == 2) {  // Back
                inSettings = false;
            } else {
                settingsMenu.select();
            }
        }
    }
    
    // Return to main menu
    drawUI();
}
```

---

## 🎯 Best Practices

### 1. Keep It Simple
- Use direct Adafruit_ST7789 calls for drawing
- Leverage existing ButtonManager for input
- Avoid complex state management

### 2. Responsive Design
- Always call `buttonManager.update()` in loops
- Redraw UI immediately after navigation changes
- Use non-blocking operations

### 3. Memory Efficiency
- Use `const char*` for menu labels
- Store large data in PROGMEM when possible
- Clean up resources in action functions

### 4. User Experience
- Provide immediate visual feedback
- Use consistent navigation patterns
- Keep menu hierarchy shallow

---

## 🔧 Configuration

### Pin Definitions (settings.h)
```cpp
// Display pins (predefined by board)
#define TFT_CS        7
#define TFT_RST       40 
#define TFT_DC        39
// TFT_I2C_POWER and TFT_BACKLITE are board-specific

// Button pins
#define BUTTON_A_PIN  0   // D0/BOOT
#define BUTTON_B_PIN  1   // D1
#define BUTTON_C_PIN  2   // D2

// Hardware feedback
#define LED_PIN       13  // Built-in red LED
#define BUZZER_PIN    15  // External buzzer
```

### Timing Configuration
```cpp
#define DEBOUNCE_DELAY_MS     50    // Button debouncing
#define LONG_PRESS_DELAY_MS   1000  // Long press threshold
```

---

## 📚 Example Projects

### Complete Working Example
See `AlertTX-1.ino` for a full implementation with:
- Three-item menu (Alerts, Games, Settings)
- Working button navigation
- Proper display initialization
- Menu action callbacks

### Custom Menu Example
```cpp
// Create a custom menu for your application
MenuItem gameMenu[] = {
    {"Snake", 1, playSnake},
    {"Pong", 2, playPong},
    {"Back", 3, returnToMain}
};

Menu games(&tft);
games.setItems(gameMenu, 3);

void showGamesMenu() {
    bool inGames = true;
    drawGamesUI();
    
    while (inGames) {
        buttonManager.update();
        // Handle navigation...
    }
}
```

---

## 🚀 Ready to Use

The UI framework is complete and functional. The main sketch demonstrates all features:

1. **Navigation** - Smooth up/down movement with visual feedback
2. **Selection** - Action callbacks work correctly
3. **Integration** - Seamless hardware component usage
4. **Performance** - Responsive and efficient operation

Start with the provided example and customize the menu items and actions for your specific needs!

---

*This guide covers the current working implementation. The system is deliberately simple and proven to work reliably.*
