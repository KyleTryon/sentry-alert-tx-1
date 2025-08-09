# UI Framework Integration Guide

## Overview

The Alert TX-1 project now features a comprehensive React-like UI framework designed specifically for Arduino/ESP32 applications. This framework provides a modern, component-based approach to building user interfaces on embedded displays while maintaining excellent performance and memory efficiency.

## Framework Architecture

### Core Principles

1. **Component-Based Architecture** - Every UI element is a reusable component
2. **Declarative Rendering** - Components declare how they should look given their state  
3. **State Management** - Automatic dirty tracking and efficient re-rendering
4. **Non-Blocking Operation** - UI rendering works alongside audio, networking, and other tasks
5. **Hardware Integration** - Seamless integration with buttons, displays, and other peripherals

### Framework Structure

```
src/ui/
├── core/                    # Core framework classes
│   ├── UIComponent.h        # Base component interface
│   ├── StatefulComponent.h  # State management and dirty tracking
│   ├── Screen.h             # Full-screen container
│   └── UIManager.h          # Navigation and rendering coordination
├── renderer/                # Display utilities
│   └── Renderer.h           # High-level drawing functions
├── components/              # Reusable UI components
│   ├── Label.h              # Text display component
│   ├── Button.h             # Interactive button component
│   ├── ProgressBar.h        # Progress indicator component
│   └── Menu.h               # Scrollable menu component
├── screens/                 # Pre-built screens
│   ├── HomeScreen.h         # Main device screen
│   └── RingtonesScreen.h    # Ringtone selection screen
└── UIFramework.h            # Convenience header
```

## Core Classes

### UIComponent

Base class for all UI elements providing fundamental functionality:

```cpp
class UIComponent {
public:
    virtual void render(Adafruit_GFX& gfx) = 0;  // Draw the component
    virtual void update() {}                      // Update component state
    virtual void handleEvent(int eventType, int eventData = 0) {}  // Handle input
    virtual bool handleTouch(int touchX, int touchY) { return false; }  // Touch handling
    
    // Position and visibility
    virtual void setPosition(int x, int y);
    virtual void setVisible(bool visible);
    virtual bool contains(int x, int y) const;  // Hit testing
};
```

### StatefulComponent

Extends UIComponent with state management and dirty tracking:

```cpp
class StatefulComponent : public UIComponent {
public:
    // State management with automatic dirty marking
    void setState(std::function<void()> mutator);
    
    // Dirty tracking for efficient rendering
    bool isDirty() override;
    void clearDirty() override;
    void markDirty();
};
```

### Screen

Container for full-screen views with component management:

```cpp
class Screen : public StatefulComponent {
public:
    virtual void build() = 0;  // Create child components
    virtual void onActivate();  // Called when screen becomes active
    virtual void onDeactivate();  // Called when screen becomes inactive
    
protected:
    template<typename T, typename... Args>
    T* addComponent(Args&&... args);  // Add child component
};
```

### UIManager

Central manager for navigation and rendering:

```cpp
class UIManager {
public:
    void begin();                    // Initialize the system
    void update();                   // Update active screen
    void render(Adafruit_GFX& gfx);  // Render if needed (30 FPS limit)
    
    void setScreen(Screen* screen);  // Navigate to screen
    void handleEvent(int eventType, int eventData = 0);  // Handle input events
    
    template<typename T, typename... Args>
    T* registerScreen(Args&&... args);  // Register a screen
};
```

## Available Components

### Label

Text display component with flexible formatting:

```cpp
Label* label = addComponent<Label>(10, 10, 100, 20, "Hello World", COLOR_WHITE);
label->setTextSize(2);
label->setCenterAlign(true);
label->setBackgroundColor(COLOR_BLUE, true);
```

### Button

Interactive button with callbacks and visual feedback:

```cpp
Button* button = addComponent<Button>(10, 50, 80, 30, "Click Me", [this]() {
    Serial.println("Button clicked!");
});
button->setColors(COLOR_DARK_GRAY, COLOR_GRAY, COLOR_WHITE);
button->setHighlighted(true);
```

### ProgressBar

Progress indicator with customizable appearance:

```cpp
ProgressBar* progress = addComponent<ProgressBar>(10, 90, 200, 15, 0.75f);
progress->setColors(COLOR_GREEN, COLOR_DARK_GRAY);
progress->setShowPercentage(true);
```

### Menu

Scrollable menu with selection handling:

```cpp
Menu* menu = addComponent<Menu>(10, 10, 220, 100);
menu->addItem("Option 1", [this]() { selectOption1(); });
menu->addItem("Option 2", [this]() { selectOption2(); });
menu->setColors(COLOR_WHITE, COLOR_BLUE);
```

## Integration with Alert TX-1

### Hardware Integration

The UI framework seamlessly integrates with Alert TX-1 hardware:

```cpp
// In AlertTX-1.ino
UIManager uiManager;
HomeScreen* homeScreen = nullptr;
RingtonesScreen* ringtonesScreen = nullptr;

void setup() {
    // Initialize hardware first
    initializeDisplay();
    initializeHardware();
    
    // Initialize UI framework
    UIFramework::initialize();
    uiManager.begin();
    
    // Create screens with hardware references
    homeScreen = uiManager.registerScreen<HomeScreen>(&buttonManager, &ringtonePlayer);
    ringtonesScreen = uiManager.registerScreen<RingtonesScreen>(&ringtonePlayer);
    
    // Set initial screen
    uiManager.setScreen(homeScreen);
}

void loop() {
    // Critical updates every cycle
    updateButtons();
    updateRingtonePlayer();
    updateUI();
    
    // Regular updates (10Hz)
    if (now - lastHeartbeat > 100) {
        updatePowerManagement();
        updateMQTT();
        updateGame();
    }
}
```

### Button Mapping

Physical buttons map to UI events:

```cpp
void updateButtons() {
    buttonManager.update();
    
    // Map hardware buttons to UI events
    if (buttonManager.isPressed(BUTTON_A_PIN)) {
        uiManager.handleEvent(1, 0); // Button A
    }
    if (buttonManager.isPressed(BUTTON_B_PIN)) {
        uiManager.handleEvent(2, 0); // Button B
    }
    if (buttonManager.isPressed(BUTTON_C_PIN)) {
        uiManager.handleEvent(3, 0); // Button C
    }
}
```

### Screen Examples

#### HomeScreen

Main interface showing device status and navigation:

```cpp
class HomeScreen : public Screen {
    void build() override {
        // Title with background
        titleLabel = addComponent<Label>(10, 10, 220, 20, "Alert TX-1", COLOR_WHITE);
        titleLabel->setBackgroundColor(COLOR_BLUE, true);
        titleLabel->setCenterAlign(true);
        
        // Battery indicator
        batteryBar = addComponent<ProgressBar>(170, 35, 60, 10, 0.85f);
        batteryBar->setColors(COLOR_GREEN);
        
        // Navigation buttons
        ringtonesButton = addComponent<Button>(10, 55, 70, 25, "Ringtones", [this]() {
            navigateToRingtones();
        });
        
        // Quick access menu
        quickMenu = addComponent<Menu>(10, 90, 220, 40);
        quickMenu->addItem("Play Last Ringtone", [this]() { playLastRingtone(); });
        quickMenu->addItem("Start BeeperHero", [this]() { startGame(); });
    }
};
```

#### RingtonesScreen

Ringtone selection and playback interface:

```cpp
class RingtonesScreen : public Screen {
    void build() override {
        // Title
        titleLabel = addComponent<Label>(10, 5, 220, 15, "Ringtones", COLOR_WHITE);
        
        // Ringtone menu populated from ringtone_data.h
        ringtoneMenu = addComponent<Menu>(10, 40, 220, 65);
        populateRingtoneMenu();
        
        // Control buttons
        playButton = addComponent<Button>(10, 110, 50, 20, "Play", [this]() {
            playSelectedRingtone();
        });
        stopButton = addComponent<Button>(70, 110, 50, 20, "Stop", [this]() {
            stopPlayback();
        });
    }
};
```

## Performance Characteristics

### Memory Usage

- **UIComponent**: ~16 bytes base overhead
- **StatefulComponent**: +8 bytes for state tracking
- **Screen**: +32 bytes for component management
- **Label**: ~24 bytes + text content
- **Button**: ~48 bytes + label text + callbacks
- **Menu**: ~64 bytes + item storage

### Rendering Performance

- **Frame Rate**: Capped at 30 FPS (33ms intervals)
- **Dirty Tracking**: Only redraws when components change
- **Partial Updates**: Component-level dirty tracking
- **Non-Blocking**: Rendering yields to other tasks

### Typical Performance

On ESP32-S3 @ 240MHz:
- HomeScreen render: ~2-5ms
- Button interaction: <1ms
- Menu navigation: ~1-3ms
- Memory usage: ~2-4KB for typical screen

## Event System

### Event Types

The framework uses a simple integer-based event system:

```cpp
// Standard event types
#define EVENT_BUTTON_A    1
#define EVENT_BUTTON_B    2  
#define EVENT_BUTTON_C    3
#define EVENT_NAVIGATE    4
#define EVENT_SELECT      5

// Component-specific events
#define EVENT_BUTTON_PRESS    10
#define EVENT_BUTTON_RELEASE  11
#define EVENT_MENU_SELECT     13
```

### Event Handling

Components can handle events at multiple levels:

```cpp
// Screen-level handling
void HomeScreen::handleEvent(int eventType, int eventData) {
    switch (eventType) {
        case EVENT_BUTTON_A:
            navigateUp();
            break;
        case EVENT_BUTTON_B:
            navigateDown();
            break;
        case EVENT_BUTTON_C:
            selectCurrent();
            break;
    }
}

// Component-level handling
void Button::handleEvent(int eventType, int eventData) {
    if (eventType == EVENT_SELECT && isHighlighted()) {
        click();
    }
}
```

## Best Practices

### Component Design

1. **Keep Components Small** - Single responsibility principle
2. **Use setState()** - Always use setState() for state changes
3. **Minimize Render Work** - Keep render() methods fast
4. **Handle Null Pointers** - Check component pointers before use

### Screen Design

1. **Build Once** - Components created in build(), not constructor
2. **Update Efficiently** - Only update what changes
3. **Handle Lifecycle** - Use onActivate()/onDeactivate() properly
4. **Resource Management** - Clean up resources when needed

### Performance Optimization

1. **Use Dirty Tracking** - Let the framework optimize redraws
2. **Batch Updates** - Group related state changes
3. **Avoid String Allocation** - Use const char* where possible
4. **Cache Calculations** - Pre-calculate expensive operations

## Extending the Framework

### Custom Components

Create new components by extending UIComponent or StatefulComponent:

```cpp
class CustomWidget : public StatefulComponent {
private:
    int value = 0;
    uint16_t color = COLOR_WHITE;

public:
    CustomWidget(int x, int y, int w, int h) : StatefulComponent(x, y, w, h) {}
    
    void setValue(int newValue) {
        if (value != newValue) {
            setState([this, newValue]() {
                value = newValue;
            });
        }
    }
    
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible()) return;
        
        // Custom rendering logic
        gfx.fillRect(x, y, w, h, color);
        // ... draw your widget
    }
};
```

### Custom Screens

Create new screens by extending Screen:

```cpp
class SettingsScreen : public Screen {
    void build() override {
        titleLabel = addComponent<Label>(10, 10, 220, 20, "Settings");
        
        // Add your components
        volumeSlider = addComponent<Slider>(10, 40, 200, 20);
        brightnessSlider = addComponent<Slider>(10, 70, 200, 20);
        
        backButton = addComponent<Button>(180, 110, 50, 20, "Back", [this]() {
            goBack();
        });
    }
    
    void update() override {
        Screen::update();
        // Update settings values
    }
};
```

## Integration with Existing Systems

### BeeperHero Game

The UI framework works alongside the existing BeeperHero game:

```cpp
// Game can use UI framework for menus and HUD
class GameScreen : public Screen {
    void build() override {
        scoreLabel = addComponent<Label>(10, 10, 100, 15, "Score: 0");
        comboLabel = addComponent<Label>(120, 10, 100, 15, "Combo: 0");
        
        // Game renders to the same display
        // UI components overlay on game graphics
    }
    
    void update() override {
        Screen::update();
        
        // Update game state
        if (beeperHeroGame) {
            beeperHeroGame->update();
            
            // Update UI with game state
            scoreLabel->setText("Score: " + String(beeperHeroGame->getScore()));
        }
    }
};
```

### MQTT Integration

UI screens can respond to MQTT messages:

```cpp
void HomeScreen::handleMQTTMessage(const char* topic, const char* message) {
    if (strcmp(topic, "alert") == 0) {
        // Show alert on screen
        statusLabel->setText("Alert: " + String(message));
        statusLabel->setTextColor(COLOR_RED);
    }
}
```

### Power Management

The UI framework respects power management:

```cpp
void UIManager::update() {
    if (powerManager.getCurrentState() == PowerState::SLEEP) {
        // Skip UI updates in sleep mode
        return;
    }
    
    // Normal UI updates
    if (currentScreen) {
        currentScreen->update();
    }
}
```

## Troubleshooting

### Common Issues

1. **Components Not Rendering**
   - Check if `build()` was called
   - Verify component visibility with `setVisible(true)`
   - Ensure screen is active

2. **Performance Issues**
   - Check for infinite dirty loops
   - Monitor memory usage with `ESP.getFreeHeap()`
   - Use dirty tracking properly

3. **Button Events Not Working**
   - Verify button manager integration
   - Check event type mapping
   - Ensure components are enabled

4. **Memory Leaks**
   - Use smart pointers or proper cleanup
   - Avoid creating components in update loops
   - Monitor heap usage over time

### Debug Tools

```cpp
// Enable debug output
#define UI_FRAMEWORK_DEBUG 1

// Monitor performance
void printFrameworkStats() {
    Serial.printf("Screens: %d\n", uiManager.getScreenCount());
    Serial.printf("Current Screen Components: %d\n", currentScreen->getComponentCount());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Last Render Time: %lu ms\n", lastRenderTime);
}
```

## Conclusion

The Alert TX-1 UI Framework provides a modern, efficient way to build user interfaces for embedded devices. Its React-like architecture makes it easy to create complex, responsive interfaces while maintaining excellent performance on resource-constrained hardware.

The framework successfully integrates with all existing Alert TX-1 systems including:
- Hardware button management
- RTTTL ringtone playback  
- BeeperHero rhythm game
- MQTT connectivity
- Power management

This creates a cohesive, professional user experience that rivals commercial embedded devices while remaining fully open-source and customizable.