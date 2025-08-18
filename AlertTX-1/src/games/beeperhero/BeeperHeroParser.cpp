#include "BeeperHeroParser.h"
#include <cstring>
#include <cstdlib>

// Note frequency table (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
const uint16_t BeeperHeroParser::noteFrequencies[8][12] = {
    // Octave 0-7, each with 12 semitones
    {16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31},      // Octave 0
    {33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62},      // Octave 1
    {65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 123},  // Octave 2
    {131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247}, // Octave 3
    {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494}, // Octave 4
    {523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988}, // Octave 5
    {1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976}, // Octave 6
    {2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951}  // Octave 7
};

uint16_t BeeperHeroParser::getNoteFrequency(char note, uint8_t octave, bool isSharp) {
    if (note == 'p') return 0; // Rest/pause
    
    octave = constrain(octave, 0, 7);
    
    // Convert note letter to semitone index
    uint8_t semitone = 0;
    switch (tolower(note)) {
        case 'c': semitone = 0; break;
        case 'd': semitone = 2; break;
        case 'e': semitone = 4; break;
        case 'f': semitone = 5; break;
        case 'g': semitone = 7; break;
        case 'a': semitone = 9; break;
        case 'b': semitone = 11; break;
        default: return 0;
    }
    
    if (isSharp) semitone++;
    if (semitone > 11) semitone = 11;
    
    return noteFrequencies[octave][semitone];
}

unsigned long BeeperHeroParser::calculateNoteDuration(uint8_t duration, uint16_t bpm, bool isDotted) {
    // Calculate note duration in milliseconds
    // duration: 1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth, 32=thirty-second
    
    // One beat (quarter note) duration in milliseconds
    unsigned long quarterNoteDuration = (60000UL / bpm);
    
    // Calculate note duration
    unsigned long noteDuration = (quarterNoteDuration * 4) / duration;
    
    // Apply dotted note (adds 50% to duration)
    if (isDotted) {
        noteDuration += noteDuration / 2;
    }
    
    return noteDuration;
}

uint8_t BeeperHeroParser::mapNoteToLane(char note, uint8_t octave, bool isSharp) {
    // Map notes to 3 lanes based on frequency/pitch
    // Lane 0: Low notes (octave 4 and below)
    // Lane 1: Mid notes (octave 5)
    // Lane 2: High notes (octave 6 and above)
    
    if (note == 'p') {
        // Rests don't appear as notes in the game
        return 255; // Invalid lane
    }
    
    if (octave <= 4) return 0;      // Low lane
    else if (octave == 5) return 1; // Mid lane
    else return 2;                  // High lane
}

std::vector<GameNote> BeeperHeroParser::parseRTTTL(const char* rtttl) {
    std::vector<GameNote> gameNotes;
    
    if (!rtttl) return gameNotes;
    
    // Make a copy to work with
    char* rtttlCopy = strdup(rtttl);
    
    // Split into sections: name:defaults:notes
    char* sections[3];
    int sectionCount = 0;
    char* token = strtok(rtttlCopy, ":");
    
    while (token && sectionCount < 3) {
        sections[sectionCount++] = token;
        token = strtok(nullptr, ":");
    }
    
    if (sectionCount < 3) {
        free(rtttlCopy);
        return gameNotes;
    }
    
    // Parse defaults
    RTTTLDefaults defaults;
    token = strtok(sections[1], ",");
    while (token) {
        if (strncmp(token, "d=", 2) == 0) {
            defaults.defaultDuration = atoi(token + 2);
        } else if (strncmp(token, "o=", 2) == 0) {
            defaults.defaultOctave = atoi(token + 2);
        } else if (strncmp(token, "b=", 2) == 0) {
            defaults.beatsPerMinute = atoi(token + 2);
        }
        token = strtok(nullptr, ",");
    }
    
    // Parse notes
    unsigned long currentTime = 0;
    token = strtok(sections[2], ",");
    
    while (token) {
        // Trim whitespace
        while (*token == ' ') token++;
        
        if (strlen(token) == 0) {
            token = strtok(nullptr, ",");
            continue;
        }
        
        GameNote gameNote;
        gameNote.startTime = currentTime;
        gameNote.isRest = false;
        gameNote.isSharp = false;
        
        // Parse note components
        uint8_t duration = defaults.defaultDuration;
        char noteLetter = 'p';
        uint8_t octave = defaults.defaultOctave;
        bool isDotted = false;
        
        char* ptr = token;
        
        // Extract duration (if present)
        if (isdigit(*ptr)) {
            duration = atoi(ptr);
            while (isdigit(*ptr)) ptr++;
        }
        
        // Extract note letter
        if (*ptr && isalpha(*ptr)) {
            noteLetter = tolower(*ptr);
            ptr++;
        }
        
        // Extract sharp
        if (*ptr == '#') {
            gameNote.isSharp = true;
            ptr++;
        }
        
        // Extract octave (if present)
        if (*ptr && isdigit(*ptr)) {
            octave = atoi(ptr);
            while (isdigit(*ptr)) ptr++;
        }
        
        // Extract dotted note
        if (*ptr == '.') {
            isDotted = true;
        }
        
        // Calculate note properties
        gameNote.duration = calculateNoteDuration(duration, defaults.beatsPerMinute, isDotted);
        gameNote.frequency = getNoteFrequency(noteLetter, octave, gameNote.isSharp);
        gameNote.lane = mapNoteToLane(noteLetter, octave, gameNote.isSharp);
        gameNote.isRest = (noteLetter == 'p');
        gameNote.noteName = noteLetter;
        gameNote.octave = octave;
        
        // Only add non-rest notes to the game
        if (!gameNote.isRest && gameNote.lane != 255) {
            gameNotes.push_back(gameNote);
        }
        
        // Advance time
        currentTime += gameNote.duration;
        
        token = strtok(nullptr, ",");
    }
    
    free(rtttlCopy);
    return gameNotes;
}

unsigned long BeeperHeroParser::getSongDuration(const std::vector<GameNote>& notes) {
    if (notes.empty()) return 0;
    
    unsigned long maxTime = 0;
    for (const auto& note : notes) {
        unsigned long noteEnd = note.startTime + note.duration;
        if (noteEnd > maxTime) {
            maxTime = noteEnd;
        }
    }
    return maxTime;
}

std::vector<GameNote> BeeperHeroParser::getNotesAtTime(const std::vector<GameNote>& notes, unsigned long currentTime) {
    std::vector<GameNote> activeNotes;
    
    for (const auto& note : notes) {
        unsigned long noteEnd = note.startTime + note.duration;
        if (currentTime >= note.startTime && currentTime <= noteEnd) {
            activeNotes.push_back(note);
        }
    }
    
    return activeNotes;
}


