#ifndef BEEPER_HERO_GAME_SCREEN_H
#define BEEPER_HERO_GAME_SCREEN_H

#include "../core/GameScreen.h"
#include "../core/StandardGameLayout.h"
#include "../../ringtones/RingtonePlayer.h"
#include "../../games/beeperhero/BeeperHeroTrack.h"
#include "../components/MenuContainer.h"

class BeeperHeroScreen : public GameScreen {
public:
    BeeperHeroScreen(Adafruit_ST7789* display);
    ~BeeperHeroScreen() override = default;

    void enter() override;
    void exit() override;
    void handleButtonPress(int button) override;

protected:
    void updateGame() override;
    void drawGame() override;
    void drawStatic() override;
    void cleanup() override;

private:
    enum GameState { SONG_SELECT = 0, COUNTDOWN, PLAYING, GAME_OVER };

    static const uint8_t NUM_LANES = 3;
    // Horizontal lanes (rows)
    static const int LANE_HEIGHT = StandardGameLayout::PLAY_AREA_HEIGHT / NUM_LANES;
    static const int LANE_WIDTH = StandardGameLayout::PLAY_AREA_WIDTH;
    static const int HIT_LINE_X = StandardGameLayout::PLAY_AREA_LEFT + 10;

    bool lanesDirty[NUM_LANES];
    int laneMinX[NUM_LANES];
    int laneMaxX[NUM_LANES];

    void markLaneDirty(uint8_t lane, int x, int width);
    void clearDirtyLanes();
    void drawLanes();
    void drawHitLine();

    struct Note { uint8_t lane; int x; int prevX; int width; bool active; };
    static const int MAX_ACTIVE_NOTES = 20;
    Note notes[MAX_ACTIVE_NOTES];
    int noteSpeedPx = 3;

    int score = 0;
    int combo = 0;
    void updateNotes();
    void drawNotes();
    void tryHitLane(uint8_t lane);
    void removeNoteAt(int idx);

    // Game flow
    GameState state = SONG_SELECT;
    int selectedSongIndex = 0;
    unsigned long countdownStartMs = 0;

    // Track-driven spawning
    BeeperHeroTrack track;
    uint16_t nextNoteIndex = 0;
    static const unsigned long NOTE_APPROACH_TIME_MS = 2000; // ms
    void resetGameplay();
    void spawnDueNotes(unsigned long playbackMs);
    void drawSelectionUI();
    void drawCountdownUI();
    void drawGameOverUI();
    void startCountdownForIndex(int index);
    void buildSongSelectionMenu();

    RingtonePlayer player;
    MenuContainer* songMenu = nullptr;
};

#endif // BEEPER_HERO_GAME_SCREEN_H


