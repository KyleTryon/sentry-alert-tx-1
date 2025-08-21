# Rendering Optimization Guide

## Overview

This document outlines the rendering optimization strategies implemented to fix performance issues causing ringtone slowdown and screen flickering in the Alert TX-1 project.

## Issues Identified

1. **CPU Blocking from Pixel-by-Pixel Drawing**
   - `AlertNotificationScreen::drawBackground()` was drawing 38,400 individual pixels per frame
   - This blocked the main loop, causing ringtone playback to slow down

2. **Excessive Redrawing**
   - Screens were redrawing entire content every frame regardless of changes
   - No dirty-flag tracking for conditional rendering

3. **Inconsistent Rendering Patterns**
   - Some screens (Theme, SystemInfo) used efficient redraw patterns
   - Others (Alerts, AlertNotification) redrew constantly

## Solutions Implemented

### 1. Efficient Background Drawing

**Before:**
```cpp
// Drawing 38,400 individual pixels!
for (int y = 0; y < DISPLAY_HEIGHT; y += 2) {
    for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
        display->drawPixel(x, y, darkBg);
        display->drawPixel(x + 1, y + 1, darkBg);
    }
}
```

**After:**
```cpp
// Drawing horizontal stripes - much faster
for (int y = 0; y < DISPLAY_HEIGHT; y += 4) {
    display->fillRect(0, y, DISPLAY_WIDTH, 2, darkBg);
}
```

### 2. Conditional Redrawing

Added `needsRedraw` flags to screens to only redraw when content changes:

```cpp
void draw() {
    if (!needsRedraw) return;
    needsRedraw = false;
    // ... draw content ...
}
```

### 3. Smart Update Tracking

For countdown timers, only redraw when the displayed second changes:

```cpp
unsigned long currentSecond = remaining / 1000;
if (currentSecond != lastCountdownSecond) {
    lastCountdownSecond = currentSecond;
    needsRedraw = true;
}
```

## Universal Rendering Pattern - Centralized System

The codebase now has a centralized rendering system that works for both component-based and direct-drawing screens:

### 1. For Component-Based Screens
Components already have built-in dirty tracking:
```cpp
// Components automatically track their own state
myComponent->markDirty();      // Mark for redraw
myComponent->isDirty();        // Check if needs redraw
myComponent->clearDirty();     // Clear after drawing
```

### 2. For Direct-Drawing Screens
Use the new draw region system in the Screen base class:
```cpp
class MyScreen : public Screen {
public:
    MyScreen(Adafruit_ST7789* display) : Screen(display, "MyScreen") {
        // Register draw regions
        addDrawRegion(DirectDrawRegion::STATIC, [this]() { 
            drawStaticContent(); 
        });
        addDrawRegion(DirectDrawRegion::DYNAMIC, [this]() { 
            drawDynamicContent(); 
        });
    }
    
    void update() override {
        Screen::update();
        
        // When data changes, mark the appropriate region dirty
        if (dataChanged()) {
            markDynamicContentDirty();
        }
    }
    
    void draw() override {
        // Base class handles everything!
        Screen::draw();
    }
    
private:
    void drawStaticContent() {
        // Draw headers, borders, labels - things that don't change
    }
    
    void drawDynamicContent() {
        // Draw values, selections, status - things that update
    }
};
```

### 3. Backwards Compatibility
The old pattern still works:
```cpp
// These methods are still available:
isStaticContentDrawn()
markStaticContentDrawn()
markDynamicContentDirty()
shouldRedrawDynamic()
clearDynamicRedrawFlag()
```

### 4. Efficient Drawing Operations
- Use `fillRect()` instead of pixel-by-pixel drawing
- Batch similar operations together
- Clear only the areas that need updating, not the entire screen
- Use the component system where possible (components track their own dirty state)

## Screen Categories

### Always Redraw (Games)
- Game screens that need constant animation
- Use frame rate limiting instead

### Redraw on Data Change (Most Screens)
- SystemInfoScreen - redraws when WiFi/battery status changes
- AlertsScreen - redraws when navigating or new messages arrive
- SettingsScreen - redraws when selection changes

### Component-Based (Recommended)
- ThemeSelectionScreen - uses MenuContainer components
- Components track their own dirty state
- Most efficient approach for complex UIs

## Performance Tips

1. **Avoid `drawPixel()` in loops** - Use `fillRect()` or `drawFastHLine()`/`drawFastVLine()`
2. **Track what actually changes** - Don't redraw if data hasn't changed
3. **Use dirty rectangles** - Only clear/redraw changed areas
4. **Batch operations** - Group similar drawing operations
5. **Profile with millis()** - Measure draw time to identify bottlenecks

## Implementation Status

All screens in the Alert TX-1 project have been converted to use the centralized rendering system:

### Converted Screens:
- ✅ AlertNotificationScreen - Fixed pixel-by-pixel drawing, added conditional rendering
- ✅ AlertsScreen - Header drawn once, list only when data changes
- ✅ HardwareTestScreen - Static labels/dynamic status separated
- ✅ MainMenuScreen - Title drawn once via static region
- ✅ SettingsScreen - Title drawn once via static region
- ✅ RingtonesScreen - Title drawn once via static region
- ✅ GamesScreen - Title drawn once via static region
- ✅ SystemInfoScreen - Labels/values separated, efficient updates
- ✅ SplashScreen - Single draw via static region

### Game Screens (Special Handling):
Game screens (Pong, Snake, BeeperHero) inherit from GameScreen base class which has its own optimized pattern:
- Frame rate limiting (configurable FPS)
- Static background caching
- Separate drawStatic() and drawGame() methods
- RenderBatch support for efficient drawing

## Testing Checklist

- [ ] Ringtones play at correct speed during screen transitions
- [ ] No visible flickering during normal use
- [ ] Smooth navigation between screens
- [ ] CPU usage remains low during idle states
- [ ] Battery life not impacted by excessive redraws
