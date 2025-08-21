# Navigation System

The AlertTX-1 uses a stack-based navigation system with screens managed by a central ScreenManager. This guide covers the navigation architecture and how to implement navigation in your screens.

## 🏗️ Architecture Overview

### Core Components

#### ScreenManager
Central navigation controller that manages a stack of screens:
- **Push/Pop Navigation**: Navigate forward and backward through screens
- **Memory Management**: Handles screen lifecycle and cleanup
- **Singleton Pattern**: Global access via `GlobalScreenManager`

#### Screen Base Class
All screens inherit from the `Screen` class:
- **Lifecycle Methods**: `enter()`, `exit()`, `cleanup()`
- **Update/Draw Loop**: `update()` and `draw()` methods
- **Component Management**: Built-in component system support

#### InputRouter
Centralized input handling:
- **Button Detection**: Handles all three buttons (A, B, C)
- **Long Press**: Detects 1500ms press for back navigation
- **Auto-repeat**: Holding A/B repeats navigation

## 📱 Navigation Flow

```
SplashScreen (2s auto-transition)
    ↓
MainMenuScreen
    ├── AlertsScreen
    ├── GamesScreen
    │   ├── BeeperHero
    │   ├── Pong
    │   └── Snake
    └── SettingsScreen
        ├── Themes → ThemeSelectionScreen
        ├── Ringtones → RingtoneSelectionScreen
        └── System Info
```

## 🎮 Button Controls

### Standard Navigation Pattern
- **Button A** (GPIO0): Navigate Up/Previous
- **Button B** (GPIO1): Navigate Down/Next
- **Button C** (GPIO2): Select/Enter
- **Long Press Any** (1500ms): Back to previous screen

## 💻 Implementation Guide

### Creating a Navigable Screen

```cpp
// MyScreen.h
#pragma once
#include "ui/core/Screen.h"

class MyScreen : public Screen {
public:
    MyScreen(Adafruit_ST7789* display);
    ~MyScreen();
    
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    void handleButtonPress(int button) override;
    
protected:
    void cleanup() override;
};
```

### Implementing Navigation

#### Forward Navigation (Push)
```cpp
void MainMenuScreen::onSettingsSelected() {
    // Get the global ScreenManager instance
    ScreenManager* manager = GlobalScreenManager::getInstance();
    
    // Push new screen onto stack
    manager->pushScreen(settingsScreen);
}
```

#### Back Navigation (Pop)
Back navigation is handled globally in the main loop:
```cpp
// In AlertTX-1.ino
if (buttonManager.isLongPressed(ButtonManager::BUTTON_A) || 
    buttonManager.isLongPressed(ButtonManager::BUTTON_B) || 
    buttonManager.isLongPressed(ButtonManager::BUTTON_C)) {
    screenManager->popScreen();
}
```

### Managing Child Screens

```cpp
class MainMenuScreen : public Screen {
private:
    // Child screen instances
    AlertsScreen* alertsScreen = nullptr;
    GamesScreen* gamesScreen = nullptr;
    SettingsScreen* settingsScreen = nullptr;
    
    void initializeScreens() {
        // Create child screens
        alertsScreen = new AlertsScreen(display);
        gamesScreen = new GamesScreen(display);
        settingsScreen = new SettingsScreen(display);
    }
    
    void cleanup() override {
        // Clean up child screens
        if (alertsScreen) {
            delete alertsScreen;
            alertsScreen = nullptr;
        }
        // ... cleanup other screens
    }
};
```

## 🔧 Advanced Features

### Screen Ownership
ScreenManager tracks screen ownership for proper memory management:

```cpp
// Push screen with ownership transfer
screenManager->pushScreen(myScreen, true);  // ScreenManager will delete

// Push screen without ownership
screenManager->pushScreen(sharedScreen, false);  // Caller manages lifetime
```

### Custom Navigation Patterns

#### Modal Screens
```cpp
// Push modal screen that captures all input
class ModalScreen : public Screen {
    void handleButtonPress(int button) override {
        if (button == BUTTON_C) {
            // Handle confirmation
            screenManager->popScreen();
        }
        // No long-press back for modals
    }
};
```

#### Multi-Level Menus
```cpp
class MenuScreen : public Screen {
private:
    MenuContainer* currentMenu;
    std::vector<MenuContainer*> menuStack;
    
    void navigateToSubmenu(MenuContainer* submenu) {
        menuStack.push_back(currentMenu);
        currentMenu = submenu;
        markForFullRedraw();
    }
    
    void navigateBack() {
        if (!menuStack.empty()) {
            currentMenu = menuStack.back();
            menuStack.pop_back();
            markForFullRedraw();
        } else {
            screenManager->popScreen();
        }
    }
};
```

## 📊 Memory Management

### Screen Lifecycle
1. **Creation**: Screen allocated when parent screen enters
2. **Enter**: `enter()` called when screen becomes active
3. **Active**: `update()` and `draw()` called each frame
4. **Exit**: `exit()` called when navigating away
5. **Cleanup**: `cleanup()` called to free resources
6. **Deletion**: Screen deleted if owned by ScreenManager

### Best Practices
- Create child screens in parent's `enter()` method
- Delete child screens in `cleanup()` method
- Use ownership flags appropriately
- Avoid circular references between screens

## 🎯 Navigation Patterns

### Hub and Spoke
Main menu acts as central hub with direct navigation to features:
```
MainMenu → Feature → MainMenu
```

### Hierarchical
Nested menus with multiple levels:
```
Settings → Display → Brightness → Settings
```

### Wizard/Flow
Sequential screens for multi-step processes:
```
Setup → WiFi → MQTT → Complete
```

## 🐛 Common Issues

### Memory Leaks
```cpp
// ❌ Wrong - leaks memory
void MyScreen::enter() {
    childScreen = new ChildScreen(display);
}

// ✅ Correct - proper cleanup
void MyScreen::cleanup() override {
    if (childScreen) {
        delete childScreen;
        childScreen = nullptr;
    }
}
```

### Navigation Loops
```cpp
// ❌ Wrong - creates loop
void ScreenA::onSelect() {
    screenManager->pushScreen(screenB);
}
void ScreenB::onSelect() {
    screenManager->pushScreen(screenA);  // Loop!
}

// ✅ Correct - use pop to return
void ScreenB::onSelect() {
    screenManager->popScreen();  // Return to A
}
```

## 📚 Related Documentation

- [UI Framework](ui-framework.md) - Component system overview
- [Screen Development](screen-development.md) - Creating custom screens
- [Input Handling](input-handling.md) - Button input details

---

**Last Updated**: December 2024
