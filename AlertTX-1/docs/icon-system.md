# Icon System Guide

The AlertTX-1 includes a complete icon system for converting PNG images to Arduino-compatible RGB565 header files. The system supports multiple icon sizes and provides efficient memory usage through PROGMEM storage.

## 🎨 Overview

### Features
- **PNG to RGB565 conversion** - Automatic color format conversion
- **Multiple sizes supported** - 16px, 32px, and custom dimensions
- **Memory efficient** - PROGMEM storage for flash memory usage
- **Build system integration** - Automatic conversion during compilation
- **Transparency support** - Transparent pixels become black
- **Cross-platform** - Works on macOS, Linux, and Windows

### Built-in Icons

The system includes 26+ professionally designed icons:

| Category | Icons | Size |
|----------|-------|------|
| **UI Navigation** | chevron-up, chevron-down, chevron-left, chevron-right | 16px |
| **Battery** | battery, battery-1, battery-2, battery-full, battery-charging | 16px |
| **Connectivity** | cellular-signal-0 through cellular-signal-3, cellular-signal-off | 16px |
| **Audio** | volume-plus, volume-minus, volume-x, play, pause-icon, music | 16px |
| **Communication** | mail, mail-unread, draft | 16px |
| **System** | alert, bug, sentry_logo | 16px, 32px |

## 🏗️ Icon Structure

### Icon Definition

Icons are defined using a simple struct:

```cpp
struct Icon {
    int x;        // X position for drawing
    int y;        // Y position for drawing  
    int w;        // Width in pixels
    int h;        // Height in pixels
    const uint16_t *data;  // RGB565 pixel data array
};
```

### Generated Header Format

Each icon generates a header file like:

```cpp
// alert_16.h (auto-generated)
#ifndef ALERT_16_H
#define ALERT_16_H

#include <Arduino.h>

// Alert icon - 16x16 pixels
const uint16_t alert_16[] PROGMEM = {
    0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,
    // ... pixel data continues
};

const int alert_16_width = 16;
const int alert_16_height = 16;

#endif
```

## 🛠️ Usage

### Drawing Icons

```cpp
#include "src/icons/alert_16.h"
#include "src/icons/sentry_logo_32.h"

void drawIcon() {
    // Draw 16x16 alert icon at position (10, 20)
    display.drawRGBBitmap(10, 20, alert_16, alert_16_width, alert_16_height);
    
    // Draw 32x32 logo at position (50, 50)
    display.drawRGBBitmap(50, 50, sentry_logo_32, 32, 32);
}
```

### Using Icon Registry

For dynamic icon access:

```cpp
#include "src/icons/icon_definitions.cpp"

// Get icon by name
const uint16_t* getIconData(const char* name, int* width, int* height) {
    if (strcmp(name, "alert") == 0) {
        *width = alert_16_width;
        *height = alert_16_height;
        return alert_16;
    }
    // Add other icons...
    return nullptr;
}
```

### Icon Helper Functions

```cpp
// Center an icon on screen
void drawCenteredIcon(const uint16_t* iconData, int iconWidth, int iconHeight) {
    int x = (SCREEN_WIDTH - iconWidth) / 2;
    int y = (SCREEN_HEIGHT - iconHeight) / 2;
    display.drawRGBBitmap(x, y, iconData, iconWidth, iconHeight);
}

// Draw icon with background
void drawIconWithBackground(int x, int y, const uint16_t* iconData, 
                           int iconWidth, int iconHeight, uint16_t bgColor) {
    display.fillRect(x, y, iconWidth, iconHeight, bgColor);
    display.drawRGBBitmap(x, y, iconData, iconWidth, iconHeight);
}
```

## ➕ Adding Custom Icons

### 1. Create PNG File

Create a PNG file with the desired dimensions and place it in `data/icons/`:

```bash
# Icon naming convention: name_SIZE.png
data/icons/my_icon_16.png
data/icons/custom_logo_32.png
```

### 2. Naming Convention

Follow the naming pattern: `name_SIZE.png`

- **name** - Descriptive icon name (lowercase, use hyphens for spaces)
- **SIZE** - Icon dimensions (16, 32, etc.)

Examples:
- `battery-full_16.png` → `battery_full_16.h`
- `sentry_logo_32.png` → `sentry_logo_32.h`
- `volume-plus_16.png` → `volume_plus_16.h`

### 3. Generate Headers

```bash
# Convert single icon
python3 tools/png_to_header.py data/icons/my_icon_16.png --output src/icons

# Convert all icons in directory
python3 tools/png_to_header.py data/icons --output src/icons

# Resize during conversion
python3 tools/png_to_header.py data/icons/large_icon.png --output src/icons --width 16

# Using Makefile
make icons
```

### 4. Use in Code

```cpp
#include "src/icons/my_icon_16.h"

void setup() {
    // Draw your custom icon
    display.drawRGBBitmap(0, 0, my_icon_16, my_icon_16_width, my_icon_16_height);
}
```

## 🔧 Build System Integration

### Makefile Commands

```bash
# Convert icons only
make icons

# Build project (includes icon conversion)
make build

# Clean generated icon headers
make clean

# Show all available commands
make help
```

### Automatic Integration

The build system automatically:
1. **Scans** `data/icons/` for PNG files
2. **Validates** naming convention and file format
3. **Converts** PNG to RGB565 format
4. **Generates** header files in `src/icons/`
5. **Updates** icon registry definitions

### Build Process

```
data/icons/alert_16.png → tools/png_to_header.py → src/icons/alert_16.h
                       ↓
                   RGB565 conversion
                       ↓
                   PROGMEM array generation
                       ↓
                   C header file creation
```

## 🎨 Design Guidelines

### Size Recommendations

- **16x16px** - UI elements, status indicators, menu icons
- **32x32px** - Logos, splash screen graphics, large buttons
- **Custom sizes** - Special use cases (maintain square aspect ratio when possible)

### Color Considerations

- **High contrast** - Icons should work with all themes
- **Simple designs** - Small pixel counts favor bold, simple shapes
- **Avoid gradients** - RGB565 format has limited color precision
- **Test transparency** - Transparent pixels become black (0x0000)

### Memory Usage

Estimate memory usage: `width × height × 2 bytes`

| Size | Memory Usage | Typical Use |
|------|--------------|-------------|
| 16x16px | 512 bytes | UI icons, status indicators |
| 32x32px | 2,048 bytes | Logos, splash graphics |
| 64x64px | 8,192 bytes | Large graphics (use sparingly) |

## 🔍 Troubleshooting

### Common Issues

**Icon not appearing:**
1. Check file naming convention (`name_SIZE.png`)
2. Verify header file was generated in `src/icons/`
3. Ensure `#include` statement is correct
4. Check display coordinates are within screen bounds

**Build errors:**
1. Verify Python 3 and Pillow library are installed
2. Check PNG file is valid and not corrupted
3. Ensure `data/icons/` directory exists
4. Verify sufficient disk space for generated headers

**Memory issues:**
1. Count total icon memory usage
2. Consider using smaller icon sizes
3. Remove unused icons from `data/icons/`
4. Use PROGMEM storage (handled automatically)

### Validation

```cpp
// Check if icon data is valid
bool validateIcon(const uint16_t* iconData, int width, int height) {
    if (iconData == nullptr) return false;
    if (width <= 0 || height <= 0) return false;
    if (width > SCREEN_WIDTH || height > SCREEN_HEIGHT) return false;
    return true;
}

// Safe icon drawing
void drawIconSafe(int x, int y, const uint16_t* iconData, int width, int height) {
    if (!validateIcon(iconData, width, height)) return;
    if (x + width > SCREEN_WIDTH || y + height > SCREEN_HEIGHT) return;
    
    display.drawRGBBitmap(x, y, iconData, width, height);
}
```

## 📊 Performance Considerations

### Memory Efficiency

- **PROGMEM storage** - Icons stored in flash memory, not RAM
- **On-demand loading** - Icons loaded only when drawn
- **Efficient formats** - RGB565 provides good quality at 2 bytes per pixel

### Rendering Performance

```cpp
// Fast icon drawing
void optimizedIconDraw() {
    // Pre-calculate positions
    const int iconX = (SCREEN_WIDTH - 16) / 2;
    const int iconY = (SCREEN_HEIGHT - 16) / 2;
    
    // Single draw call
    display.drawRGBBitmap(iconX, iconY, alert_16, 16, 16);
}
```

### Best Practices

1. **Batch icon operations** when possible
2. **Cache frequently used icons** in RAM if memory allows
3. **Use appropriate sizes** - don't scale icons in software
4. **Minimize draws** - redraw only when necessary

## 📚 Resources

### Tools and Libraries
- **[Pillow (PIL)](https://pillow.readthedocs.io/)** - Python image processing library
- **[GIMP](https://www.gimp.org/)** - Free image editor for creating icons
- **[Inkscape](https://inkscape.org/)** - Vector graphics editor (export to PNG)

### Icon Design Resources
- **[Feather Icons](https://feathericons.com/)** - Simple, beautiful open source icons
- **[Heroicons](https://heroicons.com/)** - Beautiful hand-crafted SVG icons
- **[Pixelarticons](https://pixelarticons.com/)** - Pixel-perfect icon set

The icon system provides a comprehensive solution for adding visual elements to your AlertTX-1 interface while maintaining optimal memory usage and performance.