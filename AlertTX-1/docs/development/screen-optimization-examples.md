# Screen Optimization Examples

This guide shows how to convert existing screens to use the optimized rendering patterns to prevent flickering and improve performance.

## Example 1: Converting HardwareTestScreen

### Before (Redraws Every Frame)

```cpp
void HardwareTestScreen::draw() {
    Screen::draw();
    
    // This runs EVERY FRAME!
    drawTitle("Hardware Test");
    
    // Static content redrawn constantly
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    display->setCursor(10, 35);
    display->print("LED: A0 (GPIO18)");
    display->setCursor(10, 45);
    display->print("Buzzer: A4 (GPIO14)");
    
    // Dynamic status
    if (ledTestActive) {
        display->setTextColor(ST77XX_GREEN);
        display->setCursor(150, 35);
        display->print("ACTIVE!");
    }
}
```

### After (Optimized with Conditional Rendering)

```cpp
class HardwareTestScreen : public Screen {
private:
    // Track state changes
    bool lastLedTestActive = false;
    bool lastBuzzerTestActive = false;
    
    // Override update to detect changes
    void update() override {
        Screen::update();
        
        updateLEDTest();
        updateBuzzerTest();
        
        // Check if status changed
        if (ledTestActive != lastLedTestActive || 
            buzzerTestActive != lastBuzzerTestActive) {
            lastLedTestActive = ledTestActive;
            lastBuzzerTestActive = buzzerTestActive;
            markDynamicContentDirty();
        }
    }
    
    void draw() override {
        Screen::draw();
        
        // Draw static content only once
        if (!isStaticContentDrawn()) {
            drawStaticContent();
            markStaticContentDrawn();
        }
        
        // Draw dynamic content only when changed
        if (shouldRedrawDynamic()) {
            drawDynamicContent();
            clearDynamicRedrawFlag();
        }
    }
    
    void drawStaticContent() {
        // Title and labels that never change
        drawTitle("Hardware Test");
        
        display->setTextColor(ThemeManager::getPrimaryText());
        display->setTextSize(1);
        display->setCursor(10, 35);
        display->print("LED: A0 (GPIO18)");
        display->setCursor(10, 45);
        display->print("Buzzer: A4 (GPIO14)");
    }
    
    void drawDynamicContent() {
        // Clear status area
        display->fillRect(150, 35, 70, 25, ThemeManager::getBackground());
        
        // Draw current status
        if (ledTestActive) {
            display->setTextColor(ST77XX_GREEN);
            display->setCursor(150, 35);
            display->print("ACTIVE!");
        }
        
        if (buzzerTestActive) {
            display->setTextColor(ST77XX_YELLOW);
            display->setCursor(150, 45);
            display->print("ACTIVE!");
        }
    }
};
```

## Example 2: Menu-Based Screens

For screens using MenuContainer (like SettingsScreen, ThemeSelectionScreen):

```cpp
void draw() override {
    Screen::draw();
    
    // Draw title once
    if (!isStaticContentDrawn()) {
        DisplayUtils::drawTitle(display, "Settings");
        markStaticContentDrawn();
    }
    
    // Components handle their own dirty state
    // Screen::draw() already handles this efficiently
}
```

## Example 3: List Screens with Scrolling

For screens like AlertsScreen that show scrollable lists:

```cpp
class MyListScreen : public Screen {
private:
    int selectedIndex = 0;
    int lastSelectedIndex = -1;
    int scrollOffset = 0;
    int lastScrollOffset = -1;
    
    void update() override {
        Screen::update();
        
        // Check if visible content changed
        if (selectedIndex != lastSelectedIndex || 
            scrollOffset != lastScrollOffset) {
            lastSelectedIndex = selectedIndex;
            lastScrollOffset = scrollOffset;
            markDynamicContentDirty();
        }
    }
    
    void draw() override {
        Screen::draw();
        
        if (!isStaticContentDrawn()) {
            drawHeader();
            markStaticContentDrawn();
        }
        
        if (shouldRedrawDynamic()) {
            drawList();
            clearDynamicRedrawFlag();
        }
    }
};
```

## Example 4: Screens with Periodic Updates

For screens like SystemInfoScreen that update periodically:

```cpp
class MyInfoScreen : public Screen {
private:
    unsigned long lastUpdateTime = 0;
    static const unsigned long UPDATE_INTERVAL = 1000; // 1 second
    
    // Cached values
    String lastValue = "";
    String currentValue = "";
    
    void update() override {
        Screen::update();
        
        unsigned long now = millis();
        if (now - lastUpdateTime >= UPDATE_INTERVAL) {
            lastUpdateTime = now;
            
            // Get new value
            currentValue = getSomeValue();
            
            // Only redraw if changed
            if (currentValue != lastValue) {
                lastValue = currentValue;
                markDynamicContentDirty();
            }
        }
    }
};
```

## Common Patterns

### 1. Separate Static and Dynamic Content
- Static: Headers, labels, borders
- Dynamic: Values, status indicators, selections

### 2. Track State Changes
```cpp
private:
    // Current state
    int value;
    bool isActive;
    
    // Last drawn state
    int lastDrawnValue = -1;
    bool lastDrawnActive = false;
    
    // In update()
    if (value != lastDrawnValue || isActive != lastDrawnActive) {
        lastDrawnValue = value;
        lastDrawnActive = isActive;
        markDynamicContentDirty();
    }
```

### 3. Clear Before Redraw
When updating dynamic content, clear only the affected area:

```cpp
void drawDynamicContent() {
    // Clear only the value area, not the entire screen
    display->fillRect(valueX, valueY, valueWidth, valueHeight, 
                      ThemeManager::getBackground());
    
    // Draw new value
    display->setCursor(valueX, valueY);
    display->print(currentValue);
}
```

### 4. Use Component System
For complex UIs, use the component system where each component tracks its own dirty state:

```cpp
// In screen constructor
menuComponent = new MenuContainer(display, x, y);
addComponent(menuComponent);

// Components automatically handle their own redrawing
```

## Performance Guidelines

1. **Never redraw static content every frame**
2. **Track what actually changes between frames**
3. **Clear only the areas that need updating**
4. **Use `fillRect()` instead of pixel-by-pixel operations**
5. **Batch similar drawing operations together**

## Migration Checklist

When converting a screen:

- [ ] Add state tracking variables (last drawn values)
- [ ] Split draw() into drawStaticContent() and drawDynamicContent()
- [ ] Add change detection in update()
- [ ] Use Screen base class optimization helpers
- [ ] Test for flickering at different frame rates
- [ ] Verify CPU usage is reduced
