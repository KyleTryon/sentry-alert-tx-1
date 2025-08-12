# Alert TX-1 UI Framework - Technical Reference

## 📋 Overview

This document provides detailed technical information about the UI framework implementation, including API references, performance characteristics, and integration guidelines.

---

## 🔧 Core System APIs

### UIManager Class

The main orchestrator that coordinates all UI subsystems.

```cpp
class UIManager {
public:
    // Initialization
    void begin(Arduino_GFX* display, ButtonManager* btnManager);
    
    // Main update loop (call in Arduino loop())
    void update();
    
    // Theme management
    void setTheme(const Theme& theme);
    const Theme& getTheme() const;
    
    // Screen management
    void addScreen(Screen* screen);
    void switchScreen(Screen* screen);
    void switchScreen(int screenIndex);
    Screen* getCurrentScreen();
    
    // Performance monitoring
    float getFPS() const;
    void printStats();
    
    // Event system access
    EventSystem* getEventSystem();
};
```

**Performance Characteristics:**
- **Frame Rate:** 30+ FPS with 33ms frame limiting
- **Memory Usage:** ~1KB for UI state management
- **Screen Switching:** <100ms transition time
- **Event Processing:** <5ms per update cycle

### EventSystem Class

Handles all UI events with a circular buffer queue.

```cpp
class EventSystem {
public:
    void pushEvent(UIEventType type, int id = 0, void* data = nullptr);
    bool popEvent(UIEvent& event);
    void clear();
    bool isEmpty() const;
    int getQueueSize() const;
};

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
```

**Technical Details:**
- **Queue Size:** 16 events maximum
- **Event Size:** 16 bytes per event (UIEventType + 3 ints + timestamp)
- **Overflow Behavior:** Oldest events dropped when queue full
- **Timestamp Precision:** Millisecond accuracy using millis()

### Theme System

Provides consistent color management across all components.

```cpp
struct Theme {
    uint16_t background;      // Main background color
    uint16_t foreground;      // Text and icon color
    uint16_t accent;          // Selection/highlight color
    uint16_t secondary;       // Secondary text color
    uint16_t border;          // Border/divider color
};

// Predefined themes
extern const Theme THEME_MONOCHROME;
extern const Theme THEME_GREEN;      
extern const Theme THEME_AMBER;      
extern const Theme THEME_INVERTED;   
```

**Color Definitions (RGB565):**
```cpp
#define RGB565_BLACK     0x0000  // Pure black
#define RGB565_WHITE     0xFFFF  // Pure white
#define RGB565_GREEN     0x07E0  // Bright green
#define RGB565_AMBER     0xFD20  // FlipperZero amber
#define RGB565_GRAY      0x8410  // Medium gray
```

---

## 🎨 Component Library APIs

### StatusBar Component

Displays system status information at the top of the screen.

```cpp
class StatusBar {
public:
    StatusBar();
    
    // Main drawing function
    void draw(DisplayRenderer* renderer, const Theme& theme);
    
    // Data updates
    void updateBatteryLevel(int percentage);  // 0-100%
    void updateWiFiStatus(bool connected);
    void updateBluetoothStatus(bool connected);
    void updateTime(const char* timeStr);     // "HH:MM:SS" format
    void updateTime(int hours, int minutes, int seconds);
    
    // State management
    void markForRedraw();
    bool hasChanged() const;
    
    // Layout info
    int getHeight() const;  // Returns 16 pixels
    int getY() const;       // Returns 0 (top of screen)
};
```

**Layout Specifications:**
- **Height:** 16 pixels
- **Battery Icon:** 16x16 pixels at left margin
- **WiFi Icon:** 16x16 pixels, 18px spacing
- **Bluetooth Icon:** 16x16 pixels, 18px spacing  
- **Time Display:** Right-aligned, 6px character width

### Menu Component

Provides scrollable navigation with FlipperZero-style controls.

```cpp
class Menu {
public:
    Menu();
    Menu(MenuItem* items, int count);
    
    // Setup
    void setItems(MenuItem* items, int count);
    
    // Drawing
    void draw(DisplayRenderer* renderer, const Theme& theme, int yOffset);
    
    // Navigation
    void navigateUp();
    void navigateDown();
    void select();
    void handleInput(ButtonAction action);
    void handleEvent(const UIEvent& event);
    
    // State management
    int getSelectedIndex() const;
    MenuItem* getSelectedItem();
    void setSelectedIndex(int index);
    void markForRedraw();
    bool hasChanged() const;
    
    // Layout info
    int getItemHeight() const;        // Returns 18 pixels
    int getVisibleItemCount() const;  // Returns 6 items
    int getTotalHeight() const;       // Returns 108 pixels
};
```

**Navigation Mapping:**
- **Button A (GPIO0):** Select/Enter (or Back on long press)
- **Button B (GPIO1):** Navigate Up/Previous
- **Button C (GPIO2):** Navigate Down/Next

**Visual Design:**
- **Item Height:** 18 pixels (16px icon + 2px padding)
- **Visible Items:** 6 items (108px total)
- **Selection Arrow:** ">" character at left margin
- **Icon Position:** 20px from left
- **Text Position:** 40px from left
- **Scroll Indicators:** Right edge arrows and progress bar

### IconRenderer Static Methods

Efficient rendering of 16x16 RGB565 icons with theme integration.

```cpp
class IconRenderer {
public:
    // Theme-aware drawing
    static void drawIcon(DisplayRenderer* renderer, const Icon& icon, 
                        int x, int y, const Theme& theme);
    
    // Custom color drawing
    static void drawIcon(DisplayRenderer* renderer, const Icon& icon,
                        int x, int y, uint16_t color);
    
    // Transparency support
    static void drawIconTransparent(DisplayRenderer* renderer, const Icon& icon,
                                   int x, int y, uint16_t foregroundColor, 
                                   uint16_t backgroundColor);
    
    // Color replacement
    static void drawIconWithColorReplacement(DisplayRenderer* renderer, 
                                            const Icon& icon, int x, int y, 
                                            uint16_t fromColor, uint16_t toColor);
    
    // Utility functions
    static bool isValidIcon(const Icon* icon);
    static void drawIconBounds(DisplayRenderer* renderer, const Icon& icon, 
                              int x, int y, uint16_t color);
};
```

**Performance Characteristics:**
- **Render Time:** ~2ms per 16x16 icon
- **Memory Access:** Direct PROGMEM reads with pgm_read_word()
- **Transparency:** Black pixels (0x0000) treated as transparent
- **Color Modes:** Original, theme-aware, transparent, and color-replaced

---

## 🖥️ Screen System

### Screen Base Class

Foundation for all UI screens with standardized lifecycle.

```cpp
class Screen {
public:
    virtual ~Screen() = default;
    
    // Core screen lifecycle
    virtual void begin() = 0;                           // Initialize screen
    virtual void update() = 0;                          // Update logic
    virtual void draw() = 0;                            // Render screen
    virtual void handleInput(ButtonAction action) = 0;  // Handle button input
    virtual void handleEvent(const UIEvent& event) = 0; // Handle UI events
    
    // Configuration
    void setDisplay(Arduino_GFX* d);
    void setRenderer(DisplayRenderer* r);
    void setTheme(const Theme& t);
    
    // State management
    bool needsRedraw() const;
    void markForRedraw();
    void clearRedrawFlag();
    
    // Screen identification
    virtual const char* getName() const = 0;
};
```

### MainMenuScreen Implementation

Complete implementation combining all UI components.

```cpp
class MainMenuScreen : public Screen {
private:
    StatusBar statusBar;
    Menu menu;
    MenuItem menuItems[6];
    
public:
    MainMenuScreen();
    
    // Screen interface implementation
    void begin() override;
    void update() override;
    void draw() override;
    void handleInput(ButtonAction action) override;
    void handleEvent(const UIEvent& event) override;
    const char* getName() const override;
    
    // Status updates
    void updateBatteryLevel(int percentage);
    void updateConnectivityStatus(bool wifi, bool bluetooth);
    void updateSystemTime(int hours, int minutes, int seconds);
};
```

**Menu Options:**
1. **Alerts** - Message and notification management
2. **Games** - BeeperHero and other games
3. **Settings** - System configuration
4. **Ringtones** - RTTTL ringtone management
5. **BeeperHero** - Rhythm game
6. **System Info** - Device information

---

## ⚡ Performance Specifications

### Frame Rate Performance
- **Target FPS:** 30 FPS (33ms per frame)
- **Actual Performance:** 30+ FPS consistently
- **Frame Time Budget:**
  - Event Processing: 5ms
  - Screen Update: 10ms
  - Rendering: 15ms
  - Remaining: 3ms buffer

### Memory Usage Breakdown
```
Total Framework Memory: ~73KB
├── Canvas Buffer: 64KB (240×135×2 bytes RGB565)
├── Icon Data: 8KB (25+ icons in PROGMEM)
├── UI Components: 1KB (StatusBar, Menu, etc.)
├── Event Queue: 256 bytes (16 events)
├── Button States: 72 bytes (3 buttons)
└── Theme Data: 40 bytes (4 themes)
```

### Response Time Metrics
- **Button Press to Event:** <1ms
- **Event to Screen Update:** <5ms
- **Screen Update to Display:** <15ms
- **Total Response Time:** <21ms (well under human perception threshold)

---

## 🔌 Hardware Integration

### ESP32-S3 Reverse TFT Feather Configuration

**Display Configuration:**
```cpp
// ESP32-S3 Reverse TFT Feather pin definitions
#define TFT_CS        7
#define TFT_RST       40 
#define TFT_DC        39
#define TFT_BACKLIGHT 45
// TFT_I2C_POWER and TFT_BACKLITE are predefined by the board variant

// Initialize display using Adafruit libraries
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// In setup() - CRITICAL: Follow this exact initialization sequence:
// 1. Enable TFT power supply (REQUIRED!)
pinMode(TFT_I2C_POWER, OUTPUT);
digitalWrite(TFT_I2C_POWER, HIGH);
delay(10);

// 2. Enable backlight
pinMode(TFT_BACKLITE, OUTPUT);  // Note: TFT_BACKLITE not TFT_BACKLIGHT
digitalWrite(TFT_BACKLITE, HIGH);

// 3. Initialize display
tft.init(135, 240);  // Portrait dimensions
tft.setRotation(3);  // Rotate to landscape (240x135)
```

**⚠️ Critical Notes:**
- `TFT_I2C_POWER` **MUST** be enabled first or display will not work (shows only backlight)
- Use `TFT_BACKLITE` (not `TFT_BACKLIGHT`) - this is the board-specific define
- Both `TFT_I2C_POWER` and `TFT_BACKLITE` are predefined by the ESP32-S3 Reverse TFT Feather board variant

**Button Pin Configuration:**
```cpp
#define BUTTON_A_PIN 0   // GPIO0 - D0/BOOT button (pulled HIGH, active LOW)
#define BUTTON_B_PIN 1   // GPIO1 - D1 button (pulled LOW, active HIGH)  
#define BUTTON_C_PIN 2   // GPIO2 - D2 button (pulled LOW, active HIGH)
```

**Button Characteristics:**
- **Debounce Time:** 50ms
- **Long Press Threshold:** 1000ms
- **Repeat Interval:** 200ms (for held buttons)
- **Wake-up Sources:** All 3 buttons configured for deep sleep wake

### Power Management Integration

**Deep Sleep Configuration:**
```cpp
// Button A (GPIO0) - wake on LOW
esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);

// Buttons B & C (GPIO1, GPIO2) - wake on HIGH  
esp_sleep_enable_ext1_wakeup(
    ((1ULL << GPIO_NUM_1) | (1ULL << GPIO_NUM_2)), 
    ESP_EXT1_WAKEUP_ANY_HIGH
);
```

---

## 🛠️ Development Guidelines

### Creating Custom Screens

1. **Inherit from Screen base class**
2. **Implement all virtual methods**
3. **Use renderer for all drawing operations**
4. **Respect theme colors for consistency**
5. **Handle events efficiently**

```cpp
class CustomScreen : public Screen {
private:
    bool dataChanged = false;
    
public:
    void begin() override {
        // Initialize screen state
        markForRedraw();
    }
    
    void update() override {
        // Update logic here
        if (dataChanged) {
            markForRedraw();
            dataChanged = false;
        }
    }
    
    void draw() override {
        if (!renderer || !needsRedraw()) return;
        
        // Clear background
        renderer->fillScreen(theme.background);
        
        // Draw content using theme colors
        renderer->setTextColor(theme.foreground);
        renderer->setCursor(10, 30);
        renderer->print("Custom Screen");
        
        clearRedrawFlag();
    }
    
    void handleInput(ButtonAction action) override {
        switch (action) {
            case ACTION_SELECT:
                // Handle selection
                break;
            case ACTION_BACK:
                // Handle back navigation
                break;
        }
    }
    
    void handleEvent(const UIEvent& event) override {
        // Handle specific events
    }
    
    const char* getName() const override {
        return "CustomScreen";
    }
};
```

### Adding Custom Components

1. **Follow existing component patterns**
2. **Implement needsRedraw/markForRedraw pattern**
3. **Use theme colors consistently**
4. **Optimize for performance**

### Theme Development

```cpp
const Theme THEME_CUSTOM = {
    .background = RGB565_CUSTOM_BG,    // Background color
    .foreground = RGB565_CUSTOM_FG,    // Text/icon color
    .accent = RGB565_CUSTOM_ACCENT,    // Selection color
    .secondary = RGB565_CUSTOM_SEC,    // Secondary text
    .border = RGB565_CUSTOM_BORDER     // Border color
};
```

---

## 🧪 Testing and Validation

### Performance Testing

Run the built-in performance monitoring:
```cpp
// Long press Button C to show stats
uiManager.printStats();

// Manual performance check
Serial.printf("FPS: %.1f\n", uiManager.getFPS());
Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
```

### Memory Testing

```cpp
// Check available memory
Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
Serial.printf("Largest Free Block: %u bytes\n", ESP.getMaxAllocHeap());
Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
```

### Visual Testing

1. **Theme Switching:** Press A+B simultaneously
2. **Menu Navigation:** Use B/C buttons to scroll
3. **Selection Feedback:** Press A to select items
4. **Status Updates:** Observe battery/time changes

---

## 📚 API Quick Reference

### Essential Includes
```cpp
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "src/ui/core/UIManager.h"
#include "src/ui/screens/MainMenuScreen.h"  
#include "src/hardware/ButtonManager.h"
#include "src/ui/core/Theme.h"
```

### Basic Setup Template
```cpp
UIManager uiManager;
MainMenuScreen mainMenuScreen;
ButtonManager buttonManager;

void setup() {
    // Initialize display
    display->begin();
    
    // Initialize UI framework
    uiManager.begin(display, &buttonManager);
    uiManager.addScreen(&mainMenuScreen);
    uiManager.setTheme(THEME_AMBER);
}

void loop() {
    uiManager.update();
}
```

### Common Operations
```cpp
// Theme switching
uiManager.setTheme(THEME_GREEN);

// Screen switching
uiManager.switchScreen(&customScreen);

// Status updates
mainMenuScreen.updateBatteryLevel(75);
mainMenuScreen.updateConnectivityStatus(true, false);
mainMenuScreen.updateSystemTime(14, 30, 45);

// Performance monitoring
float fps = uiManager.getFPS();
uiManager.printStats();
```

This technical reference provides the detailed implementation information needed for developers to understand, extend, and maintain the UI framework effectively.
