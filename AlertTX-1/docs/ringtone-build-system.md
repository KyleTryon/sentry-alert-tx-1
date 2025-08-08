# RTTTL Build System

## Overview

The Alert TX-1 project uses an automated build system to convert RTTTL ringtone files into embedded C++ data in binary format only. The binary format provides **50-70% memory savings** compared to text RTTTL, maximizing efficiency for embedded systems while maintaining full compatibility with the AnyRtttl library.

## How It Works

### 1. RTTTL Files
RTTTL (Ring Tone Text Transfer Language) files are stored as plain text in `data/ringtones/`:
```
data/ringtones/
├── digimon.rtttl.txt
├── mario.rtttl.txt
├── spiderman.rtttl.txt
└── ...
```

### 2. Automated Generation
The `tools/generate_ringtone_data.py` script:
- Scans the `data/ringtones/` directory
- Reads all `.txt` files
- Extracts RTTTL names and data
- Generates `src/ringtones/ringtone_data.h` with embedded C++ arrays

### 3. Compile-Time Integration
The generated header file contains:
- `const unsigned char` arrays for binary format ringtones
- A registry mapping names to binary data
- Helper functions for easy access to binary data

## Usage

### Adding New Ringtones

1. **Add RTTTL file** to `data/ringtones/`:
   ```bash
   # Example: Add a new ringtone
   echo "MySong:d=4,o=5,b=100:c,c,g,g,a,a,g,f,f,e,e,d,d,c" > data/ringtones/my_song.rtttl.txt
   ```

2. **Regenerate data** (automatic with build):
   ```bash
   make ringtones
   # or
   python3 tools/generate_ringtone_data.py
   ```

3. **Use in code**:
   ```cpp
   // Play by name (binary format)
   ringtonePlayer.playRingtoneByName("MySong");
   
   // Play by index (binary format)
   ringtonePlayer.playRingtoneByIndex(15); // 16th ringtone
   
   // Get ringtone info
   int count = ringtonePlayer.getRingtoneCount();
   const char* name = ringtonePlayer.getRingtoneName(0);
   ```

### Build Commands

```bash
# Generate ringtone data only
make ringtones

# Build project (includes ringtone generation)
make build

# Upload to device (includes build)
make upload

# Clean generated files
make clean

# Show all commands
make help
```

## Generated Code Structure

### Header File: `src/ringtones/ringtone_data.h`

```cpp
// Auto-generated - do not edit manually!

// Individual ringtone arrays (binary format only)
const unsigned char digimon_rtttl[] = {0x44, 0x69, 0x67, 0x69, ...};
const int digimon_rtttl_length = 77;

const unsigned char mario_rtttl[] = {0x4D, 0x61, 0x72, 0x69, ...};
const int mario_rtttl_length = 137;

// Registry for easy lookup
struct RingtoneEntry {
    const char* name;
    const unsigned char* data;
    int length;
    const char* filename;
};

static const RingtoneEntry RINGTONE_REGISTRY[] = {
    {"Digimon", digimon_rtttl, digimon_rtttl_length, "digimon_rtttl"},
    {"Mario", mario_rtttl, mario_rtttl_length, "mario_rtttl"},
    // ... more ringtones
    {nullptr, nullptr, 0, nullptr}  // End marker
};

// Helper functions
inline const unsigned char* getRingtoneData(const char* name);
inline const unsigned char* getRingtoneData(int index);
inline int getRingtoneLength(const char* name);
inline int getRingtoneLength(int index);
inline const char* getRingtoneName(int index);
inline int findRingtoneIndex(const char* name);
```

## Benefits

### Performance
- **No file I/O**: Data embedded at compile time
- **Faster startup**: No need to read files from storage
- **Lower memory usage**: No file system overhead
- **Binary format**: 50-70% memory savings over text RTTTL
- **Predictable timing**: No I/O delays during playback

### Reliability
- **No file system dependencies**: Works without SPIFFS/LittleFS
- **No file corruption issues**: Data is part of the firmware
- **Consistent behavior**: Same data every time

### Development Workflow
- **Easy to add ringtones**: Just drop files in `data/ringtones/`
- **Automatic updates**: Regeneration happens automatically
- **Version control friendly**: RTTTL files are plain text
- **No manual conversion**: Script handles all the work

## Integration with RingtonePlayer

The `RingtonePlayer` class automatically includes the generated data:

```cpp
#include "ringtone_data.h"  // Auto-generated

class RingtonePlayer {
public:
    // Play by name (from RTTTL file)
    void playRingtoneByName(const char* name);
    
    // Play by index
    void playRingtoneByIndex(int index);
    
    // Get ringtone information
    int getRingtoneCount() const;
    const char* getRingtoneName(int index) const;
    int findRingtoneIndex(const char* name) const;
};
```

## Example Usage

### Basic Playback
```cpp
#include "src/ringtones/RingtonePlayer.h"

RingtonePlayer player;
player.begin(BUZZER_PIN);

// Play by name (binary format)
player.playRingtoneByName("Mario");
player.playRingtoneByName("Digimon");

// Play by index (binary format)
player.playRingtoneByIndex(0);  // First ringtone
player.playRingtoneByIndex(5);  // Sixth ringtone

// In main loop
player.update();
```

### Ringtone Selection Screen
```cpp
void RingtonesScreen::draw() {
    // Show available ringtones
    int count = ringtonePlayer.getRingtoneCount();
    for (int i = 0; i < count; i++) {
        const char* name = ringtonePlayer.getRingtoneName(i);
        display.print(name);
    }
}

void RingtonesScreen::handleInput(Button button) {
    if (button == BUTTON_RIGHT) {
        // Play selected ringtone
        ringtonePlayer.playRingtoneByIndex(selectedIndex);
    }
}
```

### BeeperHero Game Integration
```cpp
void BeeperHeroGame::loadSong(const char* songName) {
    const char* rtttl = getRingtoneData(songName);
    if (rtttl) {
        // Load song for rhythm game
        parseRtttlForGame(rtttl);
    }
}
```

## File Naming Conventions

### RTTTL Files
- Use descriptive names: `mario.rtttl.txt`, `digimon.rtttl.txt`
- Avoid spaces and special characters
- Include `.rtttl.txt` extension for clarity

### Generated Identifiers
The script automatically converts filenames to valid C++ identifiers:
- `mario.rtttl.txt` → `mario_rtttl`
- `barbie_girl.txt` → `barbie_girl`
- `1000_miles.txt` → `ringtone_1000_miles` (starts with number)

## Troubleshooting

### Common Issues

1. **"Ringtone not found" error**
   - Check the ringtone name matches exactly
   - Use `getRingtoneName()` to see available names
   - Verify the RTTTL file was processed correctly

2. **Build errors after adding ringtones**
   - Run `make clean` then `make ringtones`
   - Check RTTTL syntax in the source file
   - Ensure file is readable

3. **Memory issues**
   - Monitor heap usage with `ESP.getFreeHeap()`
   - Consider removing unused ringtones
   - Check for memory leaks in playback code

### Debug Commands

```cpp
// List all available ringtones
void listRingtones() {
    int count = ringtonePlayer.getRingtoneCount();
    Serial.printf("Available ringtones (%d):\n", count);
    for (int i = 0; i < count; i++) {
        Serial.printf("  %d: %s\n", i, ringtonePlayer.getRingtoneName(i));
    }
}

// Test ringtone playback
void testRingtone(const char* name) {
    Serial.printf("Testing ringtone: %s\n", name);
    ringtonePlayer.playRingtoneByName(name);
}
```

## Future Enhancements

### Planned Features
1. **Binary RTTTL format**: Even more memory efficient
2. **Compression**: Reduce memory usage further
3. **Categories**: Organize ringtones by type/genre
4. **Metadata**: Store additional info (duration, difficulty, etc.)

### Community Contributions
- Add your favorite RTTTL files to `data/ringtones/`
- Share custom ringtone collections
- Contribute to the build system improvements

## Conclusion

The automated RTTTL build system provides:
- **Easy management** of ringtone files
- **Optimal performance** for embedded systems
- **Reliable operation** without file system dependencies
- **Developer-friendly workflow** for adding new content

This system transforms the Alert TX-1 into a capable audio platform while maintaining the simplicity of plain text RTTTL files for content creation. 