# Beeper Hero Game

## Overview

A rhythm-based game inspired by Guitar Hero, designed for the AlertTX-1 beeper device. Players must press buttons in time with musical notes scrolling down the screen, creating an engaging musical experience using the device's limited hardware. The name "Beeper Hero" celebrates the device's buzzer-based audio system.

## Game Concept

### Core Mechanics
- **Rhythm Gameplay**: Notes scroll down the screen in time with music
- **Three-Button System**: Notes are mapped to three physical buttons (LEFT_UP, LEFT_MID, LEFT_DOWN)
- **RTTTL Integration**: Uses existing RTTTL ringtone files for music and note data
- **Scoring System**: Points awarded for accurate timing and combo multipliers

### Visual Design
```
┌───────────────────────────────┐
│         Beeper Hero           │  ← Header
├───────────────────────────────┤
│ ⬅️  ⬅️  ⬅️  ⬅️  ⬅️  ⬅️         │  ← Top lane (LEFT_UP button)
│ ⬅️  ⬅️  ⬅️  ⬅️  ⬅️  ⬅️         │  ← Middle lane (LEFT_MID button)
│ ⬅️  ⬅️  ⬅️  ⬅️  ⬅️  ⬅️         │  ← Bottom lane (LEFT_DOWN button)
│                               │
│                               │
├───────────────────────────────┤
│ Score: 1250 | Combo: 8        │  ← Status bar
└───────────────────────────────┘
```

**Note:**
- Each horizontal row is a lane for a button (top = LEFT_UP, middle = LEFT_MID, bottom = LEFT_DOWN).
- Notes scroll from right to left across each row, matching the physical button layout on the left side of the device.

## Technical Implementation

### RTTTL Parser Integration

The game leverages the existing RTTTL ringtone system:

```cpp
class BeeperHeroGame {
private:
    RTTTLParser _parser;
    NoteSequence _currentSong;
    int _currentNoteIndex;
    
    // Note mapping to buttons
    enum NoteLane {
        LANE_LEFT_UP,    // Maps to BUTTON_LEFT_UP
        LANE_LEFT_MID,   // Maps to BUTTON_LEFT_MID  
        LANE_LEFT_DOWN   // Maps to BUTTON_LEFT_DOWN
    };
};
```

### Note Mapping Strategy

RTTTL notes are mapped to three lanes based on frequency/pitch:

```cpp
NoteLane mapNoteToLane(int frequency) {
    if (frequency < 440) {        // A4 and below
        return LANE_LEFT_DOWN;    // Lower notes = bottom button
    } else if (frequency < 880) { // A4 to A5
        return LANE_LEFT_MID;     // Middle notes = middle button
    } else {                      // A5 and above
        return LANE_LEFT_UP;      // Higher notes = top button
    }
}
```

### Game State Management

```cpp
struct GameState {
    bool isPlaying;
    int score;
    int combo;
    int maxCombo;
    int notesHit;
    int notesMissed;
    float accuracy;
    unsigned long gameStartTime;
    unsigned long lastNoteTime;
};
```

## Available Songs

The game uses existing RTTTL files from the ringtones directory:

### Classic Gaming
- **Mario Theme** (`mario.rtttl.txt`) - Iconic Nintendo melody
- **Zelda Theme** (`zelda.rtttl.txt`) - Epic adventure music
- **Digimon Theme** (`digimon.rtttl.txt`) - Anime opening

### Pop Culture
- **Rick Roll** (`rick_roll.rtttl.txt`) - Never gonna give you up!
- **Barbie Girl** (`barbie_girl.txt`) - 90s pop classic
- **Sk8er Boy** (`sk8erboy.txt`) - Avril Lavigne hit

### TV & Movies
- **Futurama Theme** (`futurama.rtttl.txt`) - Sci-fi comedy
- **The Simpsons** (`the_simpsons.rtttl.txt`) - TV classic
- **King of the Hill** (`king_of_the_hill.txt`) - Country comedy
- **Spider-Man Theme** (`spiderman.rtttl.txt`) - Superhero theme

### Modern Hits
- **Eiffel 65 - Blue** (`eiffel_65_blue.rtttl.txt`) - Eurodance
- **Take On Me** (`take_on_me.rtttl.txt`) - A-ha classic
- **1000 Miles** (`1000_miles.txt`) - Vanessa Carlton

## Game Modes

### 1. Quick Play
- Single song selection
- Immediate start
- Basic scoring

### 2. Career Mode
- Multiple songs in sequence
- Progressive difficulty
- Unlockable content

### 3. Practice Mode
- Slower note speed
- Visual timing guides
- No scoring pressure

## Scoring System

### Point Values
- **Perfect Hit**: 100 points
- **Good Hit**: 50 points  
- **Miss**: 0 points
- **Combo Multiplier**: +10% per 10 combo

### Accuracy Calculation
```
Accuracy = (Notes Hit / Total Notes) × 100%
```

### Performance Grades
- **S Rank**: 95%+ accuracy
- **A Rank**: 85-94% accuracy
- **B Rank**: 70-84% accuracy
- **C Rank**: 50-69% accuracy
- **D Rank**: Below 50% accuracy

## User Interface

### Main Menu
```
┌─────────────────────┐
│     Beeper Hero     │
├─────────────────────┤
│  > Quick Play       │
│    Career Mode      │
│    Practice Mode    │
│    Song Select      │
│    High Scores      │
│    Settings         │
└─────────────────────┘
```

### Song Selection
```
┌─────────────────────┐
│   Select Song       │
├─────────────────────┤
│  > Mario Theme      │
│    Zelda Theme      │
│    Rick Roll        │
│    Futurama         │
│    [More...]        │
└─────────────────────┘
```

### Game Screen
```
┌─────────────────────┐
│ Mario Theme | 1:23  │
├─────────────────────┤
│ ⬆️  ⬆️  ⬆️  ⬆️  ⬆️  ⬆️  │  ← Top lane (LEFT_UP button)
│ ⬇️  ⬇️  ⬇️  ⬇️  ⬇️  ⬇️  │  ← Middle lane (LEFT_MID button)
│ ⬇️  ⬇️  ⬇️  ⬇️  ⬇️  ⬇️  │  ← Bottom lane (LEFT_DOWN button)
│                     │
│                     │
│                     │
│                     │
│                     │
│                     │
├─────────────────────┤
│ Score: 1250 | Combo: 8 │
└─────────────────────┘
```

## Controls

### Gameplay
- **LEFT_UP Button**: Hit notes in top lane
- **LEFT_MID Button**: Hit notes in middle lane  
- **LEFT_DOWN Button**: Hit notes in bottom lane
- **RIGHT Button**: Pause game

### Menu Navigation
- **LEFT_UP/DOWN**: Navigate menu items
- **LEFT_MID**: Select/confirm
- **RIGHT**: Back/cancel

## Technical Features

### Audio Integration
- **Synchronized Audio**: Notes play in time with visual cues
- **Mute Option**: Game can run without audio
- **Volume Control**: Adjustable game volume

### Performance Optimization
- **Frame Rate**: 30 FPS for smooth animation
- **Memory Management**: Efficient note storage and cleanup
- **Battery Optimization**: Reduced brightness during gameplay

### Difficulty Settings
- **Easy**: Slower note speed, larger hit windows
- **Normal**: Standard speed and timing
- **Hard**: Faster notes, tighter timing windows
- **Expert**: Maximum speed, precise timing required

## Development Roadmap

### Phase 1: Core Gameplay ✅
- [x] Basic game structure
- [x] RTTTL parser integration
- [x] Three-button mapping
- [ ] Note scrolling animation
- [ ] Input detection and scoring

### Phase 2: Polish & Features
- [ ] Multiple difficulty levels
- [ ] Song selection menu
- [ ] High score system
- [ ] Visual effects and feedback
- [ ] Audio synchronization

### Phase 3: Advanced Features
- [ ] Career mode progression
- [ ] Custom song support
- [ ] Practice mode with timing guides
- [ ] Performance statistics
- [ ] Achievement system

### Phase 4: Community Features
- [ ] Score sharing
- [ ] Custom song creation tools
- [ ] Tournament mode
- [ ] Online leaderboards

## File Structure

```
AlertTX-1/src/game/
├── README.md              # This file
├── BeeperHeroGame.h       # Main game class header
├── BeeperHeroGame.cpp     # Main game implementation
├── NoteParser.h           # RTTTL to game notes parser
├── NoteParser.cpp         # Parser implementation
├── GameRenderer.h         # Visual rendering system
├── GameRenderer.cpp       # Renderer implementation
├── ScoreManager.h         # Scoring and statistics
├── ScoreManager.cpp       # Score implementation
└── SongLibrary.h          # Available songs database
```

## Integration with AlertTX-1

### Hardware Integration
- **Display**: ST7735 TFT for visual output
- **Buttons**: Three-button input system
- **Buzzer**: Audio feedback and music playback
- **LED**: Visual feedback for hits/misses

### System Integration
- **UIManager**: Game runs as a screen in the UI system
- **ButtonManager**: Uses hardware abstraction for input
- **RingtonePlayer**: Leverages existing audio system
- **Settings**: Game preferences stored in device settings

## Performance Considerations

### Memory Usage
- **Note Buffer**: Pre-load song data to avoid parsing delays
- **Visual Assets**: Reuse sprites and animations
- **Audio Buffering**: Stream audio data efficiently

### Processing Power
- **Note Updates**: 30 FPS update rate
- **Input Processing**: Immediate response required
- **Audio Synchronization**: Precise timing critical

### Battery Life
- **Screen Brightness**: Reduced during gameplay
- **Audio Volume**: Optimized for battery efficiency
- **Sleep Mode**: Automatic power management

This Beeper Hero implementation transforms the AlertTX-1 into a portable rhythm gaming device, leveraging the existing RTTTL music library to create an engaging and nostalgic gaming experience. The name celebrates the device's buzzer-based audio system while maintaining the familiar rhythm game mechanics. 