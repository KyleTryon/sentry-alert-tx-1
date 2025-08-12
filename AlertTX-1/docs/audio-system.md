# Audio System Guide

The AlertTX-1 features a complete audio system for playing RTTTL ringtones and providing audio feedback. The system uses the AnyRtttl library for professional-grade non-blocking playback and includes an automated build system for managing ringtone data.

## 🎵 Overview

### Key Features
- **Non-blocking playback** - Music plays without blocking the UI
- **Memory efficient** - Binary format saves 50-70% memory vs. text RTTTL
- **16+ built-in ringtones** - Popular themes and classics
- **Easy expansion** - Add new ringtones by dropping text files
- **Volume control** - Adjustable volume and mute functionality
- **Professional quality** - Precise timing for rhythm games

### Hardware Requirements
- **Passive Piezo Buzzer** (9025 or similar)
- **GPIO Pin 15** (BUZZER_PIN) - configurable in settings.h
- **External power** recommended for louder volume

## 🔧 Setup

### Hardware Connection
```
ESP32-S3 Feather    Piezo Buzzer
─────────────────    ────────────
GPIO 15 (D15) ──────── + (Positive)
GND ────────────────── - (Negative/Ground)
```

### Software Integration
```cpp
#include "src/ringtones/RingtonePlayer.h"

RingtonePlayer ringtonePlayer;

void setup() {
    ringtonePlayer.begin();
}

void loop() {
    ringtonePlayer.update();  // Must call in main loop for non-blocking playback
}
```

## 🎼 Built-in Ringtones

The system includes 16+ popular ringtones:

| Name | Description | Memory Usage |
|------|-------------|--------------|
| mario | Super Mario Bros Theme | ~137 bytes |
| zelda | Legend of Zelda Theme | ~95 bytes |
| spiderman | Spider-Man Theme | ~110 bytes |
| take_on_me | a-ha - Take On Me | ~180 bytes |
| rick_roll | Never Gonna Give You Up | ~165 bytes |
| axelf | Axel F (Beverly Hills Cop) | ~120 bytes |
| digimon | Digimon Theme | ~77 bytes |
| kim_possible_pager | Kim Possible | ~85 bytes |
| futurama | Futurama Theme | ~95 bytes |
| king_of_the_hill | King of the Hill | ~88 bytes |
| the_simpsons | The Simpsons | ~92 bytes |
| eiffel_65_blue | Blue (Da Ba Dee) | ~145 bytes |
| barbie_girl | Aqua - Barbie Girl | ~155 bytes |
| sk8erboy | Avril Lavigne - Sk8er Boi | ~140 bytes |
| 1000_miles | Vanessa Carlton | ~175 bytes |
| desk_phone | Classic Desk Phone | ~45 bytes |

## 🎮 Basic Usage

### Playing Ringtones

```cpp
// Play by name
ringtonePlayer.playRingtoneByName("mario");
ringtonePlayer.playRingtoneByName("zelda");

// Play by index (0-based)
ringtonePlayer.playRingtoneByIndex(0);  // First ringtone

// Check if playing
if (ringtonePlayer.isPlaying()) {
    Serial.println("Music is playing...");
}

// Stop playback
ringtonePlayer.stop();
```

### Volume and Control

```cpp
// Set volume (0-100%)
ringtonePlayer.setVolume(75);

// Mute/unmute
ringtonePlayer.mute();
ringtonePlayer.unmute();

// Check status
bool isMuted = ringtonePlayer.isMuted();
int currentVolume = ringtonePlayer.getVolume();
```

### Getting Ringtone Information

```cpp
// Get total count
int totalRingtones = ringtonePlayer.getRingtoneCount();

// Get ringtone names
for (int i = 0; i < totalRingtones; i++) {
    const char* name = ringtonePlayer.getRingtoneName(i);
    Serial.println(name);
}

// Find ringtone by name
int index = ringtonePlayer.findRingtoneIndex("mario");
if (index >= 0) {
    ringtonePlayer.playRingtoneByIndex(index);
}
```

## 🛠️ Adding Custom Ringtones

### 1. Create RTTTL File

Create a new text file in `data/ringtones/` with `.rtttl.txt` extension:

```bash
# Example: Add a custom ringtone
echo "MyTune:d=4,o=5,b=120:c,d,e,f,g,a,b,c6" > data/ringtones/my_tune.rtttl.txt
```

### 2. RTTTL Format

RTTTL format: `name:settings:notes`

```
MySong:d=4,o=5,b=100:c,c,g,g,a,a,g,f,f,e,e,d,d,c
```

**Settings:**
- `d=4` - Default duration (4 = quarter note)
- `o=5` - Default octave (4-7)
- `b=100` - Beats per minute (tempo)

**Notes:**
- `c,d,e,f,g,a,b` - Note names
- `c#,d#,f#,g#,a#` - Sharp notes
- `2c,4d,8e` - Duration prefixes (1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth)
- `c6,d5` - Octave suffixes
- `p` - Pause/rest

### 3. Build and Use

```bash
# Regenerate ringtone data (with caching)
make ringtones

# Build and upload
make upload

# Use in code
ringtonePlayer.playRingtoneByName("MySong");
```

## 🏗️ Build System

### Automatic Processing

The build system automatically:
1. **Scans** `data/ringtones/` for `.rtttl.txt` files
2. **Checks cache** using file hashes (only rebuilds when changed)
3. **Converts** RTTTL to memory-efficient binary format
4. **Generates** `src/ringtones/ringtone_data.h` with embedded arrays
5. **Updates cache** for future builds

### Build Commands

```bash
# Generate ringtone data only
make ringtones

# Full build (includes ringtones)
make build

# Upload with build
make upload

# Development mode (upload + monitor)
make dev

# Clean all generated files
make clean
```

### Generated Code Structure

The build system creates:

```cpp
// src/ringtones/ringtone_data.h (auto-generated)

// Binary data arrays
const unsigned char mario_rtttl[] = {0x4D, 0x61, 0x72, 0x69, ...};
const int mario_rtttl_length = 137;

// Registry for easy access
const RingtoneEntry RINGTONE_REGISTRY[] = {
    {"mario", mario_rtttl, mario_rtttl_length},
    {"zelda", zelda_rtttl, zelda_rtttl_length},
    // ...
};

const int RINGTONE_COUNT = 16;
```

## 🎯 Advanced Features

### Non-blocking Architecture

The audio system is designed for responsive performance:

```cpp
void loop() {
    // Essential: Update audio system
    ringtonePlayer.update();
    
    // UI remains responsive
    handleButtons();
    updateDisplay();
    
    // Other game logic
    updateGame();
}
```

### Rhythm Game Integration

For games requiring note timing:

```cpp
// Get current note information
if (ringtonePlayer.isPlaying()) {
    int currentFrequency = ringtonePlayer.getCurrentFrequency();
    int noteDuration = ringtonePlayer.getCurrentNoteDuration();
    
    // Use for rhythm game mechanics
    spawnNoteAtFrequency(currentFrequency);
}
```

### Memory Optimization

The binary format provides significant memory savings:

```
Text RTTTL:   "mario:d=4,o=5,b=100:e,e,p,e,p,c,e,p,g,p,p,p,g"  (50+ bytes)
Binary Format: {0x4D, 0x61, 0x72, 0x69, 0x6F, ...}              (~15 bytes)
Savings:      ~70% memory reduction
```

### Volume Control Implementation

```cpp
// Hardware volume control (varies by buzzer)
void setHardwareVolume(int volume) {
    if (volume == 0) {
        // Mute by stopping tone generation
        noTone(BUZZER_PIN);
    } else {
        // Volume control through PWM duty cycle
        int pwmValue = map(volume, 0, 100, 0, 255);
        analogWrite(BUZZER_PIN, pwmValue);
    }
}
```

## 🔍 Troubleshooting

### Common Issues

**No sound output:**
1. Check buzzer wiring (positive to GPIO 15, negative to GND)
2. Verify BUZZER_PIN definition in settings.h
3. Test with simple tone: `tone(15, 440, 1000);`

**Choppy playback:**
1. Ensure `ringtonePlayer.update()` is called in main loop
2. Check for blocking operations in loop
3. Monitor memory usage

**Build errors:**
1. Verify Python 3 is installed
2. Check that `data/ringtones/` directory exists
3. Ensure RTTTL files have `.rtttl.txt` extension

### Performance Monitoring

```cpp
// Check audio system performance
void checkAudioPerformance() {
    Serial.printf("Playing: %s\n", ringtonePlayer.isPlaying() ? "Yes" : "No");
    Serial.printf("Volume: %d%%\n", ringtonePlayer.getVolume());
    Serial.printf("Available: %d ringtones\n", ringtonePlayer.getRingtoneCount());
}
```

## 📚 Resources

### RTTTL References
- **[RTTTL Format Specification](http://merwin.bespin.org/t4a/specs/nokia_rtttl.txt)** - Original Nokia specification
- **[Online RTTTL Editor](https://adamonsoon.github.io/rtttl-play/)** - Create and test RTTTL files
- **[RTTTL Database](http://www.picaxe.com/RTTTL-Ringtones-for-Tune-Command/)** - Thousands of existing ringtones

### Library Documentation
- **[AnyRtttl Library](https://github.com/end2endzone/AnyRtttl)** - Full library documentation
- **[Arduino Tone Reference](https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/)** - Arduino tone function

The audio system provides a powerful yet simple foundation for adding music and sound effects to your AlertTX-1 project while maintaining the responsive performance essential for embedded applications.