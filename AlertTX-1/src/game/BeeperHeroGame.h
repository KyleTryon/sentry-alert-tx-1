#ifndef BEEPERHEROGAME_H
#define BEEPERHEROGAME_H

#include <Arduino.h>
#include "../ui/UIManager.h"
#include "../hardware/ButtonManager.h"
#include "../ringtones/RingtonePlayer.h"

// Game state enumeration
enum class GameState {
    MENU,           // Song selection menu
    PLAYING,        // Game is active
    PAUSED,         // Game is paused
    GAME_OVER,      // Game finished
    RESULTS         // Showing results
};

// Note lane enumeration
enum class NoteLane {
    TOP,        // Top lane (LEFT_UP button)
    MIDDLE,     // Middle lane (LEFT_MID button)
    BOTTOM      // Bottom lane (LEFT_DOWN button)
};

// Note structure for game
struct GameNote {
    NoteLane lane;           // Which lane the note belongs to
    unsigned long time;      // When the note should be hit (milliseconds)
    unsigned long duration;  // Note duration
    uint16_t frequency;      // Note frequency
    bool hit;               // Whether note was successfully hit
    bool missed;            // Whether note was missed
    bool isRest;            // Whether this is a rest/silence
};

// Game performance structure
struct GamePerformance {
    int score;
    int combo;
    int maxCombo;
    int notesHit;
    int notesMissed;
    int totalNotes;
    float accuracy;
    unsigned long gameTime;
    unsigned long startTime;
};

class BeeperHeroGame {
private:
    // System dependencies
    UIManager* uiManager;
    ButtonManager* buttonManager;
    RingtonePlayer* ringtonePlayer;
    
    // Game state
    GameState currentState;
    GamePerformance performance;
    
    // Song management
    const char* currentSong;
    unsigned long songStartTime;
    bool songLoaded;
    
    // Note management
    static const int MAX_NOTES = 100; // Maximum notes to track
    GameNote notes[MAX_NOTES];
    int noteCount;
    int currentNoteIndex;
    
    // Timing and scoring
    static const int HIT_WINDOW = 150; // ms window for "good" hit
    static const int PERFECT_WINDOW = 50; // ms window for "perfect" hit
    static const int NOTE_SCROLL_TIME = 2000; // ms for note to scroll across screen
    
    // Visual state
    unsigned long lastRenderTime;
    bool needsRender;
    
    // Button state tracking
    bool buttonStates[3]; // TOP, MIDDLE, BOTTOM
    bool lastButtonStates[3];

public:
    BeeperHeroGame(UIManager* uiManager, ButtonManager* buttonManager, RingtonePlayer* ringtonePlayer);
    ~BeeperHeroGame();
    
    // Game lifecycle
    void begin();
    void start();
    void pause();
    void resume();
    void stop();
    void reset();
    
    // Main update loop
    void update();
    
    // Game state queries
    GameState getState() const;
    bool isPlaying() const;
    bool isPaused() const;
    bool isGameOver() const;
    
    // Performance queries
    const GamePerformance& getPerformance() const;
    int getScore() const;
    int getCombo() const;
    int getMaxCombo() const;
    float getAccuracy() const;
    
    // Song management
    void loadSong(const char* rtttl);
    void unloadSong();
    const char* getCurrentSong() const;
    bool isSongLoaded() const;
    
    // Note management
    void addNote(NoteLane lane, unsigned long time, unsigned long duration, uint16_t frequency, bool isRest = false);
    void clearNotes();
    int getNoteCount() const;
    const GameNote* getNotes() const;
    
    // Input handling
    void handleButtonPress(NoteLane lane);
    void handleButtonRelease(NoteLane lane);
    
    // Rendering
    void render();
    void setNeedsRender(bool needs);
    
    // Utility functions
    NoteLane mapFrequencyToLane(uint16_t frequency) const;
    int calculateScore(int timing) const;
    void updateCombo(bool hit);
    void resetCombo();
    
private:
    // Internal game logic
    void updateGameState();
    void updateNotes();
    void checkNoteHits();
    void processMissedNotes();
    void updatePerformance();
    
    // Note timing
    unsigned long getGameTime() const;
    bool isNoteInHitWindow(const GameNote& note) const;
    int getHitTiming(const GameNote& note) const;
    
    // Button mapping
    NoteLane getButtonLane(int buttonIndex) const;
    int getLaneButton(NoteLane lane) const;
    
    // Visual helpers
    void renderMenu();
    void renderGame();
    void renderPaused();
    void renderGameOver();
    void renderResults();
    
    // Performance calculation
    void calculateAccuracy();
    void updateScore(int points);
};

// Global game instance
extern BeeperHeroGame beeperHeroGame;

#endif // BEEPERHEROGAME_H
