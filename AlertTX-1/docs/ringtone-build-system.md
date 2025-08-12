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

### 2. Automated Generation with Caching
The `tools/generate_ringtone_data.py` script:
- Scans the `data/ringtones/` directory
- **Checks cache** using SHA-256 file hashes
- **Only regenerates** when files have changed
- Reads all `.txt` files (if cache invalid)
- Extracts RTTTL names and data
- Generates `src/ringtones/ringtone_data.h` with embedded C++ arrays
- **Updates cache** for future builds

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

2. **Regenerate data** (automatic with build, uses caching):
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
# Generate ringtone data only (with caching)
make ringtones

# Build project (includes ringtone generation with caching)
make build

# Upload to device (includes build)
make upload

# Dev mode: upload and start monitor (recommended for development)
make dev

# Clean generated files and cache
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

## Caching System

### How Caching Works

The ringtone generation system includes intelligent caching to avoid unnecessary regeneration:

1. **Hash-Based Detection**: Each RTTTL file is hashed using SHA-256
2. **Cache Storage**: File hashes and metadata stored in `.ringtone_cache`
3. **Change Detection**: Only regenerates when files change or are added/removed
4. **Performance**: Dramatically faster rebuilds when no changes detected
5. **Relative Paths**: All paths stored as relative to project root for portability

### Cache Files

```
AlertTX-1/
├── .ringtone_cache          # Cache file with file hashes and metadata (ignored by git)
├── src/ringtones/
│   └── ringtone_data.h      # Generated header file (ignored by git)
└── data/ringtones/          # Source RTTTL files
    ├── mario.rtttl.txt
    ├── digimon.rtttl.txt
    └── ...
```

**Note**: Both `.ringtone_cache` and `src/ringtones/ringtone_data.h` are excluded from version control via `.gitignore` to keep the repository clean.

### Cache Structure

The `.ringtone_cache` file contains:
```json
{
  "file_hashes": {
    "mario.rtttl.txt": "a6bcc7651ef8aa9faf18a718604b9329e0af41b86e2136a89189fbcc41781813",
    "digimon.rtttl.txt": "97ef26bf0271ca20e0b5dfcff49bc2c9477d833cb850b40d7eed4fd41d012baf"
  },
  "ringtone_count": 14,
  "output_file": "src/ringtones/ringtone_data.h",
  "ringtone_dir": "data/ringtones",
  "timestamp": "2025-08-09T15:32:07.278030",
  "version": "1.0"
}
```

### Cache Behavior

```bash
# First run - generates cache and ringtone data
make ringtones
# Output: 🔧 Checking ringtone data...
#         🔄 Regenerating ringtone data (cache invalid or missing)
#         💾 Cache updated: .ringtone_cache

# Second run - uses cache (no changes)
make ringtones
# Output: 🔧 Checking ringtone data...
#         ✅ Cache is valid - no changes detected
#         Using cached ringtone data: src/ringtones/ringtone_data.h

# After adding new ringtone - detects changes
echo "NewSong:d=4,o=5,b=100:c,c,g,g" > data/ringtones/new_song.rtttl.txt
make ringtones
# Output: 🔧 Checking ringtone data...
#         New file detected: new_song.rtttl.txt
#         🔄 Changes detected in files: new_song.rtttl.txt
#         💾 Cache updated: .ringtone_cache
```

### Manual Cache Management

```bash
# Clear cache and force regeneration
make clean        # Removes cache and generated files
make ringtones    # Regenerates everything

# Or manually remove cache
rm .ringtone_cache
make ringtones    # Will regenerate due to missing cache
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