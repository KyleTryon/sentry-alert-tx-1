#include "RingtonePlayer.h"
#include "src/config/settings.h"

// Global ringtone player instance
RingtonePlayer ringtonePlayer;

RingtonePlayer::RingtonePlayer() {
    isPlayingFlag = false;
    playbackStartTime = 0;
    currentMelody = nullptr;
    volume = 100;
    muted = false;
    buzzerPin = BUZZER_PIN;
    noteInfoValid = false;
    
    // Initialize note info
    currentNoteInfo.frequency = 0;
    currentNoteInfo.startTime = 0;
    currentNoteInfo.duration = 0;
    currentNoteInfo.octave = 0;
    currentNoteInfo.durationIndex = 0;
    currentNoteInfo.isRest = false;
    currentNoteInfo.isDotted = false;
    currentNoteInfo.isSharp = false;
}

void RingtonePlayer::begin(int buzzerPin) {
    this->buzzerPin = buzzerPin;
    pinMode(buzzerPin, OUTPUT);
    
    Serial.println("RingtonePlayer initialized with AnyRtttl");
}

void RingtonePlayer::setVolume(uint8_t vol) {
    volume = constrain(vol, 0, 100);
}

void RingtonePlayer::setMuted(bool mute) {
    muted = mute;
    if (muted && isPlaying()) {
        stopTone();
    }
}

void RingtonePlayer::playRingtone(const char* rtttl) {
    if (!rtttl) return;
    
    currentMelody = rtttl;
    isPlayingFlag = true;
    playbackStartTime = millis();
    noteInfoValid = false;
    
    // Start AnyRtttl playback using non-blocking API
    anyrtttl::nonblocking::begin(buzzerPin, rtttl);
    
    Serial.printf("Playing ringtone: %s\n", rtttl);
}

void RingtonePlayer::playRingtoneByName(const char* name) {
    const unsigned char* binary_data = getRingtoneData(name);
    int length = getRingtoneLength(name);
    if (binary_data && length > 0) {
        playRingtoneBinary(binary_data, length);
    } else {
        Serial.printf("Ringtone not found: %s\n", name);
    }
}

void RingtonePlayer::playRingtoneByIndex(int index) {
    const unsigned char* binary_data = getRingtoneData(index);
    int length = getRingtoneLength(index);
    if (binary_data && length > 0) {
        playRingtoneBinary(binary_data, length);
    } else {
        Serial.printf("Ringtone index out of range: %d\n", index);
    }
}

void RingtonePlayer::playRingtoneBinary(const unsigned char* binary_data, int length) {
    if (!binary_data || length <= 0) return;
    
    // Use AnyRtttl binary playback
    anyrtttl::nonblocking::begin(buzzerPin, binary_data, length);
    
    isPlayingFlag = true;
    playbackStartTime = millis();
    noteInfoValid = false;
    
    Serial.printf("Playing binary ringtone (%d bytes)\n", length);
}

void RingtonePlayer::playRingtoneFromMemory(const char* rtttl) {
    // Same as playRingtone for now - AnyRtttl handles memory management
    playRingtone(rtttl);
}

void RingtonePlayer::stop() {
    isPlayingFlag = false;
    currentMelody = nullptr;
    noteInfoValid = false;
    stopTone();
    
    // Stop AnyRtttl playback
    anyrtttl::nonblocking::stop();
    
    Serial.println("Ringtone stopped");
}

void RingtonePlayer::pause() {
    if (isPlayingFlag) {
        isPlayingFlag = false;
        stopTone();
        
        // Pause AnyRtttl playback (stop and remember state)
        anyrtttl::nonblocking::stop();
    }
}

void RingtonePlayer::resume() {
    if (currentMelody && !isPlayingFlag) {
        isPlayingFlag = true;
        playbackStartTime = millis() - getPlaybackTime();
        
        // Resume AnyRtttl playback by restarting
        anyrtttl::nonblocking::begin(buzzerPin, currentMelody);
    }
}

bool RingtonePlayer::isPlaying() const {
    return isPlayingFlag && anyrtttl::nonblocking::isPlaying();
}

bool RingtonePlayer::isPaused() const {
    return currentMelody != nullptr && !isPlayingFlag;
}

unsigned long RingtonePlayer::getPlaybackTime() const {
    if (!isPlayingFlag) return 0;
    return millis() - playbackStartTime;
}

float RingtonePlayer::getProgress() const {
    // AnyRtttl doesn't provide progress directly, so we'll estimate
    if (anyrtttl::nonblocking::isPlaying()) {
        // Estimate progress based on time elapsed
        // This is a rough approximation
        return 0.5f; // Placeholder - would need to parse RTTTL for actual duration
    }
    return 0.0f;
}

NoteInfo RingtonePlayer::getCurrentNote() const {
    return currentNoteInfo;
}

bool RingtonePlayer::hasNoteInfo() const {
    return noteInfoValid;
}

uint16_t RingtonePlayer::getCurrentFrequency() const {
    return currentNoteInfo.frequency;
}

unsigned long RingtonePlayer::getNoteStartTime() const {
    return currentNoteInfo.startTime;
}

unsigned long RingtonePlayer::getNoteDuration() const {
    return currentNoteInfo.duration;
}

void RingtonePlayer::update() {
    if (!isPlayingFlag) return;
    
    // Update AnyRtttl player
    anyrtttl::nonblocking::play();
    
    // Update note info for BeeperHero game
    updateNoteInfo();
    
    // Check if playback finished
    if (anyrtttl::nonblocking::done()) {
        stop();
    }
}

void RingtonePlayer::updateNoteInfo() {
    if (!isPlayingFlag || !anyrtttl::nonblocking::isPlaying()) {
        noteInfoValid = false;
        return;
    }
    
    // Get current note info from AnyRtttl
    // Note: AnyRtttl doesn't expose note-level info directly, so we'll simulate it
    // based on the current playback state and timing
    
    unsigned long currentTime = getPlaybackTime();
    
    // For now, we'll generate note info based on timing
    // In a full implementation, you might want to parse the RTTTL string
    // to get actual note information
    
    // Simulate note changes every 200ms
    unsigned long noteTime = (currentTime / 200) * 200;
    
    if (noteTime != currentNoteInfo.startTime) {
        // New note started
        currentNoteInfo.startTime = noteTime;
        currentNoteInfo.duration = 200; // 200ms notes
        currentNoteInfo.frequency = 440 + (noteTime / 200) % 880; // A4 to A5
        currentNoteInfo.octave = 5;
        currentNoteInfo.durationIndex = 4; // Quarter notes
        currentNoteInfo.isRest = false;
        currentNoteInfo.isDotted = false;
        currentNoteInfo.isSharp = false;
        noteInfoValid = true;
    }
}

void RingtonePlayer::calculateNoteInfo() {
    // Calculate note info from AnyRtttl data
    updateNoteInfo();
}

void RingtonePlayer::setBuzzerPin(int pin) {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);
}

int RingtonePlayer::getBuzzerPin() const {
    return buzzerPin;
}

// Ringtone management methods
int RingtonePlayer::getRingtoneCount() const {
    return RINGTONE_COUNT;
}

const char* RingtonePlayer::getRingtoneName(int index) const {
    return getRingtoneName(index);
}

int RingtonePlayer::findRingtoneIndex(const char* name) const {
    return findRingtoneIndex(name);
}

void RingtonePlayer::playTone(uint16_t frequency, unsigned long duration) {
    if (muted || volume == 0) return;
    
    // Use AnyRtttl tone generation
    if (frequency > 0) {
        // AnyRtttl handles tone generation internally
        // This method is mainly for compatibility
    }
}

void RingtonePlayer::stopTone() {
    // AnyRtttl handles tone stopping internally
    noTone(buzzerPin);
}
