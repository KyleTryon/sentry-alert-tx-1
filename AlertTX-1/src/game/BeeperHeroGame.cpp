#include "BeeperHeroGame.h"
#include "src/config/settings.h"

// Global game instance
BeeperHeroGame beeperHeroGame(nullptr, nullptr, nullptr);

BeeperHeroGame::BeeperHeroGame(UIManager* uiManager, ButtonManager* buttonManager, RingtonePlayer* ringtonePlayer) {
    this->uiManager = uiManager;
    this->buttonManager = buttonManager;
    this->ringtonePlayer = ringtonePlayer;
    
    // Initialize game state
    currentState = GameState::MENU;
    currentSong = nullptr;
    songLoaded = false;
    songStartTime = 0;
    
    // Initialize note management
    noteCount = 0;
    currentNoteIndex = 0;
    
    // Initialize performance
    performance.score = 0;
    performance.combo = 0;
    performance.maxCombo = 0;
    performance.notesHit = 0;
    performance.notesMissed = 0;
    performance.totalNotes = 0;
    performance.accuracy = 0.0f;
    performance.gameTime = 0;
    performance.startTime = 0;
    
    // Initialize visual state
    lastRenderTime = 0;
    needsRender = true;
    
    // Initialize button states
    for (int i = 0; i < 3; i++) {
        buttonStates[i] = false;
        lastButtonStates[i] = false;
    }
}

BeeperHeroGame::~BeeperHeroGame() {
    // Cleanup if needed
}

void BeeperHeroGame::begin() {
    Serial.println("BeeperHeroGame initialized");
    reset();
}

void BeeperHeroGame::start() {
    if (currentState == GameState::MENU || currentState == GameState::GAME_OVER) {
        if (!songLoaded) {
            // Load default song if none loaded
            loadSong("Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p");
        }
    }
    
    if (songLoaded) {
        currentState = GameState::PLAYING;
        performance.startTime = millis();
        songStartTime = millis();
        
        // Start music playback
        if (ringtonePlayer) {
            ringtonePlayer->playRingtone(currentSong);
        }
        
        Serial.println("BeeperHero game started");
    }
}

void BeeperHeroGame::pause() {
    if (currentState == GameState::PLAYING) {
        currentState = GameState::PAUSED;
        
        // Pause music
        if (ringtonePlayer) {
            ringtonePlayer->pause();
        }
        
        Serial.println("BeeperHero game paused");
    }
}

void BeeperHeroGame::resume() {
    if (currentState == GameState::PAUSED) {
        currentState = GameState::PLAYING;
        
        // Resume music
        if (ringtonePlayer) {
            ringtonePlayer->resume();
        }
        
        Serial.println("BeeperHero game resumed");
    }
}

void BeeperHeroGame::stop() {
    currentState = GameState::GAME_OVER;
    
    // Stop music
    if (ringtonePlayer) {
        ringtonePlayer->stop();
    }
    
    // Calculate final performance
    updatePerformance();
    
    Serial.println("BeeperHero game stopped");
}

void BeeperHeroGame::reset() {
    currentState = GameState::MENU;
    clearNotes();
    performance.score = 0;
    performance.combo = 0;
    performance.maxCombo = 0;
    performance.notesHit = 0;
    performance.notesMissed = 0;
    performance.totalNotes = 0;
    performance.accuracy = 0.0f;
    performance.gameTime = 0;
    performance.startTime = 0;
    
    needsRender = true;
}

void BeeperHeroGame::update() {
    // Update game state based on current state
    updateGameState();
    
    // Update notes and check for hits
    if (currentState == GameState::PLAYING) {
        updateNotes();
        checkNoteHits();
        processMissedNotes();
        updatePerformance();
    }
    
    // Render if needed
    if (needsRender || (millis() - lastRenderTime >= 33)) { // ~30 FPS
        render();
        lastRenderTime = millis();
        needsRender = false;
    }
}

void BeeperHeroGame::updateGameState() {
    // Check if game should end
    if (currentState == GameState::PLAYING) {
        // Check if song is finished
        if (ringtonePlayer && !ringtonePlayer->isPlaying()) {
            if (currentNoteIndex >= noteCount) {
                currentState = GameState::RESULTS;
                Serial.println("Song finished - showing results");
            }
        }
        
        // Check if all notes have been processed
        if (currentNoteIndex >= noteCount && ringtonePlayer && !ringtonePlayer->isPlaying()) {
            currentState = GameState::RESULTS;
        }
    }
}

void BeeperHeroGame::updateNotes() {
    // TODO: Update notes from AnyRtttl when library is added
    // For now, this is a placeholder implementation
    
    if (!ringtonePlayer || !ringtonePlayer->isPlaying()) return;
    
    // Get current note info from ringtone player
    if (ringtonePlayer->hasNoteInfo()) {
        NoteInfo noteInfo = ringtonePlayer->getCurrentNote();
        
        // Map frequency to lane
        NoteLane lane = mapFrequencyToLane(noteInfo.frequency);
        
        // Add note if it's not a rest and we have space
        if (!noteInfo.isRest && noteCount < MAX_NOTES) {
            addNote(lane, noteInfo.startTime, noteInfo.duration, noteInfo.frequency);
        }
    }
}

void BeeperHeroGame::checkNoteHits() {
    unsigned long gameTime = getGameTime();
    
    // Check each note for hits
    for (int i = currentNoteIndex; i < noteCount; i++) {
        GameNote& note = notes[i];
        
        if (note.hit || note.missed) continue;
        
        // Check if note is in hit window
        if (isNoteInHitWindow(note)) {
            // Check if corresponding button is pressed
            int buttonIndex = getLaneButton(note.lane);
            if (buttonIndex >= 0 && buttonIndex < 3 && buttonStates[buttonIndex]) {
                // Note hit!
                note.hit = true;
                int timing = getHitTiming(note);
                int score = calculateScore(timing);
                updateScore(score);
                updateCombo(true);
                
                Serial.printf("Note hit! Lane: %d, Timing: %d, Score: %d\n", 
                             (int)note.lane, timing, score);
            }
        }
        
        // Check if note is missed
        if (gameTime > note.time + HIT_WINDOW) {
            note.missed = true;
            updateCombo(false);
            Serial.printf("Note missed! Lane: %d\n", (int)note.lane);
        }
    }
}

void BeeperHeroGame::processMissedNotes() {
    // Process any notes that have been missed
    for (int i = currentNoteIndex; i < noteCount; i++) {
        if (notes[i].missed && !notes[i].processed) {
            // Handle missed note
            notes[i].processed = true;
        }
    }
}

void BeeperHeroGame::updatePerformance() {
    if (currentState == GameState::PLAYING) {
        performance.gameTime = getGameTime();
        calculateAccuracy();
    }
}

NoteLane BeeperHeroGame::mapFrequencyToLane(uint16_t frequency) const {
    // Map frequency to game lanes
    if (frequency < 440) {        // A4 and below
        return NoteLane::BOTTOM;  // Lower notes = bottom button
    } else if (frequency < 880) { // A4 to A5
        return NoteLane::MIDDLE;  // Middle notes = middle button
    } else {                      // A5 and above
        return NoteLane::TOP;     // Higher notes = top button
    }
}

int BeeperHeroGame::calculateScore(int timing) const {
    // Calculate score based on timing accuracy
    if (abs(timing) <= PERFECT_WINDOW) {
        return 100; // Perfect hit
    } else if (abs(timing) <= HIT_WINDOW) {
        return 50;  // Good hit
    }
    return 0; // Miss
}

void BeeperHeroGame::updateCombo(bool hit) {
    if (hit) {
        performance.combo++;
        if (performance.combo > performance.maxCombo) {
            performance.maxCombo = performance.combo;
        }
    } else {
        resetCombo();
    }
}

void BeeperHeroGame::resetCombo() {
    performance.combo = 0;
}

void BeeperHeroGame::loadSong(const char* rtttl) {
    currentSong = rtttl;
    songLoaded = true;
    clearNotes();
    
    Serial.printf("Song loaded: %s\n", rtttl);
}

void BeeperHeroGame::unloadSong() {
    currentSong = nullptr;
    songLoaded = false;
    clearNotes();
}

void BeeperHeroGame::addNote(NoteLane lane, unsigned long time, unsigned long duration, uint16_t frequency, bool isRest) {
    if (noteCount < MAX_NOTES) {
        notes[noteCount].lane = lane;
        notes[noteCount].time = time;
        notes[noteCount].duration = duration;
        notes[noteCount].frequency = frequency;
        notes[noteCount].hit = false;
        notes[noteCount].missed = false;
        notes[noteCount].isRest = isRest;
        notes[noteCount].processed = false;
        noteCount++;
        performance.totalNotes++;
    }
}

void BeeperHeroGame::clearNotes() {
    noteCount = 0;
    currentNoteIndex = 0;
    performance.totalNotes = 0;
}

void BeeperHeroGame::handleButtonPress(NoteLane lane) {
    int buttonIndex = getLaneButton(lane);
    if (buttonIndex >= 0 && buttonIndex < 3) {
        buttonStates[buttonIndex] = true;
    }
}

void BeeperHeroGame::handleButtonRelease(NoteLane lane) {
    int buttonIndex = getLaneButton(lane);
    if (buttonIndex >= 0 && buttonIndex < 3) {
        buttonStates[buttonIndex] = false;
    }
}

void BeeperHeroGame::render() {
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::PLAYING:
            renderGame();
            break;
        case GameState::PAUSED:
            renderPaused();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::RESULTS:
            renderResults();
            break;
    }
}

// Placeholder render methods - will be implemented with actual UI
void BeeperHeroGame::renderMenu() {
    // TODO: Implement menu rendering
}

void BeeperHeroGame::renderGame() {
    // TODO: Implement game rendering
}

void BeeperHeroGame::renderPaused() {
    // TODO: Implement paused rendering
}

void BeeperHeroGame::renderGameOver() {
    // TODO: Implement game over rendering
}

void BeeperHeroGame::renderResults() {
    // TODO: Implement results rendering
}

// Getter methods
GameState BeeperHeroGame::getState() const { return currentState; }
bool BeeperHeroGame::isPlaying() const { return currentState == GameState::PLAYING; }
bool BeeperHeroGame::isPaused() const { return currentState == GameState::PAUSED; }
bool BeeperHeroGame::isGameOver() const { return currentState == GameState::GAME_OVER; }
const GamePerformance& BeeperHeroGame::getPerformance() const { return performance; }
int BeeperHeroGame::getScore() const { return performance.score; }
int BeeperHeroGame::getCombo() const { return performance.combo; }
int BeeperHeroGame::getMaxCombo() const { return performance.maxCombo; }
float BeeperHeroGame::getAccuracy() const { return performance.accuracy; }
const char* BeeperHeroGame::getCurrentSong() const { return currentSong; }
bool BeeperHeroGame::isSongLoaded() const { return songLoaded; }
int BeeperHeroGame::getNoteCount() const { return noteCount; }
const GameNote* BeeperHeroGame::getNotes() const { return notes; }

// Private helper methods
unsigned long BeeperHeroGame::getGameTime() const {
    if (currentState == GameState::PLAYING) {
        return millis() - performance.startTime;
    }
    return 0;
}

bool BeeperHeroGame::isNoteInHitWindow(const GameNote& note) const {
    unsigned long gameTime = getGameTime();
    return (gameTime >= note.time - HIT_WINDOW) && (gameTime <= note.time + HIT_WINDOW);
}

int BeeperHeroGame::getHitTiming(const GameNote& note) const {
    unsigned long gameTime = getGameTime();
    return (int)(gameTime - note.time);
}

NoteLane BeeperHeroGame::getButtonLane(int buttonIndex) const {
    switch (buttonIndex) {
        case 0: return NoteLane::TOP;
        case 1: return NoteLane::MIDDLE;
        case 2: return NoteLane::BOTTOM;
        default: return NoteLane::MIDDLE;
    }
}

int BeeperHeroGame::getLaneButton(NoteLane lane) const {
    switch (lane) {
        case NoteLane::TOP: return 0;
        case NoteLane::MIDDLE: return 1;
        case NoteLane::BOTTOM: return 2;
        default: return 1;
    }
}

void BeeperHeroGame::calculateAccuracy() {
    if (performance.totalNotes > 0) {
        performance.accuracy = (float)performance.notesHit / performance.totalNotes * 100.0f;
    }
}

void BeeperHeroGame::updateScore(int points) {
    performance.score += points;
    performance.notesHit++;
}

void BeeperHeroGame::setNeedsRender(bool needs) {
    needsRender = needs;
}
