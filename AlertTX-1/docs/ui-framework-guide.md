# UI Framework Guide

The AlertTX-1 features a modern component-based UI framework built on top of the Adafruit GFX library. This guide covers the architecture, components, and how to build screens and interfaces.

## 🏗️ Architecture Overview

The UI framework follows a hierarchical component-based design:

```
Component (base class)
├── MenuItem (clickable menu items)
└── MenuContainer (manages collections of MenuItems)

Screen (base class) 
├── MainMenuScreen (main navigation)
├── AlertsScreen (alert information)
├── GamesScreen (game library)
├── SettingsScreen (device settings)
└── SplashScreen (boot screen)

ScreenManager (navigation controller)
├── Screen stack management
├── Input routing
└── Transition handling
```

## 🎨 Theme System

The framework includes a powerful theming system with four built-in themes:

### Available Themes
- **Default** - FlipperZero-inspired (black/white/yellow)
- **Terminal** - Retro green terminal style
- **Amber** - Classic amber CRT monitor
- **High Contrast** - Maximum visibility (pure black/white)

### Using Themes
```cpp
#include "src/ui/core/Theme.h"

// Switch themes at runtime
ThemeManager::setTheme(&THEME_TERMINAL);   // Green terminal
ThemeManager::setTheme(&THEME_AMBER);      // Amber CRT
ThemeManager::setTheme(&THEME_HIGH_CONTRAST); // High contrast
ThemeManager::setTheme(&THEME_DEFAULT);    // Back to default

// All UI elements automatically update
```

### Custom Themes
```cpp
const Theme THEME_CUSTOM PROGMEM = {
    .background = ST77XX_BLUE,
    .surfaceBackground = ST77XX_BLUE,
    .primaryText = ST77XX_WHITE,
    .selectedText = ST77XX_BLUE,
    .accent = ST77XX_WHITE,
    // ... other colors
};

ThemeManager::setTheme(&THEME_CUSTOM);
```

## 🧩 Components

### Component Base Class

All UI elements inherit from the `Component` base class:

```cpp
class Component {
public:
    // Layout management
    void setBounds(int x, int y, int w, int h);
    void setPosition(int x, int y);
    void setSize(int w, int h);
    
    // Theme integration
    uint16_t getThemeColor(const char* colorType);
    
    // Dirty tracking for efficiency
    void markDirty();
    bool isDirty();
    
    // Rendering
    virtual void draw() = 0;
    virtual void update() {}
};
```

### MenuItem Component

Individual menu items with clickable behavior:

```cpp
class MenuItem : public Component, public Clickable {
public:
    MenuItem(Adafruit_ST7789* display, const char* label, int id);
    
    // Callback setup
    void setOnSelect(std::function<void()> callback);
    void setOnSelect(void (*callback)());  // C-style support
    
    // State management
    void setSelected(bool isSelected);
    void setLabel(const char* newLabel);
    
    // Interaction
    void onClick() override;
    void onPress() override;
};
```

### MenuContainer Component

Manages collections of MenuItem components:

```cpp
class MenuContainer : public Component {
public:
    MenuContainer(Adafruit_ST7789* display, int x, int y);
    
    // Item management
    bool addMenuItem(const char* label, int id, std::function<void()> callback);
    bool addMenuItem(MenuItem* item);
    
    // Navigation
    void moveUp();
    void moveDown();
    void selectCurrent();
    
    // Layout
    void autoLayout();
    void setVisibleItemCount(int count);
};
```

## 📱 Screen System

### Screen Base Class

Screens manage collections of components and handle input:

```cpp
class Screen {
public:
    Screen(Adafruit_ST7789* display, const char* name);
    
    // Lifecycle
    virtual void enter() { active = true; }   
    virtual void exit() { active = false; }   
    virtual void update();                     
    virtual void draw();                       
    
    // Input handling
    virtual void handleButtonPress(int button) = 0;
    
    // Component management
    void addComponent(Component* component);
    void removeComponent(Component* component);
};
```

### Creating Custom Screens

```cpp
class CustomScreen : public Screen {
private:
    MenuContainer* menu;
    
public:
    CustomScreen(Adafruit_ST7789* display) 
        : Screen(display, "CustomScreen") {
        
        // Create menu component
        menu = new MenuContainer(display, 10, 50);
        addComponent(menu);
        
        // Setup menu items
        menu->addMenuItem("Option 1", 1, []() { 
            Serial.println("Option 1 selected"); 
        });
        menu->addMenuItem("Option 2", 2, []() { 
            Serial.println("Option 2 selected"); 
        });
    }
    
    void handleButtonPress(int button) override {
        switch(button) {
            case 0: // Button A - Up
                menu->moveUp();
                break;
            case 1: // Button B - Down
                menu->moveDown();
                break;
            case 2: // Button C - Select
                menu->selectCurrent();
                break;
        }
    }
};
```

## 🔄 Screen Navigation

### ScreenManager

The ScreenManager handles navigation between screens:

```cpp
class ScreenManager {
public:
    // Navigation
    bool pushScreen(Screen* screen);      // Navigate to new screen
    bool popScreen();                     // Go back
    bool switchToScreen(Screen* screen);  // Replace current
    
    // Input routing
    void handleButtonPress(int button);
    
    // Rendering
    void update();  // ~60 FPS updates
    void draw();    // Efficient rendering
};
```

### Global Navigation

Access navigation from anywhere in your code:

```cpp
#include "src/ui/core/ScreenManager.h"

// Navigate to a new screen
ScreenManager* manager = GlobalScreenManager::getInstance();
manager->pushScreen(new SettingsScreen(display));

// Go back to previous screen
manager->popScreen();

// Replace current screen
manager->switchToScreen(new MainMenuScreen(display));
```

### Navigation Patterns

```cpp
// In a menu callback
void onSettingsSelected() {
    ScreenManager* manager = GlobalScreenManager::getInstance();
    manager->pushScreen(new SettingsScreen(display));
}

// Back navigation (handled globally in main loop)
if (buttonManager.isLongPressed(ButtonManager::BUTTON_A) || 
    buttonManager.isLongPressed(ButtonManager::BUTTON_B) || 
    buttonManager.isLongPressed(ButtonManager::BUTTON_C)) {
    screenManager->popScreen();
}
```

## ⚡ Best Practices

### Memory Management
- Use fixed arrays instead of dynamic allocation
- Components are limited: 10 per screen, 8 menu items max
- Screen stack supports 8 levels of navigation
- Clean up resources in destructors

### Performance Optimization
- Use dirty tracking: only redraw components that changed
- Call `markDirty()` when component state changes
- Framework maintains 60 FPS updates with 30+ FPS rendering
- Input debouncing is handled automatically

### Theme Integration
```cpp
// Always use theme colors
uint16_t textColor = getThemeColor("primaryText");
uint16_t bgColor = getThemeColor("background");

// Components automatically respect themes
// Manual drawing should use theme colors
```

### Layout Guidelines
```cpp
// Use DisplayConfig constants for positioning
#include "src/config/DisplayConfig.h"

// Center text horizontally
int centerX = (SCREEN_WIDTH - textWidth) / 2;

// Standard menu positioning
MenuContainer* menu = new MenuContainer(display, 10, 50);
```

## 🔧 Integration Example

Here's a complete example integrating all framework features:

```cpp
#include "src/ui/core/ScreenManager.h"
#include "src/ui/core/Theme.h"
#include "src/ui/components/MenuContainer.h"

class MyApp {
private:
    Adafruit_ST7789* display;
    ScreenManager* screenManager;
    
public:
    void setup() {
        // Initialize theme system
        ThemeManager::begin();
        ThemeManager::setTheme(&THEME_DEFAULT);
        
        // Setup screen manager
        screenManager = GlobalScreenManager::getInstance();
        screenManager->initialize(display);
        
        // Create and show main screen
        MainMenuScreen* mainScreen = new MainMenuScreen(display);
        screenManager->switchToScreen(mainScreen);
    }
    
    void loop() {
        // Update and draw
        screenManager->update();
        screenManager->draw();
        
        // Handle input (example)
        if (buttonPressed) {
            screenManager->handleButtonPress(buttonId);
        }
    }
};
```

## 📊 System Capabilities

### Memory Usage
- **Component Framework**: ~4KB RAM
- **Theme System**: ~128 bytes flash storage
- **Screen Stack**: ~2KB for navigation
- **Total Overhead**: <8KB for full framework

### Performance Metrics
- **Frame Rate**: 60 FPS updates, 30+ FPS rendering
- **Response Time**: <50ms button to visual feedback
- **Memory Efficiency**: Fixed allocation, no dynamic memory
- **Boot Time**: <2 seconds to main interface

## 🔍 Debugging

The framework includes comprehensive validation and debugging tools:

```cpp
// Component validation
component->validate();           // Check component integrity
screen->validate();             // Validate all screen components
screenManager->validate();      // Check navigation state

// Performance monitoring
screenManager->printPerformanceStats();
screenManager->printStackState();
component->printBounds();
```

## 📚 Advanced Topics

### Clickable Interface

Add custom click behavior to any component:

```cpp
template<typename BaseComponent>
class ClickableComponent : public BaseComponent, public Clickable {
public:
    void setOnClick(std::function<void()> callback) {
        clickCallback = callback;
    }
    
    void onClick() override {
        if (clickCallback) clickCallback();
    }
    
private:
    std::function<void()> clickCallback;
};
```

### Custom Components

Create your own components by inheriting from the base class:

```cpp
class ProgressBar : public Component {
private:
    int progress = 0;  // 0-100
    
public:
    ProgressBar(Adafruit_ST7789* display, int x, int y, int w, int h) 
        : Component(display) {
        setBounds(x, y, w, h);
    }
    
    void draw() override {
        uint16_t bgColor = getThemeColor("background");
        uint16_t fillColor = getThemeColor("accent");
        
        // Draw progress bar
        display->fillRect(x, y, width, height, bgColor);
        int fillWidth = (width * progress) / 100;
        display->fillRect(x, y, fillWidth, height, fillColor);
        
        clearDirty();
    }
    
    void setProgress(int newProgress) {
        if (newProgress != progress) {
            progress = newProgress;
            markDirty();
        }
    }
};
```

The UI framework provides a solid foundation for building sophisticated interfaces while maintaining the simplicity and reliability that makes embedded development successful.