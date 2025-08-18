#include "BeeperHeroScreen.h"
#include "../core/Theme.h"
#include "../../config/settings.h"
#include <limits.h>

BeeperHeroScreen::BeeperHeroScreen(Adafruit_ST7789* display)
    : GameScreen(display, "BeeperHero", 44) {
    for (uint8_t i = 0; i < NUM_LANES; ++i) {
        lanesDirty[i] = true;
        laneMinX[i] = INT_MAX;
        laneMaxX[i] = 0;
    }
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        notes[i].active = false;
        notes[i].x = notes[i].prevX = StandardGameLayout::PLAY_AREA_RIGHT;
        notes[i].width = 8;
        notes[i].lane = 0;
    }
}

void BeeperHeroScreen::enter() {
    GameScreen::enter();
    setTargetFPS(60);
    player.begin(BUZZER_PIN);
    staticBackgroundCached = false;
    state = SONG_SELECT;
    selectedSongIndex = 0;
    buildSongSelectionMenu();
    score = 0;
    combo = 0;
}

void BeeperHeroScreen::exit() {
    GameScreen::exit();
}

void BeeperHeroScreen::cleanup() {
    player.stop();
}

void BeeperHeroScreen::handleButtonPress(int button) {
    if (state == SONG_SELECT) {
        if (songMenu) songMenu->handleButtonPress(button);
        return;
    }

    if (state == PLAYING) {
        if (button == ButtonInput::BUTTON_A) tryHitLane(0);
        else if (button == ButtonInput::BUTTON_B) tryHitLane(1);
        else if (button == ButtonInput::BUTTON_C) tryHitLane(2);
    }

    if (state == GAME_OVER) {
        if (button == ButtonInput::BUTTON_B || button == ButtonInput::BUTTON_C || button == ButtonInput::BUTTON_A) {
            state = SONG_SELECT;
            markForFullRedraw();
        }
    }
}

void BeeperHeroScreen::updateGame() {
    if (state == SONG_SELECT) {
        return;
    }

    if (state == COUNTDOWN) {
        unsigned long now = millis();
        if (now - countdownStartMs >= 2000) {
            state = PLAYING;
            // Start audio playback now to sync with visuals (already began on C, we just continue)
        }
        return;
    }

    if (state == PLAYING) {
        player.update();
        unsigned long t = player.getPlaybackTime();
        spawnDueNotes(t);
        updateNotes();
        if (!player.isPlaying()) {
            state = GAME_OVER;
        }
    }
}

void BeeperHeroScreen::drawGame() {
    if (state == SONG_SELECT) {
        // Components handle drawing; mark redraw only when needed
        if (songMenu && songMenu->isDirty()) {
            Screen::draw();
        }
        return;
    }
    if (state == COUNTDOWN) {
        drawCountdownUI();
        return;
    }
    if (state == GAME_OVER) {
        drawGameOverUI();
        return;
    }
    clearDirtyLanes();
    drawNotes();
}

void BeeperHeroScreen::drawStatic() {
    StandardGameLayout::drawGameHeader(display, "BeeperHero");
    drawLanes();
    drawHitLine();
}

void BeeperHeroScreen::markLaneDirty(uint8_t lane, int x, int width) {
    if (lane >= NUM_LANES) return;
    lanesDirty[lane] = true;
    if (x < laneMinX[lane]) laneMinX[lane] = x;
    if (x + width > laneMaxX[lane]) laneMaxX[lane] = x + width;
}

void BeeperHeroScreen::clearDirtyLanes() {
    uint16_t bg = ThemeManager::getBackground();
    for (uint8_t lane = 0; lane < NUM_LANES; ++lane) {
        if (!lanesDirty[lane]) continue;
        int y = StandardGameLayout::PLAY_AREA_TOP + lane * LANE_HEIGHT;
        int x0 = laneMinX[lane];
        int x1 = laneMaxX[lane];
        if (x0 > x1) continue;
        int w = x1 - x0;
        display->fillRect(x0, y + 1, w, LANE_HEIGHT - 2, bg);
        lanesDirty[lane] = false;
        laneMinX[lane] = INT_MAX;
        laneMaxX[lane] = 0;
    }
}

void BeeperHeroScreen::drawLanes() {
    uint16_t border = ThemeManager::getBorder();
    for (int i = 0; i <= NUM_LANES; ++i) {
        int y = StandardGameLayout::PLAY_AREA_TOP + i * LANE_HEIGHT;
        display->drawFastHLine(StandardGameLayout::PLAY_AREA_LEFT, y, LANE_WIDTH, border);
    }
}

void BeeperHeroScreen::drawHitLine() {
    uint16_t accent = ThemeManager::getAccent();
    display->drawFastVLine(HIT_LINE_X, StandardGameLayout::PLAY_AREA_TOP, StandardGameLayout::PLAY_AREA_HEIGHT, accent);
}

void BeeperHeroScreen::resetGameplay() {
    // Clear notes
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        notes[i].active = false;
        notes[i].x = notes[i].prevX = StandardGameLayout::PLAY_AREA_RIGHT;
        notes[i].width = 8;
        notes[i].lane = 0;
    }
    nextNoteIndex = 0;
    score = 0;
    combo = 0;
}

void BeeperHeroScreen::spawnDueNotes(unsigned long playbackMs) {
    // Spawn any notes whose appear time has passed
    while (true) {
        const BeeperHeroNote* note = track.getNote(nextNoteIndex);
        if (!note) break;
        unsigned long appear = (note->startTime > NOTE_APPROACH_TIME_MS) ? (note->startTime - NOTE_APPROACH_TIME_MS) : 0;
        if (playbackMs < appear) break;
        // spawn
        for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
            if (!notes[i].active) {
                notes[i].active = true;
                notes[i].lane = note->lane % NUM_LANES;
                notes[i].width = 8;
                notes[i].prevX = StandardGameLayout::PLAY_AREA_RIGHT;
                notes[i].x = StandardGameLayout::PLAY_AREA_RIGHT;
                markLaneDirty(notes[i].lane, notes[i].x - notes[i].width - 2, notes[i].width + 4);
                break;
            }
        }
        nextNoteIndex++;
    }
}

void BeeperHeroScreen::updateNotes() {
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active) continue;
        notes[i].prevX = notes[i].x;
        notes[i].x -= noteSpeedPx;
        int delta = notes[i].prevX - notes[i].x;
        if (delta > 0) {
            // Only clear the trailing strip behind the moving note
            int clearX = notes[i].x;
            int clearW = delta;
            markLaneDirty(notes[i].lane, clearX, clearW);
        }
        if (notes[i].x + notes[i].width < StandardGameLayout::PLAY_AREA_LEFT) {
            removeNoteAt(i);
            combo = 0; // miss
        }
    }
}

void BeeperHeroScreen::drawNotes() {
    // Direct draw without relying on global batch flush
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active) continue;
        int x = notes[i].x - notes[i].width;
        int y = StandardGameLayout::PLAY_AREA_TOP + notes[i].lane * LANE_HEIGHT + 2;
        uint16_t color = ThemeManager::getPrimaryText();
        display->fillRect(x, y, notes[i].width, LANE_HEIGHT - 4, color);
    }
}

void BeeperHeroScreen::tryHitLane(uint8_t lane) {
    const int hitWidth = 8;
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active || notes[i].lane != lane) continue;
        int noteLeft = notes[i].x - notes[i].width;
        if (noteLeft <= HIT_LINE_X + hitWidth && noteLeft >= HIT_LINE_X - hitWidth) {
            score += 100;
            combo += 1;
            removeNoteAt(i);
            break;
        }
    }
}

void BeeperHeroScreen::removeNoteAt(int idx) {
    notes[idx].active = false;
}

void BeeperHeroScreen::drawSelectionUI() {
    // No-op: handled by MenuContainer component added to screen
}

void BeeperHeroScreen::drawCountdownUI() {
    StandardGameLayout::clearPlayArea(display, ThemeManager::getBackground());
    unsigned long elapsed = millis() - countdownStartMs;
    const char* text = (elapsed < 1500) ? "Get Ready" : "Go!";
    display->setTextColor(ThemeManager::getAccent());
    display->setTextSize(2);
    int16_t x1, y1; uint16_t w, h;
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    int x = StandardGameLayout::PLAY_AREA_LEFT + (StandardGameLayout::PLAY_AREA_WIDTH - (int)w) / 2;
    int y = StandardGameLayout::PLAY_AREA_TOP + (StandardGameLayout::PLAY_AREA_HEIGHT - (int)h) / 2;
    display->setCursor(x, y);
    display->print(text);
}

void BeeperHeroScreen::drawGameOverUI() {
    StandardGameLayout::clearPlayArea(display, ThemeManager::getBackground());
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    int y = StandardGameLayout::PLAY_AREA_TOP + 10;
    display->setCursor(StandardGameLayout::PLAY_AREA_LEFT + 8, y);
    display->print("Game Over");
    y += 14;
    display->setCursor(StandardGameLayout::PLAY_AREA_LEFT + 8, y);
    display->print("Score: ");
    display->print(score);
    y += 14;
    display->setCursor(StandardGameLayout::PLAY_AREA_LEFT + 8, y);
    display->print("Press B to select");
}

void BeeperHeroScreen::startCountdownForIndex(int index) {
    selectedSongIndex = index;
    resetGameplay();
    const uint8_t* trackData = getBeeperHeroTrackData(selectedSongIndex);
    size_t trackSize = getBeeperHeroTrackSize(selectedSongIndex);
    if (trackData && trackSize > 0 && track.loadFromMemory(trackData, trackSize)) {
        track.printTrackInfo();
    }
    player.stop();
    player.playRingtoneByIndex(selectedSongIndex);
    countdownStartMs = millis();
    state = COUNTDOWN;
    markForFullRedraw();
}

void BeeperHeroScreen::buildSongSelectionMenu() {
    // Remove existing
    if (songMenu) {
        removeComponent(songMenu);
        delete songMenu;
        songMenu = nullptr;
    }

    songMenu = new MenuContainer(display, StandardGameLayout::PLAY_AREA_LEFT + 8, StandardGameLayout::PLAY_AREA_TOP + 8);
    addComponent(songMenu);

    int count = player.getRingtoneCount();
    for (int i = 0; i < count && i < 8; i++) { // respect MAX_MENU_ITEMS
        const char* name = ::getRingtoneName(i);
        int idx = i; // capture by value
        songMenu->addMenuItem(name ? name : "(unnamed)", i, [this, idx]() {
            startCountdownForIndex(idx);
        });
    }
    songMenu->autoLayout();
}


