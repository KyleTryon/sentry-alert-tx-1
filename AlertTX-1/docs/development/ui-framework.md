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
  - Ringtones: selectable in Settings, preview on select, and persisted in NVS.

## Source layout

```
AlertTX-1/src/
├── config/
│   ├── DisplayConfig.h
│   └── settings.h
├── ui/
│   ├── core/            # Component, Screen, ScreenManager, Theme, DisplayUtils
│   ├── components/      # MenuContainer, MenuItem, Clickable
│   ├── screens/         # SplashScreen, MainMenuScreen, SettingsScreen, Alerts, etc.
│   └── games/           # BeeperHeroScreen, PongScreen, SnakeScreen (GameScreen-based)
```

## Controls

- Button A (GPIO0): Up/Previous
- Button B (GPIO1): Down/Next
- Button C (GPIO2): Select/Enter
- Long press any button (1500 ms): Back (pop screen)

### Input routing (centralized)

- Input is routed by a shared Input Router, not by individual screens.
- Behavior is consistent across all screens:
  - Long-press on any button triggers Back (pop screen) with debounce.
  - Select (C) is recognized on release as a short click (prevents accidental select after long-press).
  - Holding A/B auto-repeats navigation/movement after a short delay.
  - Menus auto-select the first item by default.

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

## Game framework (new)

- GameScreen (core): base class for games providing:
  - `setTargetFPS(int)`, frame gating in `update()`
  - Static vs dynamic drawing split via `drawStatic()` and `drawGame()`
- StandardGameLayout (core): shared header + play-area constants and helpers
  - `drawGameHeader()`, `drawPlayAreaBorder()`, `clearPlayArea()`
- All games now live under `src/ui/games/` and inherit `GameScreen`:
  - `BeeperHeroScreen`, `PongScreen`, `SnakeScreen`

### Cleanup hook (new)

- `Screen::cleanup()` is called from `Screen::exit()`.
- Use this to stop audio, free resources, and reset pointers.
- `ScreenManager::clearStack()` now deletes owned screens (and calls `exit()`), preventing leaks.

## Alerts (new)

- AlertsScreen: message list with read/unread indicators and 16px icons.
- New messages arrive via MQTT (JSON payload) and trigger the selected ringtone.
- Ringtone selection is managed in Settings and persisted via `SettingsManager`.

## See also

- Hardware setup: [Hardware Setup](../setup/hardware-setup.md)
- Display troubleshooting: [Display Troubleshooting](../setup/display-troubleshooting.md)
- Audio (AnyRTTTL) and ringtone build system: [AnyRTTTL Integration](anyrtttl-integration.md), [Ringtone System](../features/ringtone-system.md)
- Icon pipeline: [Icon System](../features/icon-system.md)
- Power management: [Power Management](../features/power-management.md)

