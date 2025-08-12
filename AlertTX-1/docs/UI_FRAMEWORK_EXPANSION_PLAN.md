# Alert TX-1 UI Framework Expansion Plan

## Overview

This document outlines the planned expansion of the Alert TX-1 UI framework while maintaining its core principles of simplicity, memory efficiency, and reliability. The goal is to evolve the current working system into a more flexible and visually appealing framework.

## 🎯 Current State Analysis

### What We Have (Working)
```
AlertTX-1/src/ui/
├── Menu.h/cpp          # Simple menu with hardcoded styling
└── (MenuItem struct)   # Basic menu item definition
```

**Current Capabilities:**
- ✅ Basic navigation (up/down/select)
- ✅ Text-based menu with ">" indicator
- ✅ Simple callback system
- ✅ Fixed positioning and colors
- ✅ Memory efficient (~2KB total)

**Current Limitations:**
- ❌ Hardcoded colors and positioning
- ❌ Fixed menu item size (15px height)
- ❌ No screen abstraction
- ❌ No component reusability
- ❌ No theme system

---

## 🚀 Expansion Goals

### Phase 1: Theme System & Enhanced Menu
1. **Theme Support** - Centralized color management
2. **Larger Menu Items** - Full width, ~30px height items
3. **Enhanced Visual Design** - Better spacing and alignment

### Phase 2: Component Architecture  
1. **Component Separation** - Break Menu and MenuItem apart
2. **Screen Abstraction** - Navigation between different screens
3. **Clickable Component** - Abstract button-like behavior

### Phase 3: Style System (Optional)
1. **Layout Properties** - Padding, margins, alignment
2. **Responsive Sizing** - Percentage-based widths/heights
3. **Text Styling** - Alignment, size, weight properties

---

## 📋 Phase 1: Theme System & Enhanced Menu

### 1.1 Theme System Design

**Core Theme Structure:**
```cpp
struct Theme {
    // Background colors
    uint16_t background;        // Main background (ST77XX_BLACK)
    uint16_t surfaceBackground; // Component backgrounds
    
    // Text colors  
    uint16_t primaryText;       // Main text (ST77XX_WHITE)
    uint16_t secondaryText;     // Dimmed text (ST77XX_GRAY)
    uint16_t selectedText;      // Selected text (ST77XX_BLACK)
    
    // Accent colors
    uint16_t accent;            // Selection/highlight (ST77XX_YELLOW)
    uint16_t accentDark;        // Pressed state
    uint16_t border;            // Borders and dividers
};
```

**Predefined Themes:**
```cpp
// Default FlipperZero-inspired theme
extern const Theme THEME_DEFAULT;

// High contrast theme
extern const Theme THEME_HIGH_CONTRAST;

// Green terminal theme  
extern const Theme THEME_TERMINAL;

// Amber CRT theme
extern const Theme THEME_AMBER;
```

**Memory Efficiency:**
- Themes stored in PROGMEM (read-only)
- Single active theme pointer (8 bytes)
- No theme copying, only references

### 1.2 Enhanced Menu Design

**New Menu Specifications:**
```cpp
class Menu {
private:
    // Layout (full width, larger items)
    static const int MENU_PADDING = 10;      // Edge padding
    static const int ITEM_HEIGHT = 30;       // Double current height
    static const int ITEM_SPACING = 2;       // Space between items
    
    // Full width calculation: 240px - (2 * 10px padding) = 220px
    int menuWidth;  // Calculated based on screen width
    
    // Theme support
    const Theme* theme;
    
public:
    Menu(Adafruit_ST7789* display, const Theme* theme);
    void setTheme(const Theme* newTheme);
    
    // Enhanced drawing with theme colors
    void draw();
};
```

**Visual Improvements:**
- **Full Width Items:** 220px wide (240px screen - 20px padding)
- **Larger Touch Targets:** 30px height vs current 15px
- **Better Spacing:** 2px between items for clarity
- **Theme Integration:** All colors from theme system

---

## 📋 Phase 2: Component Architecture

### 2.1 Component Separation

**New File Structure:**
```
AlertTX-1/src/ui/
├── core/
│   ├── Theme.h             # Theme definitions and management
│   ├── Component.h         # Base component class
│   └── Screen.h            # Screen base class
├── components/
│   ├── Menu.h/cpp          # Menu container component
│   ├── MenuItem.h/cpp      # Individual menu item component
│   └── Clickable.h         # Abstract clickable behavior
└── screens/
    ├── MainMenuScreen.h/cpp    # Main menu screen
    ├── AlertsScreen.h/cpp      # Alerts screen  
    └── SettingsScreen.h/cpp    # Settings screen
```

### 2.2 Base Component Design

**Component Base Class:**
```cpp
class Component {
protected:
    Adafruit_ST7789* display;
    const Theme* theme;
    
    // Layout properties
    int x, y, width, height;
    bool visible = true;
    bool needsRedraw = true;
    
public:
    Component(Adafruit_ST7789* display, const Theme* theme);
    virtual ~Component() = default;
    
    // Core interface
    virtual void draw() = 0;
    virtual void update() {}  // Optional update logic
    
    // Layout management
    void setBounds(int x, int y, int w, int h);
    void setPosition(int x, int y);
    void setSize(int w, int h);
    void setVisible(bool visible);
    
    // Theme management
    void setTheme(const Theme* newTheme);
    
    // Dirty tracking for efficiency
    void markDirty() { needsRedraw = true; }
    bool isDirty() const { return needsRedraw; }
    void clearDirty() { needsRedraw = false; }
};
```

### 2.3 MenuItem Component

**Individual Menu Item:**
```cpp
class MenuItem : public Component {
private:
    const char* label;
    int id;
    bool selected = false;
    
    // Clickable behavior
    std::function<void()> onSelect;  // Modern C++ callback
    
public:
    MenuItem(Adafruit_ST7789* display, const Theme* theme, 
             const char* label, int id);
    
    // Component interface
    void draw() override;
    
    // MenuItem specific
    void setLabel(const char* newLabel);
    void setSelected(bool isSelected);
    void setOnSelect(std::function<void()> callback);
    
    // Interaction
    void select() { if (onSelect) onSelect(); }
    
    // Getters
    const char* getLabel() const { return label; }
    int getId() const { return id; }
    bool isSelected() const { return selected; }
};
```

### 2.4 Enhanced Menu Container

**Menu as Component Container:**
```cpp
class Menu : public Component {
private:
    std::vector<MenuItem*> items;  // Or fixed array for memory efficiency
    int selectedIndex = 0;
    int maxItems;  // Memory limit
    
public:
    Menu(Adafruit_ST7789* display, const Theme* theme, int maxItems = 8);
    ~Menu();
    
    // Component interface
    void draw() override;
    void update() override;
    
    // Menu management
    void addItem(const char* label, int id, std::function<void()> callback);
    void removeItem(int index);
    void clear();
    
    // Navigation
    void moveUp();
    void moveDown();
    void select();
    
    // State
    int getSelectedIndex() const { return selectedIndex; }
    MenuItem* getSelectedItem() const;
    int getItemCount() const { return items.size(); }
};
```

### 2.5 Screen Abstraction

**Screen Base Class:**
```cpp
class Screen {
protected:
    Adafruit_ST7789* display;
    const Theme* theme;
    std::vector<Component*> components;  // Or fixed array
    bool active = false;
    
public:
    Screen(Adafruit_ST7789* display, const Theme* theme);
    virtual ~Screen();
    
    // Screen lifecycle
    virtual void enter() { active = true; }   // Called when screen becomes active
    virtual void exit() { active = false; }   // Called when leaving screen
    virtual void update();                     // Update all components
    virtual void draw();                       // Draw all components
    
    // Input handling
    virtual void handleButtonPress(int button) = 0;
    
    // Component management
    void addComponent(Component* component);
    void removeComponent(Component* component);
    void clearComponents();
    
    // Theme management
    void setTheme(const Theme* newTheme);
    
    // State
    bool isActive() const { return active; }
};
```

### 2.6 Clickable Behavior Abstraction

**Clickable Interface:**
```cpp
class Clickable {
public:
    virtual ~Clickable() = default;
    virtual void onClick() = 0;
    virtual void onPress() {}    // Optional press feedback
    virtual void onRelease() {}  // Optional release feedback
};

// Can be mixed into components that need click behavior
class ClickableMenuItem : public MenuItem, public Clickable {
public:
    void onClick() override { select(); }
    void onPress() override { /* Visual press feedback */ }
};
```

---

## 📋 Phase 3: Style System (Optional Advanced Feature)

### 3.1 Layout Properties

**Style Structure:**
```cpp
struct Style {
    // Spacing
    uint8_t paddingLeft = 0;
    uint8_t paddingRight = 0; 
    uint8_t paddingTop = 0;
    uint8_t paddingBottom = 0;
    
    uint8_t marginLeft = 0;
    uint8_t marginRight = 0;
    uint8_t marginTop = 0;
    uint8_t marginBottom = 0;
    
    // Sizing (0-100 for percentage, >100 for absolute pixels)
    uint8_t width = 100;   // 100 = 100% width
    uint8_t height = 0;    // 0 = auto height
    
    // Text alignment
    enum TextAlign { LEFT, CENTER, RIGHT } textAlign = LEFT;
    enum VerticalAlign { TOP, MIDDLE, BOTTOM } verticalAlign = MIDDLE;
    
    // Text properties
    uint8_t textSize = 1;
    bool textBold = false;
};
```

### 3.2 Style Application

**Component with Style:**
```cpp
class StyledComponent : public Component {
protected:
    Style style;
    
    // Calculate actual dimensions from style
    int getContentX() const { return x + style.paddingLeft; }
    int getContentY() const { return y + style.paddingTop; }
    int getContentWidth() const;   // Accounts for padding and % width
    int getContentHeight() const;  // Accounts for padding and % height
    
public:
    void setStyle(const Style& newStyle) { 
        style = newStyle; 
        markDirty(); 
    }
    
    // Style helpers
    void setPadding(uint8_t padding) { 
        style.paddingLeft = style.paddingRight = 
        style.paddingTop = style.paddingBottom = padding;
        markDirty();
    }
    
    void setTextAlign(Style::TextAlign align) {
        style.textAlign = align;
        markDirty();
    }
};
```

---

## 🔧 Implementation Strategy

### Memory Efficiency Considerations

1. **Theme Propagation:**
   ```cpp
   // Option 1: Pass theme pointer down (8 bytes per component)
   Component(Adafruit_ST7789* display, const Theme* theme);
   
   // Option 2: Global theme singleton (most memory efficient)
   class ThemeManager {
       static const Theme* currentTheme;
   public:
       static const Theme* getTheme() { return currentTheme; }
       static void setTheme(const Theme* theme) { currentTheme = theme; }
   };
   ```

2. **Component Storage:**
   ```cpp
   // Fixed arrays for predictable memory usage
   static const int MAX_MENU_ITEMS = 8;
   static const int MAX_SCREEN_COMPONENTS = 10;
   
   // Use placement new for efficiency
   MenuItem menuItemStorage[MAX_MENU_ITEMS];
   Component* screenComponents[MAX_SCREEN_COMPONENTS];
   ```

3. **Screen Management:**
   ```cpp
   class ScreenManager {
       Screen* screens[8];  // Fixed number of screens
       Screen* currentScreen = nullptr;
       int screenCount = 0;
       
   public:
       void navigateTo(Screen* screen);
       void goBack();  // Simple stack for navigation
   };
   ```

### Performance Targets

- **Memory Usage:** <128KB total framework (current ~2KB)
- **Rendering:** Maintain 30+ FPS with larger components
- **Response Time:** <50ms button to visual feedback
- **Component Limit:** Max 10 components per screen for memory control

---

## 🎨 Visual Design Evolution

### Current Design
```
┌─────────────────────────────────────┐
│  AlertTX-1                          │  ← Title area
├─────────────────────────────────────┤
│   > Alerts                          │  ← 15px height items
│     Games                           │  
│     Settings                        │  ← 120px width
└─────────────────────────────────────┘
```

### Proposed Design
```
┌─────────────────────────────────────┐
│  AlertTX-1                          │  ← Title area (themed)
├─────────────────────────────────────┤
│ ┌─────────────────────────────────┐ │  ← 10px padding
│ │ > Alerts                        │ │  ← 30px height items  
│ └─────────────────────────────────┘ │  ← Full width (220px)
│                                     │  ← 2px spacing
│ ┌─────────────────────────────────┐ │
│ │   Games                         │ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │   Settings                      │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

---

## 🎯 Implementation Phases

### Phase 1: Foundation (Week 1)
- [ ] Create Theme system with 4 predefined themes
- [ ] Enhance Menu with full-width, larger items
- [ ] Implement theme color integration
- [ ] Test memory usage and performance

### Phase 2: Components (Week 2)  
- [ ] Create Component base class
- [ ] Separate MenuItem as individual component
- [ ] Build Screen abstraction system
- [ ] Implement ScreenManager for navigation

### Phase 3: Integration (Week 3)
- [ ] Convert existing menu to new component system
- [ ] Create MainMenuScreen, AlertsScreen, SettingsScreen
- [ ] Implement screen transitions
- [ ] Add Clickable behavior abstraction

### Phase 4: Polish (Week 4)
- [ ] Performance optimization
- [ ] Memory usage analysis
- [ ] Visual refinements
- [ ] Documentation updates

### Phase 5: Style System (Optional)
- [ ] Implement basic Style struct
- [ ] Add padding and margin support
- [ ] Implement percentage-based sizing
- [ ] Add text alignment properties

---

## 🔍 Technical Considerations

### Screen Size Constraints
- **Display:** 240x135 pixels
- **Menu Items:** ~4-5 items visible at 30px height
- **Padding:** 10px edges for comfortable touch targets
- **Text:** Size 1 (8px) for optimal readability

### Memory Budget
- **Current Usage:** ~2KB for UI
- **Target Usage:** <64KB for expanded framework  
- **Component Limit:** Max 50 total components across all screens
- **Theme Storage:** <1KB for all theme definitions

### Performance Requirements
- **Frame Rate:** Maintain 30+ FPS with new components
- **Memory Allocation:** Minimize dynamic allocation
- **Button Response:** <50ms from press to visual update
- **Screen Transitions:** <200ms transition time

---

This plan provides a clear roadmap for evolving the current simple and working UI framework into a more sophisticated system while maintaining the core principles of simplicity, reliability, and memory efficiency that make the current system successful.
