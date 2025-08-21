# API Reference

Complete API documentation for the AlertTX-1 project components.

## Core Classes

### Screen

Base class for all UI screens.

```cpp
class Screen {
public:
    // Lifecycle
    virtual void enter();                    // Called when screen becomes active
    virtual void exit();                     // Called when leaving screen
    virtual void update();                   // Update logic (called each frame)
    virtual void draw();                     // Render screen
    
    // Input handling
    virtual void handleButtonPress(int button);
    virtual void handleButtonRelease(int button);
    
    // State management
    void markForFullRedraw();               // Request complete redraw
    bool isActive() const;                  // Check if screen is active
    
protected:
    virtual void cleanup();                 // Free resources (called from exit)
    
    // Component management
    void addComponent(Component* component);
    void removeComponent(Component* component);
    void clearComponents();
};
```

### Component

Base class for UI components.

```cpp
class Component {
public:
    // Rendering
    virtual void draw() = 0;               // Render component
    void markDirty();                      // Mark for redraw
    bool isDirty() const;                  // Check if needs redraw
    void clearDirty();                     // Clear dirty flag
    
    // Visibility
    void setVisible(bool visible);
    bool isVisible() const;
    
    // Bounds
    void setBounds(int x, int y, int width, int height);
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    
    // Interaction
    virtual bool contains(int x, int y) const;
};
```

### ScreenManager

Manages navigation between screens.

```cpp
class ScreenManager {
public:
    // Singleton access
    static ScreenManager* getInstance();
    
    // Navigation
    void pushScreen(Screen* screen, bool takeOwnership = false);
    bool popScreen();
    void clearStack();
    
    // Update cycle
    void update();
    void draw();
    
    // State
    Screen* getCurrentScreen() const;
    int getStackSize() const;
};
```

## UI Components

### MenuContainer

Container for menu items with navigation.

```cpp
class MenuContainer : public Component {
public:
    // Menu items
    void addMenuItem(const char* label, int id, std::function<void()> callback);
    void clearMenuItems();
    
    // Navigation
    void navigateUp();
    void navigateDown();
    void selectCurrent();
    
    // Selection
    int getSelectedIndex() const;
    void setSelectedIndex(int index);
    
    // Scrolling
    void setMaxVisibleItems(int max);
    int getScrollOffset() const;
};
```

### MenuItem

Individual menu item with callback.

```cpp
class MenuItem : public Component, public Clickable {
public:
    // Properties
    void setLabel(const char* label);
    const char* getLabel() const;
    
    // Selection
    void setSelected(bool selected);
    bool isSelected() const;
    
    // Callback
    void setCallback(std::function<void()> callback);
    
    // Clickable interface
    void onPress() override;
    void onRelease() override;
    void onClick() override;
};
```

## Hardware Components

### ButtonManager

Manages button input with debouncing and long press detection.

```cpp
class ButtonManager {
public:
    // Constants
    static const int BUTTON_A = 0;  // GPIO0
    static const int BUTTON_B = 1;  // GPIO1
    static const int BUTTON_C = 2;  // GPIO2
    
    // Initialization
    void begin();
    void update();
    
    // State queries
    bool isPressed(int button) const;
    bool wasPressed(int button) const;
    bool wasReleased(int button) const;
    bool isLongPressed(int button) const;
    
    // Configuration
    void setDebounceDelay(unsigned long delay);
    void setLongPressDelay(unsigned long delay);
};
```

### Buzzer

Audio output control.

```cpp
class Buzzer {
public:
    static void begin(int pin);
    static void tone(int frequency, int duration = 0);
    static void noTone();
    static void beep(int duration = 100);
    static void doubleBeep();
    static void errorBeep();
};
```

### LED

Status LED control.

```cpp
class LED {
public:
    static void begin(int pin);
    static void on();
    static void off();
    static void toggle();
    static void blink(int times = 1, int delayMs = 100);
    static bool isOn();
};
```

## Theme System

### ThemeManager

Manages UI themes and colors.

```cpp
class ThemeManager {
public:
    // Initialization
    static void begin();
    static void loadFromSettings();
    
    // Theme selection
    static void setTheme(const Theme* theme);
    static void setThemeByIndex(int index, bool persist = false);
    static const Theme* getCurrentTheme();
    static int getCurrentThemeIndex();
    
    // Theme access
    static int getThemeCount();
    static const char* getThemeName(int index);
    static const Theme* getThemeByIndex(int index);
    
    // Color access
    static uint16_t getBackground();
    static uint16_t getPrimary();
    static uint16_t getSecondary();
    static uint16_t getAccent();
    static uint16_t getBorder();
    static uint16_t getText();
    static uint16_t getTextDim();
    static uint16_t getSuccess();
    static uint16_t getWarning();
    static uint16_t getError();
};
```

### Theme

Theme color definition.

```cpp
struct Theme {
    const char* name;
    uint16_t background;
    uint16_t primary;
    uint16_t secondary;
    uint16_t accent;
    uint16_t border;
    uint16_t text;
    uint16_t textDim;
    uint16_t success;
    uint16_t warning;
    uint16_t error;
};
```

## Settings Management

### SettingsManager

Persistent storage using ESP32 NVS.

```cpp
class SettingsManager {
public:
    // Initialization
    static bool begin();
    
    // Theme settings
    static int getThemeIndex();
    static bool setThemeIndex(int index);
    
    // Ringtone settings
    static int getRingtoneIndex();
    static bool setRingtoneIndex(int index);
    
    // Custom settings
    static int getInt(const char* key, int defaultValue = 0);
    static bool setInt(const char* key, int value);
    static bool getString(const char* key, char* buffer, size_t maxLen);
    static bool setString(const char* key, const char* value);
    
    // Maintenance
    static void clear();
    static void printDebugInfo();
};
```

## Audio System

### RingtonePlayer

Plays RTTTL ringtones.

```cpp
class RingtonePlayer {
public:
    // Initialization
    void begin(int buzzerPin);
    
    // Playback
    void playRingtone(const char* rtttl);
    void playRingtoneByName(const char* name);
    void playRingtoneByIndex(int index);
    void stop();
    
    // Status
    bool isPlaying() const;
    void update();  // Must be called in loop
    
    // Library access
    int getRingtoneCount() const;
    const char* getRingtoneName(int index) const;
    int findRingtoneIndex(const char* name) const;
};
```

## Network Components

### MQTTClient

MQTT client for receiving alerts.

```cpp
class MQTTClient {
public:
    // Configuration
    void setup(const char* server, int port, const char* clientId);
    void setCallback(std::function<void(const char*, const char*)> callback);
    
    // Connection
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    // Subscription
    bool subscribe(const char* topic);
    bool unsubscribe(const char* topic);
    
    // Publishing
    bool publish(const char* topic, const char* payload);
    
    // Maintenance
    void loop();  // Must be called regularly
};
```

## Utility Functions

### DisplayUtils

Helper functions for display operations.

```cpp
namespace DisplayUtils {
    // Text rendering
    void centerText(Adafruit_ST7789* display, const char* text, 
                   int y, int size = 2);
    void drawTitle(Adafruit_ST7789* display, const char* title, 
                  int x, int y, int size = 2);
    
    // Layout helpers
    int getTextWidth(const char* text, int size = 1);
    int getTextHeight(int size = 1);
    
    // Drawing helpers
    void drawRoundedRect(Adafruit_ST7789* display, int x, int y, 
                        int w, int h, int r, uint16_t color);
    void drawProgressBar(Adafruit_ST7789* display, int x, int y, 
                        int w, int h, float progress, uint16_t color);
}
```

### InputRouter

Centralized input handling.

```cpp
class InputRouter {
private:
    ButtonManager* buttons;
    unsigned long lastActivityMs;
    
public:
    // Initialization
    void begin(ButtonManager* buttonManager);
    
    // Update
    void update();
    
    // Activity tracking
    unsigned long getLastActivityTime() const;
    bool hasRecentActivity(unsigned long timeoutMs) const;
    
    // Route configuration
    void setLongPressHandler(std::function<void()> handler);
    void setButtonHandler(int button, std::function<void()> handler);
};
```

## Game Framework

### GameScreen

Base class for games with frame rate control.

```cpp
class GameScreen : public Screen {
protected:
    // Frame timing
    unsigned long lastUpdateMs = 0;
    unsigned long targetFrameTime = 16;  // 60 FPS
    
    // Game area
    int gameLeft, gameRight, gameTop, gameBottom;
    
public:
    // Game lifecycle
    virtual void updateGame() = 0;
    virtual void drawGame() = 0;
    virtual void drawStatic() = 0;
    
    // Configuration
    void setTargetFPS(int fps);
    
    // Helpers
    bool shouldUpdateFrame();
    void markRegionDirty(int x, int y, int w, int h);
};
```

### GameObject

Base class for game objects with position tracking.

```cpp
class GameObject {
protected:
    int x, y, prevX, prevY;
    int width, height;
    uint16_t color;
    bool visible = true;
    
public:
    // Movement
    void setPosition(int newX, int newY);
    void move(int dx, int dy);
    
    // Rendering
    void clearPrevious(Adafruit_ST7789* display, uint16_t bgColor);
    void draw(Adafruit_ST7789* display);
    
    // Collision
    bool intersects(const GameObject& other) const;
    bool contains(int px, int py) const;
    
    // Properties
    bool isVisible() const;
    void setVisible(bool v);
};
```

## Constants

### Pin Definitions

```cpp
// Buttons
const int BUTTON_A_PIN = 0;   // GPIO0 - Up/Previous
const int BUTTON_B_PIN = 1;   // GPIO1 - Down/Next
const int BUTTON_C_PIN = 2;   // GPIO2 - Select/Enter

// Audio/Visual
const int BUZZER_PIN = 14;    // GPIO14 (A4)
const int LED_PIN = 18;       // GPIO18 (A0)

// Display (built-in)
// TFT_CS, TFT_DC, TFT_RST defined by board
```

### Display Configuration

```cpp
namespace DisplayConfig {
    const int SCREEN_WIDTH = 240;
    const int SCREEN_HEIGHT = 240;  // 135 physical, 240 after rotation
    const int ROTATION = 3;         // Landscape
    
    const int HEADER_HEIGHT = 30;
    const int FOOTER_HEIGHT = 30;
    const int CONTENT_HEIGHT = SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT;
}
```

### Timing Constants

```cpp
// Input timing
const unsigned long DEBOUNCE_DELAY = 50;      // ms
const unsigned long LONG_PRESS_DELAY = 1500;  // ms
const unsigned long REPEAT_DELAY = 500;       // ms
const unsigned long REPEAT_RATE = 100;        // ms

// Display timing
const unsigned long SPLASH_DURATION = 2000;   // ms
const unsigned long MENU_SCROLL_DELAY = 200;  // ms
```

## Error Codes

```cpp
enum ErrorCode {
    SUCCESS = 0,
    ERROR_WIFI_CONNECT = -1,
    ERROR_MQTT_CONNECT = -2,
    ERROR_DISPLAY_INIT = -3,
    ERROR_SD_MOUNT = -4,
    ERROR_MEMORY_ALLOC = -5,
    ERROR_FILE_NOT_FOUND = -6,
    ERROR_INVALID_PARAM = -7
};
```

---

**API Version**: 1.0  
**Last Updated**: December 2024
