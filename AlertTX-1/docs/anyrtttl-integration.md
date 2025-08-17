# AnyRtttl Library Integration

## Overview

The Alert TX-1 project integrates the [AnyRtttl library](https://github.com/end2endzone/AnyRtttl) to provide professional-grade RTTTL ringtone playback and rhythm game functionality. This library offers non-blocking playback, memory-efficient storage, and precise note timing - perfect for the BeeperHero rhythm game.

## Library Features

### Key Benefits
- **Non-blocking playback** - Maintains responsive UI during music
- **Memory efficient** - Binary format reduces memory usage by 70-80%
- **Precise timing** - Accurate note timing for rhythm games
- **Multiple formats** - Text RTTTL, binary 10-bit, and binary 16-bit
- **Professional quality** - Well-tested, documented, and maintained

### Supported Platforms
- ESP32/ESP32-S3 (primary target)
- ESP8266
- Arduino (AVR, ARM)
- Other Arduino-compatible boards

## Installation Status

### ✅ Library Installed
The AnyRtttl library has been successfully installed in the project:
```
AlertTX-1/lib/AnyRtttl/
├── src/
│   ├── anyrtttl.h      # Main library header
│   ├── anyrtttl.cpp    # Main library implementation
│   ├── binrtttl.h      # Binary format support
│   ├── binrtttl.cpp    # Binary format implementation
│   └── pitches.h       # Note frequency definitions
├── examples/           # Example sketches
├── docs/              # Documentation
└── library.properties # Arduino library metadata
```

### ✅ Integration Complete
The codebase has been fully integrated with AnyRtttl:

1. **Enhanced RingtonePlayer** - Uses actual AnyRtttl API
2. **BeeperHero Game** - Integrated with note timing system
3. **Test Sketch** - Available for validation

## Implementation Details

### RingtonePlayer Integration

The `RingtonePlayer` class uses the AnyRtttl non-blocking API:

```cpp
// Start playback
anyrtttl::nonblocking::begin(BUZZER_PIN, rtttl);

// Update in main loop
anyrtttl::nonblocking::play();

// Check status
bool playing = anyrtttl::nonblocking::isPlaying();
bool finished = anyrtttl::nonblocking::done();

// Stop playback
anyrtttl::nonblocking::stop();
```

- Cleanup: `RingtonePlayer::stop()` halts playback and clears state. Games should also provide a `cleanup()` hook (called from `Screen::exit()`) to stop audio on exit.

### Key Features Implemented

1. **Non-blocking Operation**
   - Music plays without blocking the main loop
   - UI remains responsive during playback
   - Power management continues to work

2. **Note Information for Games**
   - Real-time simulated note frequency/timing via `updateNoteInfo()`
   - Sufficient for lane assignment and visual sync in BeeperHero

3. **Volume and Mute Control**
   - Volume control (0-100%)
   - Mute functionality
   - Hardware-level tone control

### BeeperHero Game Integration

1. **Synchronized Audio/Visual**
   - Visual notes are spawned on `NoteInfo` boundaries (fallback synthetic spawning if not available)
2. **Lane Mapping**
   - Simple frequency split into 3 lanes (low/mid/high)
3. **Optimized Rendering**
   - Lane-based dirty tracking clears only changed spans
   - Standardized header/play-area via `StandardGameLayout`
4. **Resource Cleanup**
   - `cleanup()` stops audio on exit to prevent leaks or buzzing

## Usage Examples

### Basic Ringtone Playback

```cpp
#include "src/ringtones/RingtonePlayer.h"

// Initialize
RingtonePlayer player;
player.begin(BUZZER_PIN);

// Play a ringtone
player.playRingtone("Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p");

// In main loop
player.update();
```

## Troubleshooting

- No sound: verify `BUZZER_PIN` in `settings.h` and wiring.
- Desync visuals: ensure `player.update()` is called regularly in your game `updateGame()`.
- Stuck audio: ensure game screens implement `cleanup()` which calls `player.stop()`.

## Conclusion

AnyRtttl provides solid, non-blocking playback. Coupled with lane-based dirty rendering and `StandardGameLayout`, BeeperHero achieves smooth visuals without full-screen redraws, and audio is reliably stopped on exit via `cleanup()`. 