#ifndef RINGTONE_PLAYER_H
#define RINGTONE_PLAYER_H

#include <Arduino.h>
#include "../../lib/AnyRtttl/src/anyrtttl.h"
#include "ringtone_data.h"  // Auto-generated ringtone data

// Note information structure for BeeperHero game integration
struct NoteInfo {
    uint16_t frequency;     // Note frequency in Hz
    unsigned long startTime; // When the note starts (milliseconds)
    unsigned long duration;  // Note duration (milliseconds)
    uint8_t octave;         // Note octave
    uint8_t durationIndex;  // Duration index (1=whole, 2=half, 4=quarter, etc.)
    bool isRest;           // True if this is a rest/silence
    bool isDotted;         // True if note is dotted
    bool isSharp;          // True if note is sharp (#)
};

class RingtonePlayer {
private:
    // Current playback state
    bool isPlayingFlag;
    unsigned long playbackStartTime;
    const char* currentMelody;
    
    // BeeperHero game integration
    NoteInfo currentNoteInfo;
    bool noteInfoValid;
    
    // Volume control
    uint8_t volume;
    bool muted;
    
    // Hardware interface
    int buzzerPin;

public:
    RingtonePlayer();
    ~RingtonePlayer() = default;
    
    // Initialization
    void begin(int buzzerPin);
    void setVolume(uint8_t vol); // 0-100
    void setMuted(bool mute);
    
    // Playback control
    void playRingtone(const char* rtttl);  // Legacy text format support
    void playRingtoneByName(const char* name);  // Play by ringtone name (binary format)
    void playRingtoneByIndex(int index);        // Play by index (binary format)
    void playRingtoneBinary(const unsigned char* binary_data, int length);  // Play binary format
    void stop();
    void pause();
    void resume();
    
    // Status queries
    bool isPlaying() const;
    bool isPaused() const;
    unsigned long getPlaybackTime() const;
    float getProgress() const; // 0.0 to 1.0
    
    // BeeperHero game integration
    NoteInfo getCurrentNote() const;
    bool hasNoteInfo() const;
    uint16_t getCurrentFrequency() const;
    unsigned long getNoteStartTime() const;
    unsigned long getNoteDuration() const;
    
    // Main update loop (call in main loop)
    void update();
    
    // Utility functions
    void setBuzzerPin(int pin);
    int getBuzzerPin() const;
    
    // Ringtone management
    int getRingtoneCount() const;
    const char* getRingtoneName(int index) const;
    int findRingtoneIndex(const char* name) const;
    
private:
    // Internal methods
    void updateNoteInfo();
    void calculateNoteInfo();
    
    // Hardware interface
    void playTone(uint16_t frequency, unsigned long duration);
    void stopTone();
};

// Global ringtone player instance
extern RingtonePlayer ringtonePlayer;

#endif // RINGTONE_PLAYER_H
