#include "BeeperHeroTrack.h"
#include <cstring>

BeeperHeroTrack::BeeperHeroTrack() 
    : header(nullptr), songName(nullptr), notes(nullptr), isLoaded(false) {
}

BeeperHeroTrack::~BeeperHeroTrack() {
    // Note: We don't own the memory, just pointing to it
}

bool BeeperHeroTrack::loadFromMemory(const uint8_t* trackData, size_t dataSize) {
    if (!trackData || dataSize < sizeof(BeeperHeroTrackHeader)) {
        Serial.println("BeeperHeroTrack: Invalid track data");
        return false;
    }
    
    // Cast to header structure
    header = reinterpret_cast<const BeeperHeroTrackHeader*>(trackData);
    
    // Validate magic bytes
    if (strncmp(header->magic, "BPHR", 4) != 0) {
        Serial.println("BeeperHeroTrack: Invalid magic bytes");
        return false;
    }
    
    // Validate version
    if (header->version != 1) {
        Serial.printf("BeeperHeroTrack: Unsupported version: %d\n", header->version);
        return false;
    }
    
    // Calculate expected size
    size_t expectedSize = sizeof(BeeperHeroTrackHeader) + 
                         header->songNameLength + 1 + // +1 for null terminator
                         (header->noteCount * sizeof(BeeperHeroNote));
    
    if (dataSize < expectedSize) {
        Serial.printf("BeeperHeroTrack: Data size mismatch. Expected: %zu, Got: %zu\n", 
                     expectedSize, dataSize);
        return false;
    }
    
    // Set up pointers
    const uint8_t* ptr = trackData + sizeof(BeeperHeroTrackHeader);
    songName = reinterpret_cast<const char*>(ptr);
    ptr += header->songNameLength + 1;
    notes = reinterpret_cast<const BeeperHeroNote*>(ptr);
    
    isLoaded = true;
    
    Serial.printf("BeeperHeroTrack: Loaded track '%s' with %d notes\n", 
                 songName, header->noteCount);
    
    return true;
}

const BeeperHeroNote* BeeperHeroTrack::getNote(uint16_t index) const {
    if (!isLoaded || index >= header->noteCount) {
        return nullptr;
    }
    return &notes[index];
}

const BeeperHeroNote* BeeperHeroTrack::getNotesInTimeRange(uint32_t startTime, uint32_t endTime, uint16_t& count) const {
    count = 0;
    if (!isLoaded) return nullptr;
    
    // Find first note in range
    const BeeperHeroNote* firstNote = nullptr;
    for (uint16_t i = 0; i < header->noteCount; i++) {
        if (notes[i].startTime >= startTime && notes[i].startTime <= endTime) {
            if (!firstNote) firstNote = &notes[i];
            count++;
        } else if (firstNote) {
            // We've passed the range
            break;
        }
    }
    
    return firstNote;
}

bool BeeperHeroTrack::shouldNoteBeVisible(uint16_t noteIndex, uint32_t currentTime, uint32_t approachTime) const {
    const BeeperHeroNote* note = getNote(noteIndex);
    if (!note) return false;
    
    uint32_t noteAppearTime = note->startTime > approachTime ? note->startTime - approachTime : 0;
    uint32_t noteDisappearTime = note->startTime + note->duration + 200; // 200ms grace period
    
    return currentTime >= noteAppearTime && currentTime <= noteDisappearTime;
}

bool BeeperHeroTrack::isNoteHittable(uint16_t noteIndex, uint32_t currentTime, uint32_t hitWindow) const {
    const BeeperHeroNote* note = getNote(noteIndex);
    if (!note) return false;
    
    uint32_t timeDiff = abs((int32_t)currentTime - (int32_t)note->startTime);
    return timeDiff <= hitWindow;
}

float BeeperHeroTrack::calculateNotePosition(uint16_t noteIndex, uint32_t currentTime, uint32_t approachTime) const {
    const BeeperHeroNote* note = getNote(noteIndex);
    if (!note) return -1.0f;
    
    uint32_t noteAppearTime = note->startTime > approachTime ? note->startTime - approachTime : 0;
    
    if (currentTime < noteAppearTime) {
        return -1.0f; // Not visible yet
    }
    
    if (currentTime >= note->startTime) {
        return 1.0f; // At or past hit line
    }
    
    // Calculate position between 0.0 (top) and 1.0 (hit line)
    float progress = (float)(currentTime - noteAppearTime) / approachTime;
    return constrain(progress, 0.0f, 1.0f);
}

void BeeperHeroTrack::printTrackInfo() const {
    if (!isLoaded) {
        Serial.println("BeeperHeroTrack: No track loaded");
        return;
    }
    
    Serial.println("=== BeeperHero Track Info ===");
    Serial.printf("Song: %s\n", songName);
    Serial.printf("Duration: %lu ms (%.1f seconds)\n", header->songDuration, header->songDuration / 1000.0f);
    Serial.printf("BPM: %d\n", header->bpm);
    Serial.printf("Notes: %d\n", header->noteCount);
    
    // Count notes per lane
    uint16_t laneCounts[3] = {0, 0, 0};
    for (uint16_t i = 0; i < header->noteCount; i++) {
        if (notes[i].lane < 3) {
            laneCounts[notes[i].lane]++;
        }
    }
    
    Serial.printf("Lane distribution: L1=%d, L2=%d, L3=%d\n", 
                 laneCounts[0], laneCounts[1], laneCounts[2]);
    
    // Calculate difficulty metrics
    float notesPerSecond = (float)header->noteCount / (header->songDuration / 1000.0f);
    Serial.printf("Difficulty: %.1f notes/second\n", notesPerSecond);
    
    Serial.println("============================");
}


