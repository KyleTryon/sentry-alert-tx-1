# Icons

Icon library: https://pixelarticons.com/
Default Size: 16x16

- Website: https://pixelarticons.com
- Github: https://github.com/halfmage/pixelarticons
- NPM: https://www.npmjs.com/package/pixelarticons
- Figma: https://www.figma.com/community/file/952542622393317653

## Overview

This directory contains PNG icons that are automatically converted to Arduino-compatible header files. The build system converts these PNG files into RGB565 format and stores them as PROGMEM arrays for efficient memory usage on the ESP32.

**Important:** All icons must be exactly 16x16 pixels. The build system will error if any icon has different dimensions.

## Adding New Icons

### 1. Add PNG File
Place your PNG icon file in this directory (`AlertTX-1/data/icons/`). The file must be:
- PNG format
- **Exactly 16x16 pixels** (no other sizes are accepted)
- With transparency support (transparent areas will be converted to black)

### 2. Regenerate Headers
Run the icon conversion process:
```bash
make icons
```

This will:
- Convert all PNG files in `data/icons/` to header files in `src/icons/`
- **Verify all icons are 16x16 pixels**
- Handle transparency (transparent pixels become black)
- Convert to RGB565 format for efficient storage
- Generate proper C identifiers (hyphens become underscores)
- Add auto-generated comments to prevent manual modification

### 3. Use in Your Code
Include the generated header in your Arduino sketch:
```cpp
#include "icons/your_icon_name.h"
```

## Using Icons in Arduino Code

### Basic Usage
```cpp
#include "icons/alert.h"
#include "icons/warning_box.h"

void setup() {
    // The icon is now available as a global variable
    // alert - contains the alert icon
    // warning_box - contains the warning box icon
}

void drawIcon(const Icon& icon, int x, int y, int scale = 1) {
    // Draw the icon at position (x, y) with optional scaling
    // icon.data contains the RGB565 pixel data
    // icon.w and icon.h contain the dimensions (always 16x16)
    // scale: 1 = 16x16, 2 = 32x32, 3 = 48x48, etc.
}
```

### Icon Structure
Each generated header defines:
- `ICON_NAME_WIDTH` and `ICON_NAME_HEIGHT` - dimensions (always 16)
- `icon_name_data[]` - PROGMEM array with RGB565 pixel data
- `icon_name` - Icon struct instance

### Example: Drawing an Icon
```cpp
void drawIcon(const Icon& icon, int x, int y, int scale = 1) {
    for (int py = 0; py < icon.h; py++) {
        for (int px = 0; px < icon.w; px++) {
            uint16_t pixel = pgm_read_word(&icon.data[py * icon.w + px]);
            // Draw scaled pixel
            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    // Draw pixel at (x + (px * scale) + sx, y + (py * scale) + sy) with color 'pixel'
                }
            }
        }
    }
}

// Usage
drawIcon(alert, 10, 10);           // Draw alert icon at (10, 10) - 16x16
drawIcon(battery_full, 50, 10, 2); // Draw battery icon at (50, 10) - 32x32
drawIcon(play, 10, 50, 3);         // Draw play icon at (10, 50) - 48x48
```

## Build System Integration

### Automatic Conversion
The icon conversion happens automatically when you run:
- `make build` - Builds the project (includes icon conversion)
- `make all` - Default target (includes icon conversion)
- `make upload` - Uploads to device (includes icon conversion)

### Manual Conversion
Convert icons manually:
```bash
make icons
```

### Clean Generated Files
Remove all generated icon headers:
```bash
make clean
```

## File Naming Conventions

- PNG files can use hyphens (e.g., `warning-box.png`)
- Generated C identifiers use underscores (e.g., `warning_box`)
- Header guards use uppercase with underscores (e.g., `WARNING_BOX_H`)

## Scaling Support

Since all icons are standardized to 16x16 pixels, you can easily scale them at runtime:

```cpp
drawIcon(alert, 10, 10);           // 16x16 (1x scale)
drawIcon(alert, 10, 10, 2);        // 32x32 (2x scale)
drawIcon(alert, 10, 10, 3);        // 48x48 (3x scale)
drawIcon(alert, 10, 10, 4);        // 64x64 (4x scale)
```

This approach provides:
- **Consistency**: All icons have the same base size
- **Flexibility**: Scale to any size needed
- **Memory Efficiency**: Store only one size, render at any size
- **Performance**: Simple integer scaling is fast

## Memory Considerations

- Each 16x16 icon uses approximately 512 bytes of PROGMEM (16×16×2 bytes)
- Icons are only included in the firmware if their headers are `#include`d
- Use selective inclusion to minimize firmware size
- Scaling doesn't increase memory usage - it's done at render time

## Troubleshooting

### Icon Not Appearing
- Check that the PNG file is valid and not corrupted
- Ensure the header file was generated (`make icons`)
- Verify the `#include` statement uses the correct path

### Build Errors
- **Size Error**: Make sure all icons are exactly 16x16 pixels
- Make sure `src/icons/Icon.h` exists (created by the build system)
- Check that icon names don't conflict with other identifiers
- Verify that hyphens in filenames are properly converted to underscores

### Memory Issues
- Each 16x16 icon uses only 512 bytes of PROGMEM
- Use selective inclusion to only include needed icons
- Monitor firmware size with `arduino-cli compile --show-properties`

## Auto-Generated Files

All generated header files include a comment block indicating they are auto-generated:
```cpp
// AUTO-GENERATED FILE - DO NOT MODIFY
// Generated from: icon_name.png
// Size: 16x16 pixels
```

**Do not manually edit these files** - any changes will be overwritten when you run `make icons`.