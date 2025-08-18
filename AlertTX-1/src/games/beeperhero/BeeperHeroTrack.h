#ifndef BEEPER_HERO_TRACK_H
#define BEEPER_HERO_TRACK_H

#include <Arduino.h>

/**
 * BeeperHero Track Data Format
 * 
 * Binary format optimized for rhythm game performance.
 * Pre-generated from RTTTL files with precise timing and lane mapping.
 */

// Track file header structure
struct BeeperHeroTrackHeader {
    char magic[4];           // "BPHR" magic bytes
    uint8_t version;         // Format version (1)
    uint8_t songNameLength;  // Length of song name string
    uint16_t noteCount;      // Total number of notes
    uint32_t songDuration;   // Song duration in milliseconds
    uint16_t bpm;           // Beats per minute
    uint16_t reserved;      // Reserved for future use
} __attribute__((packed));

// Individual note data structure
struct BeeperHeroNote {
    uint32_t startTime;     // Note start time (milliseconds from song start)
    uint16_t duration;      // Note duration in milliseconds
    uint8_t lane;          // Lane number (0, 1, or 2)
    uint8_t flags;         // Flags for special note properties
} __attribute__((packed));

// Note flags
#define NOTE_FLAG_HOLD     0x01    // Long note (hold for duration)
#define NOTE_FLAG_BONUS    0x02    // Bonus note (extra points)
#define NOTE_FLAG_CRITICAL 0x04    // Critical note (must hit)

// Track difficulty levels
enum TrackDifficulty {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM = 1,
    DIFFICULTY_HARD = 2
};

// Lane assignment algorithm types
enum LaneAlgorithm {
    LANE_BY_OCTAVE = 0,     // Map by octave (low, mid, high)
    LANE_BY_FREQUENCY = 1,  // Map by frequency ranges
    LANE_BY_PATTERN = 2     // Map by musical patterns
};

/**
 * BeeperHeroTrack
 * 
 * Handles loading and accessing pre-generated track data.
 */
class BeeperHeroTrack {
private:
    const BeeperHeroTrackHeader* header;
    const char* songName;
    const BeeperHeroNote* notes;
    bool isLoaded;
    
public:
    BeeperHeroTrack();
    ~BeeperHeroTrack();
    
    // Load track from binary data
    bool loadFromMemory(const uint8_t* trackData, size_t dataSize);
    
    // Track information
    bool isValid() const { return isLoaded && header != nullptr; }
    const char* getSongName() const { return songName; }
    uint16_t getNoteCount() const { return header ? header->noteCount : 0; }
    uint32_t getSongDuration() const { return header ? header->songDuration : 0; }
    uint16_t getBPM() const { return header ? header->bpm : 120; }
    
    // Note access
    const BeeperHeroNote* getNote(uint16_t index) const;
    const BeeperHeroNote* getNotesInTimeRange(uint32_t startTime, uint32_t endTime, uint16_t& count) const;
    
    // Gameplay helpers
    bool shouldNoteBeVisible(uint16_t noteIndex, uint32_t currentTime, uint32_t approachTime) const;
    bool isNoteHittable(uint16_t noteIndex, uint32_t currentTime, uint32_t hitWindow) const;
    float calculateNotePosition(uint16_t noteIndex, uint32_t currentTime, uint32_t approachTime) const;
    
    // Debug
    void printTrackInfo() const;
};

#endif // BEEPER_HERO_TRACK_H


