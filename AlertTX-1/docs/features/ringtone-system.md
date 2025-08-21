# RTTTL Build System

## Overview

The Alert TX-1 project uses an automated build system to convert RTTTL ringtone files into embedded C++ data in **multiple optimized formats**. The system generates binary format for memory efficiency, text format for BeeperHero rhythm game compatibility, and specialized BeeperHero track data for precise gameplay timing.

## How It Works

### 1. RTTTL Files
RTTTL (Ring Tone Text Transfer Language) files are stored as plain text in `data/ringtones/`:
```
data/ringtones/
├── axelf.rtttl.txt         → "Axel F"
├── digimon.rtttl.txt       → "Digimon"
├── mario.rtttl.txt         → "Mario"
├── spiderman.rtttl.txt     → "Spiderman"
└── ... (16 total ringtones)
```

**Important**: Each RTTTL file must have the song name on the first line, followed by a colon and the RTTTL data. Example:
```
Mario:d=4,o=5,b=125:16e6,16e6,32p,8e6,16c6,8e6,8g6...
```

### 2. Automated Generation with Caching
The `tools/generate_ringtone_data.py` script:
- Scans the `data/ringtones/` directory
- **Checks cache** using SHA-256 file hashes
- **Only regenerates** when files have changed
- Reads all `.txt` files (if cache invalid)
- Extracts RTTTL names and data
- **Sorts ringtones alphabetically** by their display name
- **Generates three data formats**:
  - **Binary RTTTL**: Memory-efficient storage (50-70% savings)
  - **Text RTTTL**: Required for AnyRtttl non-blocking API
  - **BeeperHero Track Data**: Optimized binary format for rhythm game
- Creates `src/ringtones/ringtone_data.h` with embedded C++ arrays
- **Updates cache** for future builds

### 3. Compile-Time Integration
The generated header file contains:
- `const unsigned char` arrays for binary format ringtones
- `const char*` strings for text RTTTL data
- `const uint8_t` arrays for BeeperHero track data
- A unified registry mapping names to all data formats
- Helper functions for easy access to all data types

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

// Non-blocking audio (text format)
const char* textRTTTL = getTextRTTTL("MySong");
anyrtttl::nonblocking::begin(BUZZER_PIN, textRTTTL);

// Get ringtone info
int count = getRingtoneCount();
const char* name = getRingtoneName(0);
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

## Selection Persistence

Ringtone selection is persisted in ESP32 NVS via `SettingsManager` and previewed on selection:

```cpp
#include "src/config/SettingsManager.h"
#include "src/ringtones/RingtonePlayer.h"

// Save selection
SettingsManager::setRingtoneIndex(index);

// Load and optionally preview at startup
int ringIdx = SettingsManager::getRingtoneIndex();
ringtonePlayer.begin(BUZZER_PIN);
// Optional: ringtonePlayer.playRingtoneByIndex(ringIdx);
```

This mirrors theme persistence and survives reboots.

## Generated Code Structure

### Header File: `src/ringtones/ringtone_data.h`

```cpp
// Auto-generated - do not edit manually!

// Binary format arrays (memory efficient)
const unsigned char digimon_rtttl[] PROGMEM = {0x44, 0x69, 0x67, 0x69, ...};
const size_t digimon_rtttl_size = 77;

// Text format strings (for non-blocking audio)
const char digimon_text[] PROGMEM = "Digimon:d=4,o=5,b=120:c,c,g,g,...";

// BeeperHero track data (optimized for rhythm game)
const uint8_t digimon_track[] PROGMEM = {0x42, 0x50, 0x48, 0x52, ...};
const size_t digimon_track_size = 156;

// Unified registry
struct RingtoneEntry {
    const char* name;
    const unsigned char* binary_data;
    size_t binary_size;
    const char* text_data;
    const uint8_t* track_data;
    size_t track_size;
    const char* filename;
};
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
#         Generated 16 ringtones in 3 formats (binary, text, track)
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
#         Generated 17 ringtones in 3 formats (binary, text, track)
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
- **Multiple format optimization**:
  - **Binary format**: 50-70% memory savings for normal playback
  - **Text format**: Required for non-blocking audio (BeeperHero)
  - **Track format**: Ultra-compressed for rhythm game timing (80% savings)
- **Predictable timing**: No I/O delays during playback

### Reliability
- **No file system dependencies**: Works without SPIFFS/LittleFS
- **No file corruption issues**: Data is part of the firmware
- **Consistent behavior**: Same data every time
- **Format flexibility**: Best format automatically selected per use case

### Development Workflow
- **Easy to add ringtones**: Just drop files in `data/ringtones/`
- **Automatic updates**: Regeneration happens automatically
- **Intelligent caching**: Only regenerates when files change
- **Version control friendly**: RTTTL files are plain text
- **No manual conversion**: Script handles all format generation
- **BeeperHero ready**: Rhythm game data generated automatically

## Integration with System Components

### RingtonePlayer Integration

The `RingtonePlayer` class automatically includes the generated data:

```cpp
#include "ringtone_data.h"  // Auto-generated

class RingtonePlayer {
public:
    // Play by name (binary format - memory efficient)
    void playRingtoneByName(const char* name);
    
    // Play by index (binary format)
    void playRingtoneByIndex(int index);
    
    // Non-blocking playback (text format)
    void playRingtoneFromText(const char* name);
    
    // Get ringtone information
    int getRingtoneCount() const;
    const char* getRingtoneName(int index) const;
    int findRingtoneIndex(const char* name) const;
};
```

### BeeperHero Game Integration

The BeeperHero rhythm game uses specialized track data:

```cpp
#include "ringtone_data.h"
#include "src/games/beeperhero/BeeperHeroTrack.h"

class BeeperHeroScreen {
private:
    BeeperHeroTrack track;
    
public:
    void startGame() {
        // Load optimized track data
        const uint8_t* trackData = getBeeperHeroTrackData(selectedSongIndex);
        size_t trackSize = getBeeperHeroTrackSize(selectedSongIndex);
        track.loadFromMemory(trackData, trackSize);
        
        // Start non-blocking audio (text format required)
        const char* textRTTTL = getTextRTTTL(selectedSongIndex);
        anyrtttl::nonblocking::begin(BUZZER_PIN, textRTTTL);
    }
};
```

**BeeperHero Track Format Benefits**:
- **Ultra-compact**: 80% smaller than text RTTTL
- **Precise timing**: Millisecond-accurate note placement
- **Lane mapping**: Notes pre-assigned to 3 gameplay lanes
- **Fast loading**: Binary format loads instantly
- **Game-optimized**: Includes gameplay metadata (BPM, duration, note count)

## Example Usage

### Basic Playback
```cpp
#include "src/ringtones/RingtonePlayer.h"

RingtonePlayer player;
player.begin(BUZZER_PIN);

// Play by name (binary format - most efficient)
player.playRingtoneByName("Mario");
player.playRingtoneByName("Digimon");

// Play by index (binary format)
player.playRingtoneByIndex(0);  // First ringtone
player.playRingtoneByIndex(5);  // Sixth ringtone

// In main loop
player.update();
```

### BeeperHero Rhythm Game
```cpp
#include "src/ui/games/BeeperHeroScreen.h"

// Game automatically uses optimized track data
BeeperHeroScreen beeperHero(&display);

// All songs in ringtone library are automatically available
// Track data generated during build process
```

### Non-blocking Audio (Advanced)
```cpp
#include "ringtone_data.h"
#include <anyrtttl.h>

// For responsive UI during long playback
const char* textRTTTL = getTextRTTTL("Mario");
anyrtttl::nonblocking::begin(BUZZER_PIN, textRTTTL);

// In main loop
anyrtttl::nonblocking::play();
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