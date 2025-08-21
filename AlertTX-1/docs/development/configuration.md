# Configuration Options

Complete reference for all configurable options in the AlertTX-1 project.

## settings.h Configuration

Main configuration file: `src/config/settings.h`

### Pin Configuration

```cpp
// Button pins (GPIO)
const int BUTTON_A_PIN = 0;        // Built-in button A (Up/Previous)
const int BUTTON_B_PIN = 1;        // Built-in button B (Down/Next)
const int BUTTON_C_PIN = 2;        // Built-in button C (Select/Enter)

// Audio/Visual pins
const int BUZZER_PIN = 14;         // GPIO14 (A4) - Passive buzzer
const int LED_PIN = 18;            // GPIO18 (A0) - Status LED
```

### Network Configuration

```cpp
// WiFi settings (in main sketch)
const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";

// MQTT settings
const char* mqtt_server = "mqtt.broker.address";
const int mqtt_port = 1883;
const char* mqtt_username = "";    // Optional
const char* mqtt_password = "";    // Optional
const char* mqtt_client_id = "AlertTX1";
const char* mqtt_topic = "alerts/beeper";

// Network timeouts
const unsigned long WIFI_TIMEOUT = 10000;      // 10 seconds
const unsigned long MQTT_TIMEOUT = 5000;       // 5 seconds
const unsigned long MQTT_RECONNECT_DELAY = 5000; // 5 seconds
```

### Display Configuration

```cpp
// Display dimensions
const int DISPLAY_WIDTH = 240;
const int DISPLAY_HEIGHT = 240;    // After rotation
const int DISPLAY_ROTATION = 3;    // 0-3 (3 = landscape)

// Layout configuration
const int HEADER_HEIGHT = 30;
const int FOOTER_HEIGHT = 30;
const int CONTENT_START_Y = HEADER_HEIGHT;
const int CONTENT_HEIGHT = DISPLAY_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT;

// Text sizes
const int TITLE_TEXT_SIZE = 3;
const int NORMAL_TEXT_SIZE = 2;
const int SMALL_TEXT_SIZE = 1;
```

### Input Configuration

```cpp
// Button timing (milliseconds)
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long LONG_PRESS_DELAY = 1500;   // Back navigation
const unsigned long REPEAT_INITIAL_DELAY = 500; // Before auto-repeat
const unsigned long REPEAT_RATE = 100;          // During auto-repeat

// Button behavior
const bool ENABLE_AUTO_REPEAT = true;          // For A/B navigation
const bool ENABLE_LONG_PRESS_BACK = true;      // Global back navigation
```

### Audio Configuration

```cpp
// Buzzer settings
const int DEFAULT_BEEP_FREQUENCY = 1000;       // Hz
const int DEFAULT_BEEP_DURATION = 100;          // ms
const int ERROR_BEEP_FREQUENCY = 500;           // Hz
const int SUCCESS_BEEP_FREQUENCY = 2000;        // Hz

// Ringtone settings
const int DEFAULT_RINGTONE_INDEX = 0;           // Startup ringtone
const bool PLAY_STARTUP_SOUND = true;
const bool PLAY_ALERT_SOUND = true;
const int ALERT_REPEAT_COUNT = 3;               // Times to repeat alert
```

### Theme Configuration

```cpp
// Default theme index (0-4)
const int DEFAULT_THEME_INDEX = 0;  // 0=Default, 1=HighContrast, 2=Terminal, 3=Amber, 4=Sentry

// Available themes
enum ThemeIndex {
    THEME_DEFAULT = 0,
    THEME_HIGH_CONTRAST = 1,
    THEME_TERMINAL = 2,
    THEME_AMBER = 3,
    THEME_SENTRY = 4
};
```

### Power Management

```cpp
// Sleep settings (milliseconds)
const unsigned long INACTIVITY_TIMEOUT = 60000;     // 1 minute to dim
const unsigned long DIM_GRACE_PERIOD = 10000;       // 10s before deep sleep
const unsigned long DEEP_SLEEP_INTERVAL = 300000;   // 5 minute wake interval

// Battery monitoring
const bool ENABLE_BATTERY_MONITOR = true;
const int BATTERY_CHECK_INTERVAL = 60000;           // Check every minute
const int LOW_BATTERY_THRESHOLD = 15;               // Percentage
const int CRITICAL_BATTERY_THRESHOLD = 5;           // Percentage
```

### Memory Configuration

```cpp
// Component limits
const int MAX_COMPONENTS_PER_SCREEN = 20;
const int MAX_MENU_ITEMS = 50;
const int MAX_ALERTS = 100;
const int MAX_GAME_OBJECTS = 50;

// Buffer sizes
const int MQTT_BUFFER_SIZE = 1024;
const int JSON_BUFFER_SIZE = 512;
const int STRING_BUFFER_SIZE = 256;
```

## Build Configuration

### Makefile Variables

```makefile
# Board settings
BOARD_FQBN = esp32:esp32:adafruit_feather_esp32s3_reversetft
UPLOAD_SPEED = 921600
CPU_FREQUENCY = 240MHz
FLASH_MODE = qio
FLASH_SIZE = 4MB

# Build options
OPTIMIZATION_LEVEL = -Os           # Size optimization
WARNINGS = -Wall -Wextra
DEBUG_LEVEL = 0                     # 0=none, 1=error, 2=warn, 3=info, 4=debug, 5=verbose

# Feature flags
ENABLE_SERIAL_DEBUG = 1
ENABLE_WIFI = 1
ENABLE_MQTT = 1
ENABLE_GAMES = 1
ENABLE_THEMES = 1
ENABLE_POWER_MANAGEMENT = 0         # Future feature
```

## Compile-Time Options

### Debug Configuration

```cpp
// Debug output levels
#define DEBUG_NONE     0
#define DEBUG_ERROR    1
#define DEBUG_WARNING  2
#define DEBUG_INFO     3
#define DEBUG_VERBOSE  4

// Current debug level
#define DEBUG_LEVEL DEBUG_INFO

// Debug macros
#if DEBUG_LEVEL >= DEBUG_ERROR
  #define DEBUG_ERROR(msg) Serial.println("[ERROR] " msg)
#else
  #define DEBUG_ERROR(msg)
#endif

// Feature-specific debug
#define DEBUG_MQTT      0    // MQTT debugging
#define DEBUG_DISPLAY   0    // Display operations
#define DEBUG_INPUT     0    // Button input
#define DEBUG_MEMORY    0    // Memory usage
```

### Feature Toggles

```cpp
// Core features
#define FEATURE_WIFI_ENABLED        1
#define FEATURE_MQTT_ENABLED        1
#define FEATURE_THEMES_ENABLED      1
#define FEATURE_RINGTONES_ENABLED   1

// Optional features
#define FEATURE_GAMES_ENABLED       1
#define FEATURE_BATTERY_MONITOR     1
#define FEATURE_SD_CARD             0    // Future
#define FEATURE_OTA_UPDATE          0    // Future
#define FEATURE_SLEEP_MODE          0    // Future

// Game features
#define GAME_BEEPER_HERO_ENABLED    1
#define GAME_PONG_ENABLED          1
#define GAME_SNAKE_ENABLED         1
```

### Performance Tuning

```cpp
// Display performance
#define USE_DMA_DISPLAY            0    // Use DMA for display transfers
#define DISPLAY_BUFFER_SIZE        0    // 0=no buffer, or buffer size
#define USE_PARTIAL_UPDATE         1    // Enable partial screen updates

// Game performance
#define GAME_TARGET_FPS           60    // Target frame rate
#define GAME_FRAME_SKIP_MAX        3    // Max frames to skip
#define GAME_DIRTY_TRACKING        1    // Enable dirty region tracking

// Memory optimization
#define USE_PROGMEM               1     // Store constants in flash
#define MINIMIZE_RAM_USAGE        0     // Aggressive RAM optimization
```

## Runtime Configuration

### Persistent Settings (NVS)

Settings stored in ESP32 Non-Volatile Storage:

```cpp
// Theme settings
"theme_idx"        // int: Current theme index (0-4)

// Audio settings  
"ringtone_idx"     // int: Selected ringtone index
"volume"           // int: Volume level (0-100)
"mute"             // bool: Mute status

// Network settings
"wifi_ssid"        // string: WiFi SSID
"wifi_pass"        // string: WiFi password (encrypted)
"mqtt_server"      // string: MQTT broker address
"mqtt_port"        // int: MQTT port

// Game settings
"beeper_highscore" // int: BeeperHero high score
"pong_highscore"   // int: Pong high score  
"snake_highscore"  // int: Snake high score

// Power settings
"sleep_timeout"    // int: Inactivity timeout (ms)
"sleep_interval"   // int: Deep sleep check interval (ms)
"backlight_dim"    // int: Dim level (0-100)
```

### Environment-Specific Configuration

#### Development
```cpp
#ifdef DEVELOPMENT
  #define SERIAL_BAUD_RATE 115200
  #define DEBUG_LEVEL DEBUG_VERBOSE
  #define STARTUP_DELAY 2000        // Wait for serial monitor
#endif
```

#### Production
```cpp
#ifdef PRODUCTION
  #define SERIAL_BAUD_RATE 0        // Disable serial
  #define DEBUG_LEVEL DEBUG_NONE
  #define STARTUP_DELAY 0
#endif
```

#### Testing
```cpp
#ifdef TESTING
  #define MOCK_HARDWARE 1           // Use mock hardware
  #define SKIP_WIFI_CONNECT 1       // Skip network
  #define USE_TEST_DATA 1           // Use test alerts
#endif
```

## Configuration Best Practices

### 1. Use Constants
```cpp
// Good
const int BUTTON_PIN = 14;
pinMode(BUTTON_PIN, INPUT);

// Bad
pinMode(14, INPUT);  // Magic number
```

### 2. Group Related Settings
```cpp
// Good - grouped by feature
namespace AudioConfig {
    const int BUZZER_PIN = 14;
    const int DEFAULT_VOLUME = 80;
    const bool ENABLE_STARTUP_SOUND = true;
}

// Bad - scattered constants
const int BUZZER = 14;
const int VOL = 80;
```

### 3. Document Units
```cpp
// Good
const unsigned long TIMEOUT_MS = 5000;      // 5 seconds in milliseconds
const int SPEED_MPS = 10;                   // Speed in meters per second

// Bad  
const int TIMEOUT = 5000;  // What unit?
const int SPEED = 10;      // What unit?
```

### 4. Provide Defaults
```cpp
// Good
int getRingtoneIndex() {
    return SettingsManager::getInt("ringtone_idx", DEFAULT_RINGTONE_INDEX);
}

// Bad
int getRingtoneIndex() {
    return SettingsManager::getInt("ringtone_idx");  // No default
}
```

## Customization Examples

### Custom Theme
```cpp
// In src/ui/core/Theme.cpp
const Theme THEME_CUSTOM = {
    "Custom",              // name
    0x0000,               // background (black)
    0xFFFF,               // primary (white)
    0x7BEF,               // secondary (gray)
    0xF800,               // accent (red)
    0x4208,               // border (dark gray)
    0xFFFF,               // text (white)
    0x7BEF,               // textDim (gray)
    0x07E0,               // success (green)
    0xFE00,               // warning (yellow)
    0xF800                // error (red)
};
```

### Custom Ringtone
Add to `data/ringtones/custom.rtttl.txt`:
```
Custom:d=4,o=5,b=120:8c,8e,8g,c6,8g,8e,c,8c
```

### Custom Network Settings
```cpp
// In AlertTX-1.ino
#ifdef HOME_NETWORK
    const char* ssid = "HomeWiFi";
    const char* mqtt_server = "192.168.1.100";
#else
    const char* ssid = "OfficeWiFi";
    const char* mqtt_server = "mqtt.office.local";
#endif
```

---

**Configuration Version**: 1.0  
**Last Updated**: December 2024
