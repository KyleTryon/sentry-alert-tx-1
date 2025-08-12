# Alert TX-1 Documentation

This directory contains comprehensive documentation for the Alert TX-1 ESP32-S3 Reverse TFT Feather project.

## 📚 Documentation Index

### 🎨 UI Framework Documentation
- **[UI Framework Expansion Plan](UI_FRAMEWORK_EXPANSION_PLAN.md)** - Complete development plan and current status (Phase 3 ✅)
- **[Phase 1 Complete](PHASE1_IMPLEMENTATION_COMPLETE.md)** - Theme system and enhanced menu implementation
- **[Phase 2 Complete](PHASE2_IMPLEMENTATION_COMPLETE.md)** - Component-based architecture implementation
- **[Navigation System Complete](NAVIGATION_SYSTEM_COMPLETE.md)** - Full navigation system with screen hierarchy and long press back navigation
- **[UI Framework Guide](UI_FRAMEWORK_GUIDE.md)** - Simple UI system overview (legacy reference)

### 🔧 Hardware Documentation  
- **[Hardware Setup](hardware-setup.md)** - Board configuration and component wiring
- **[Display Troubleshooting](display-troubleshooting.md)** - ⚠️ **CRITICAL** ESP32-S3 TFT display initialization fix
- **[Pinout Reference](pinout-reference.md)** - Complete ESP32-S3 Reverse TFT Feather pinout
- **[Power Management](power-management.md)** - Battery and power optimization strategies

### 🎵 Audio System Documentation
- **[AnyRTTTL Integration](anyrtttl-integration.md)** - RTTTL ringtone playback implementation
- **[Ringtone Build System](ringtone-build-system.md)** - Converting RTTTL files to header format

### 🎨 Icon System Documentation
- **[Icon System Summary](ICON_SYSTEM_SUMMARY.md)** - PNG to RGB565 header conversion system

## 🚀 Quick Start

### For UI Framework Development
1. Study **[Phase 2 Complete](PHASE2_IMPLEMENTATION_COMPLETE.md)** for current architecture overview
2. Review **[UI Framework Expansion Plan](UI_FRAMEWORK_EXPANSION_PLAN.md)** for development roadmap
3. Study `AlertTX-1.ino` for a complete working example with component architecture

### For Hardware Setup
1. Follow **[Hardware Setup](hardware-setup.md)** for initial board configuration
2. Reference **[Pinout Reference](pinout-reference.md)** for GPIO assignments
3. Use **[Display Troubleshooting](display-troubleshooting.md)** if display issues occur

### For Audio Development
1. Review **[AnyRTTTL Integration](anyrtttl-integration.md)** for ringtone implementation
2. Use **[Ringtone Build System](ringtone-build-system.md)** to add custom ringtones

## 📊 Project Status

### ✅ Completed Components (Phase 2)
- **Component-Based UI Framework** - Modern architecture with Screen/Component system (100%)
- **Display Management** - Centralized DisplayConfig and DisplayUtils (100%)
- **Icon System** - 26 RGB565 icons with flexible sizing (16px/32px) (100%)
- **Hardware Integration** - ESP32-S3 button and display management (100%)
- **Theme System** - 4 predefined color themes with runtime switching (100%)
- **Screen Navigation** - ScreenManager with stack-based navigation (100%)
- **Splash Screen** - Professional boot experience with auto-transition (100%)

### 🔄 Available for Enhancement (Phase 3+)
- Additional screen implementations (Alerts, Settings, Games)
- Style system with layout properties (padding, alignment)
- More audio features and ringtone management
- Advanced animations and transitions

## 🎯 Architecture Overview

The Alert TX-1 uses a modern, component-based UI architecture:

```
AlertTX-1/
├── src/
│   ├── config/            # Hardware configuration and display specs
│   │   ├── DisplayConfig.h # Single source of truth for display dimensions
│   │   └── settings.h     # Hardware pin definitions
│   ├── ui/                # UI Framework
│   │   ├── core/          # Core system (Component, Screen, ScreenManager, Theme, DisplayUtils)
│   │   ├── components/    # Reusable UI components (MenuItem, MenuContainer, Clickable)
│   │   ├── screens/       # Screen implementations (MainMenuScreen, SplashScreen)
│   │   └── Menu.h/cpp     # Legacy simple menu (Phase 1)
│   ├── hardware/          # Hardware abstraction layer
│   ├── icons/             # RGB565 icon data files
│   └── ringtones/         # RTTTL audio data
└── docs/                  # This documentation
```

## 🎮 Controls Reference

### Navigation (Current Implementation)
- **Button A (GPIO0):** Navigate Up/Previous
- **Button B (GPIO1):** Navigate Down/Next  
- **Button C (GPIO2):** Select/Enter

### Theme Switching
- **Settings Menu Item:** Cycle through 4 available themes (Default, Terminal, Amber, High Contrast)

### Splash Screen
- **Any Button:** Skip 2-second splash screen and go directly to main menu

## 🔧 Development Guidelines

### Code Style
- Follow existing component patterns (Component -> Screen -> ScreenManager)
- Use ThemeManager for all colors to maintain consistency
- Use DisplayConfig constants for all screen dimensions
- Use DisplayUtils for text rendering and layout utilities
- Implement markDirty() for efficient screen updates
- Document all public APIs

### Performance Targets (Current)
- **Frame Rate:** 30+ FPS ✅
- **Response Time:** <50ms button response ✅
- **Memory Usage:** ~988KB program storage (68% of 1.4MB) ✅
- **Boot Time:** <2 seconds to splash screen ✅

### Architecture Principles
- **Single Source of Truth:** DisplayConfig.h for all display specifications
- **DRY Compliance:** DisplayUtils extends Adafruit_GFX functionality
- **Component-Based:** All UI elements inherit from Component base class
- **Theme-Driven:** All colors managed through ThemeManager

## 📖 Additional Resources

- **[Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)** - Core graphics library documentation
- **[ESP32-S3 Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)** - Official ESP32-S3 documentation
- **[Adafruit ESP32-S3 Reverse TFT Feather](https://learn.adafruit.com/adafruit-esp32-s3-reverse-tft-feather)** - Hardware guide

---

**📧 Need Help?** Check the Phase 2 Implementation Complete document for detailed API information, or study the working example in `AlertTX-1.ino`.
