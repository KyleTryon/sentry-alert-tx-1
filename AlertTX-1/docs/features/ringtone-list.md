# Available Ringtones

The Alert TX-1 includes 16 built-in ringtones, all properly formatted and ready to use.

## Complete Ringtone List (Alphabetically Sorted)

1. **Axel F** - Beverly Hills Cop theme
2. **Barbie Girl** - Aqua's pop hit
3. **Blue (Eiffel 65)** - "I'm Blue" by Eiffel 65
4. **Desk Phone** - Classic office phone ring
5. **Digimon** - Theme from the animated series
6. **Futurama** - Theme from the animated series
7. **Kim Possible** - Disney Channel show beeper sound
8. **King of the Hill** - Theme from the animated series
9. **Mario** - Super Mario Bros. theme
10. **Rick Roll** - "Never Gonna Give You Up" by Rick Astley
11. **Sk8er Boi** - Avril Lavigne's hit song
12. **Spiderman** - Classic Spiderman theme
13. **Take On Me** - a-ha's iconic song
14. **Test Tones** - Various test beeps and tones
15. **The Simpsons** - Theme from the animated series
16. **Zelda** - Legend of Zelda theme

## Technical Details

All ringtones are stored in RTTTL (Ring Tone Text Transfer Language) format and are compiled into the firmware for reliable playback without filesystem dependencies.

### File Locations
- Source files: `data/ringtones/*.rtttl.txt`
- Generated header: `src/ringtones/ringtone_data.h`

### Memory Optimization
- Binary format provides ~42.5% memory savings
- Multiple formats available (binary, text, BeeperHero track)
- Total size: 1,794 bytes (binary) vs 3,119 bytes (text)

## Usage

### In Settings Menu
Navigate to Settings → Ringtone to preview and select your notification sound.

### In Code
```cpp
// Play by name
ringtonePlayer.playRingtoneByName("Mario");

// Play by index (0-15)
ringtonePlayer.playRingtoneByIndex(8); // Plays "Mario"

// Get ringtone info
const char* name = ringtonePlayer.getRingtoneName(8); // Returns "Mario"
```

## Adding Custom Ringtones

1. Create a new `.rtttl.txt` file in `data/ringtones/`
2. Ensure the RTTTL name is on the first line
3. Run `make ringtones` to regenerate the data
4. The new ringtone will be automatically included

Example RTTTL format:
```
MyTune:d=4,o=5,b=125:c,e,g,c6,g,e,c
```

## Troubleshooting

If a ringtone doesn't play:
1. Check that the RTTTL name is on the first line of the file
2. Verify the RTTTL syntax is valid
3. Run `make clean && make ringtones` to force regeneration
4. Check serial output for any parsing errors
