#ifndef BEEPER_HERO_PARSER_H
#define BEEPER_HERO_PARSER_H

#include <Arduino.h>
#include <vector>

// Game note structure for BeeperHero
struct GameNote {
    unsigned long startTime;  // When note starts (milliseconds)
    unsigned long duration;   // How long note lasts
    uint8_t lane;            // Which lane (0, 1, 2)
    uint16_t frequency;      // Note frequency
    bool isRest;            // True if this is a rest/pause
    char noteName;          // Original note letter (c, d, e, f, g, a, b, p)
    uint8_t octave;         // Note octave
    bool isSharp;           // True if sharp (#)
};

class BeeperHeroParser {
private:
    struct RTTTLDefaults {
        uint8_t defaultDuration = 4;
        uint8_t defaultOctave = 5;
        uint16_t beatsPerMinute = 120;
    };
    
    // Note frequency lookup table
    static const uint16_t noteFrequencies[8][12];
    
    // Convert note letter + octave to frequency
    uint16_t getNoteFrequency(char note, uint8_t octave, bool isSharp);
    
    // Calculate note duration in milliseconds
    unsigned long calculateNoteDuration(uint8_t duration, uint16_t bpm, bool isDotted);
    
    // Map note to lane (0, 1, 2) based on frequency/octave
    uint8_t mapNoteToLane(char note, uint8_t octave, bool isSharp);

public:
    // Parse RTTTL string into game notes with precise timing
    std::vector<GameNote> parseRTTTL(const char* rtttl);
    
    // Get song duration in milliseconds
    unsigned long getSongDuration(const std::vector<GameNote>& notes);
    
    // Get notes that should be active at a specific time
    std::vector<GameNote> getNotesAtTime(const std::vector<GameNote>& notes, unsigned long currentTime);
};

#endif // BEEPER_HERO_PARSER_H


