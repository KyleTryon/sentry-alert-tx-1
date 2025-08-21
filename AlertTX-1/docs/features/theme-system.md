# Theme Selection System Implementation

## Overview

The Alert TX-1 now features a comprehensive theme selection system with real-time preview and persistent storage. Users can navigate to a dedicated theme selection screen, preview themes instantly by navigating through the list, and save their preferred theme permanently.

Note: Ringtone selection in `Settings` now follows the same persistence pattern using `SettingsManager::setRingtoneIndex()` / `getRingtoneIndex()` and previews the selected ringtone on change.

## 🎨 Features

### Core Functionality
- **Dynamic Theme List**: Automatically pulls available themes from `ThemeManager`
- **Real-Time Preview**: Themes are applied immediately when navigating through the list
- **Persistent Storage**: Selected themes are saved automatically using ESP32 NVS
- **Graceful Fallbacks**: Handles corrupted/missing settings with intelligent defaults  
- **Memory Efficient**: Uses ESP32 Preferences library (~200 bytes memory footprint)
- **5 Themes Available**: Default, High Contrast, Terminal, Amber, and Sentry themes

### User Experience
- **Clean Interface**: Simple list of theme names without clutter
- **Immediate Feedback**: See theme changes instantly while browsing
- **Current Indicator**: Shows which theme is currently saved
- **Consistent Navigation**: Uses standard A=Up, B=Down, C=Select pattern
- **Back Navigation**: Long press any button (1500ms) to return to settings

## 🏗️ Architecture

### Component Structure
```
Theme Selection System
├── SettingsManager.h/cpp        # ESP32 NVS persistence layer
├── ThemeManager.h/cpp (Enhanced)  # Theme registry with metadata
├── ThemeSelectionScreen.h/cpp    # Dedicated theme selection UI
└── SettingsScreen.h/cpp (Updated) # Navigation to theme selection
```

### Data Flow
```
1. Boot: SettingsManager::begin() → Load saved theme index from NVS
2. Startup: ThemeManager::loadFromSettings() → Apply saved theme
3. User: Navigate to Settings → Themes → ThemeSelectionScreen
4. Preview: Navigate list → Immediate theme application (no save)
5. Select: Press C → Apply theme + save to NVS
6. Return: Long press → Return to settings (theme persisted)
```

## 📁 Implementation Details

### 1. SettingsManager (Persistence Layer)

**File**: `src/config/SettingsManager.h/cpp`

**Purpose**: Provides robust, production-ready persistent storage for user preferences using ESP32's Non-Volatile Storage (NVS).

**Key Features**:
- ESP32 Preferences library integration
- Automatic error handling and graceful fallbacks
- Validation of saved values
- Debug logging for troubleshooting
- Namespace isolation (`"alerttx1"`)

**API**:
```cpp
SettingsManager::begin();                    // Initialize NVS
int themeIndex = SettingsManager::getThemeIndex();  // Get saved theme (0-3)
bool success = SettingsManager::setThemeIndex(2);   // Save theme
SettingsManager::printDebugInfo();          // Debug NVS status
```

**Storage Details**:
- **Namespace**: `"alerttx1"` (under 15 char limit)
- **Key**: `"theme_idx"` (theme index 0-3)
- **Fallback**: Returns 0 (Default theme) if no saved value or corruption
- **Validation**: Ensures theme index is within valid range (0-3)

### 2. Enhanced ThemeManager

**File**: `src/ui/core/Theme.h/cpp`

**Enhancements**:
- **Theme Registry**: Static arrays for themes, names, and descriptions
- **Index-Based Access**: Get theme by index for dynamic menu creation
- **Metadata Support**: Theme names and descriptions for UI display
- **Persistence Integration**: `loadFromSettings()` and `setThemeByIndex(index, persist)`
- **Validation**: `isValidThemeIndex()` for bounds checking

**New API**:
```cpp
ThemeManager::loadFromSettings();           // Load theme from SettingsManager
ThemeManager::setThemeByIndex(2, true);     // Apply theme + save to storage
const char* name = ThemeManager::getThemeName(1);    // Get theme name
const char* desc = ThemeManager::getThemeDescription(1); // Get description
const Theme* theme = ThemeManager::getThemeByIndex(2);   // Get theme data
int current = ThemeManager::getCurrentThemeIndex();      // Get active theme index
```

**Theme Registry**:
```cpp
static const Theme* themes[5] = {
    &THEME_DEFAULT,      // Index 0: "Default" - 
    &THEME_HIGH_CONTRAST,// Index 1: "High Contrast" - Pure black and white for accessibility
    &THEME_TERMINAL,     // Index 2: "Terminal" - Retro green terminal computing style
    &THEME_AMBER,        // Index 3: "Amber" - Classic amber CRT with dark amber background
    &THEME_SENTRY        // Index 4: "Sentry" - Purple theme with company branding colors
};
```

### 3. ThemeSelectionScreen

**File**: `src/ui/screens/ThemeSelectionScreen.h/cpp`

**Purpose**: Dedicated screen for theme selection with immediate preview and clean interface.

**Key Features**:
- **Dynamic Menu Creation**: Automatically creates menu items from `ThemeManager::THEME_COUNT`
- **Lambda Callbacks**: Modern C++ approach with captured theme indices
- **Live Preview**: Themes applied immediately on navigation (no persistence)
- **Permanent Selection**: C button saves theme with persistence
- **Simplified UI**: No descriptions, just clean theme name list

**Navigation Flow**:
```
Settings Screen → "Themes" → ThemeSelectionScreen
├── A Button: Move up in theme list (live preview)
├── B Button: Move down in theme list (live preview)  
├── C Button: Select theme permanently (save to NVS)
└── Long Press: Return to settings (theme persisted)
```

**Implementation Highlights**:
```cpp
// Dynamic menu creation from ThemeManager
for (int i = 0; i < ThemeManager::THEME_COUNT; i++) {
    const char* themeName = ThemeManager::getThemeName(i);
    auto callback = [this, i]() { this->onThemeSelected(i); };
    themeMenu->addMenuItem(themeName, i, callback);
}

// Live preview on navigation
if (currentSelected != selectedThemeIndex) {
    selectedThemeIndex = currentSelected;
    ThemeManager::setThemeByIndex(selectedThemeIndex, false); // Preview only
    markForFullRedraw();
}

// Permanent selection on button press
void onThemeSelected(int themeIndex) {
    ThemeManager::setThemeByIndex(themeIndex, true); // Apply + save
    markForFullRedraw();
}
```

### 4. Updated SettingsScreen

**File**: `src/ui/screens/SettingsScreen.h/cpp`

**Changes**:
- **Child Screen Management**: Creates and manages `ThemeSelectionScreen` instance
- **Navigation Integration**: `onThemesSelected()` uses `ScreenManager::pushScreen()`
- **Lifecycle Management**: `initializeChildScreens()` and `cleanupChildScreens()`
- **Menu Update**: Changed "Theme" to "Themes" for clarity

**Screen Hierarchy**:
```
MainMenuScreen
└── SettingsScreen
    ├── Ringtone (placeholder)
    ├── Themes → ThemeSelectionScreen
    └── System Info (placeholder)
```

## 🚀 Integration & Startup

### Main Sketch Updates

**File**: `AlertTX-1.ino`

**Initialization Sequence**:
```cpp
// STEP 4: Initialize Settings Manager (persistent storage)
SettingsManager::begin();

// STEP 5: Initialize Theme System with saved preferences  
ThemeManager::begin();          // Initialize with default
ThemeManager::loadFromSettings(); // Load saved theme preference
```

**Boot Process**:
1. **Settings Manager Init**: Opens NVS namespace, checks for existing settings
2. **Theme Manager Init**: Sets up theme registry, loads default theme
3. **Load Saved Theme**: Retrieves saved theme index from NVS, applies theme
4. **Screen Creation**: All screens use the loaded theme automatically
5. **Normal Operation**: Theme persists across all UI interactions

## 📱 User Experience

### Theme Selection Workflow

1. **Access**: Main Menu → Settings → Themes
2. **Browse**: Use A/B buttons to navigate theme list
   - **Immediate Preview**: Each theme applies instantly for preview
   - **Visual Feedback**: UI updates immediately with new colors
3. **Select**: Press C to permanently save the theme
   - **Persistence**: Theme saved to ESP32 NVS storage
   - **Confirmation**: Serial log confirms save operation
4. **Return**: Long press any button to return to settings
   - **Persistence**: Selected theme remains active across reboots

### Visual Layout

```
┌─────────────────────────────┐
│           Themes            │ ← Clean title
├─────────────────────────────┤
│ > Default                   │ ← Current selection (3 visible)
│   High Contrast             │ ← Live preview as you navigate
│   Terminal                  │ ← Scroll down to see more...
└─────────────────────────────┘

Scroll down to view:
│   Amber                     │ ← 4th theme  
│   Sentry                    │ ← 5th theme (purple)
```

## 🔧 Technical Specifications

### Memory Usage
- **SettingsManager**: ~200 bytes (ESP32 Preferences overhead)
- **ThemeManager Arrays**: ~64 bytes (4 themes × 16 bytes metadata)
- **ThemeSelectionScreen**: ~150 bytes (MenuContainer + minimal state)
- **Total Added**: ~414 bytes

### Storage Specifications
- **Technology**: ESP32 NVS (Non-Volatile Storage)
- **Namespace**: `"alerttx1"` (project-specific isolation)
- **Key**: `"theme_idx"` (theme index storage 0-4)
- **Data Type**: 32-bit integer (4 bytes)
- **Persistence**: Survives power cycles, flash writes, factory resets
- **Wear Leveling**: Automatic (handled by ESP32 NVS)

### Error Handling
- **Missing Settings**: Defaults to theme index 0 (Default theme)
- **Corrupted Values**: Validates range (0-4), falls back to default
- **NVS Failures**: Continues operation without persistence, logs errors
- **Invalid Indices**: Bounds checking in all theme operations

### Debug Features
- **Serial Logging**: Detailed logs for all theme operations
- **NVS Debug Info**: `SettingsManager::printDebugInfo()` for troubleshooting
- **State Tracking**: Current theme, saved theme, and preview state logged

## 🧪 Testing & Validation

### Manual Testing Checklist

**Basic Functionality**:
- [ ] Navigate to Settings → Themes opens ThemeSelectionScreen
- [ ] A/B buttons navigate through 4 theme options
- [ ] Themes apply immediately for preview (colors change)
- [ ] C button saves selected theme permanently
- [ ] Long press returns to settings with theme persisted

**Persistence Testing**:
- [ ] Select non-default theme, reboot device → theme persists
- [ ] Change theme multiple times → latest selection persists
- [ ] First boot with empty NVS → defaults to Default theme

**Error Handling**:
- [ ] Invalid theme index in NVS → falls back to Default theme
- [ ] NVS corruption/failure → continues operation, logs error

**Memory/Performance**:
- [ ] Theme switching is immediate (< 100ms)
- [ ] No memory leaks after repeated theme changes
- [ ] Smooth navigation through theme list

## 🚀 Future Enhancements

### Planned Improvements
1. **Custom Themes**: User-defined color customization
2. **Theme Import/Export**: Share themes between devices
3. **Time-Based Themes**: Automatic day/night theme switching
4. **Theme Previews**: Small color swatches in theme list
5. **Animation**: Smooth color transitions when switching themes

### Extension Points
- **SettingsManager**: Add support for other settings (ringtone, brightness)
- **ThemeManager**: Dynamic theme loading from SD card or network
- **ThemeSelectionScreen**: Grid layout for more themes, categories
- **Color Customization**: RGB picker for creating custom themes

## 📚 Related Documentation

- **[UI Framework Overview](../development/ui-framework.md)**: Overall component architecture
- **[Navigation System](../development/navigation-system.md)**: Screen navigation details
- **[Build System](../development/build-system.md)**: Build and deployment

## 🏁 Summary

The Theme Selection System successfully implements a complete user customization experience with:

- ✅ **Real-time preview** of themes while browsing
- ✅ **Persistent storage** using ESP32 NVS technology  
- ✅ **Clean, simple interface** focused on usability
- ✅ **Robust error handling** with intelligent fallbacks
- ✅ **Memory efficient** implementation (~414 bytes total)
- ✅ **Professional-grade** persistence using official ESP32 libraries

This implementation provides a solid foundation for user customization and demonstrates how to properly integrate persistent storage in embedded Arduino projects using modern C++ patterns and ESP32 platform capabilities.

## 🆕 Latest Addition: Sentry Purple Theme

The **Sentry theme** (Index 4) brings company branding colors to the Alert TX-1:

### Visual Characteristics:
- **Background**: Dark purple (#1F1633) for sophisticated, branded appearance
- **Primary Text**: Pure white for maximum readability and contrast
- **Selected Text**: Bright yellow for high visibility on purple selections
- **Accent**: Sentry purple (#6A5FC1) for selections and highlights
- **Overall Feel**: Professional, branded, high-contrast interface

### Usage:
```cpp
// Access the Sentry theme
ThemeManager::setThemeByIndex(4, true);  // Apply and save Sentry theme
const char* name = ThemeManager::getThemeName(4);  // Returns "Sentry"
```

The Sentry theme is now fully integrated into the theme selection system with complete persistence, live preview, and scrolling support!
