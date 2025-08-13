# Alert TX-1 UI Framework Overview

## What you need to know

- The UI is a modern component-based system designed for clarity and tiny-memory environments.
- Key building blocks:
  - Component: base class for drawable UI elements.
  - MenuItem and MenuContainer: list-based navigation with selection.
  - Screen: a collection of components with lifecycle methods.
  - ScreenManager: stack-based navigation between screens.
  - Theme: centralized colors and styles; changeable at runtime.
  - DisplayConfig and DisplayUtils: single source of truth and helpers for sizing, text, and layout.

## Source layout

```
AlertTX-1/src/
├── config/
│   ├── DisplayConfig.h
│   └── settings.h
├── ui/
│   ├── core/            # Component, Screen, ScreenManager, Theme, DisplayUtils
│   ├── components/      # MenuContainer, MenuItem, Clickable
│   ├── screens/         # SplashScreen, MainMenuScreen, etc.
│   └── Menu.h/.cpp      # Legacy simple menu (Phase 1)
```

## Controls

- Button A (GPIO0): Up/Previous
- Button B (GPIO1): Down/Next
- Button C (GPIO2): Select/Enter
- Long press any button (1500 ms): Back (pop screen)

## Common tasks

- Switch theme at runtime:
```cpp
// Initialize once in setup()
ThemeManager::begin();
ThemeManager::setTheme(&THEME_DEFAULT); // or THEME_TERMINAL, THEME_AMBER, THEME_HIGH_CONTRAST
```

- Navigate between screens:
```cpp
// Use ScreenManager to push/pop
screenManager->pushScreen(alertsScreen);
screenManager->popScreen();
```

## Add a new screen (minimal pattern)

```cpp
// MyScreen.h
class MyScreen : public Screen {
public:
  MyScreen(Adafruit_ST7789* display) : Screen(display) {}
  void enter() override { Screen::enter(); /* create components, layout */ }
  void draw() override { /* draw components */ }
  void handleButtonPress(int button) override { /* route to components */ }
};
```

```cpp
// Usage
MyScreen* my = new MyScreen(&tft);
screenManager->pushScreen(my);
```

## Tips for contributors

- Use DisplayConfig constants rather than hardcoded dimensions.
- Prefer DisplayUtils for text/layout helpers.
- Keep components small, with clear bounds and markDirty() updates.
- Favor fixed-size arrays for predictable memory where practical.

## See also

- Hardware setup: docs/hardware-setup.md
- Display troubleshooting: docs/display-troubleshooting.md
- Audio (AnyRTTTL) and ringtone build system: docs/anyrtttl-integration.md, docs/ringtone-build-system.md
- Icon pipeline: docs/ICON_SYSTEM_SUMMARY.md
- Power management: docs/power-management.md
