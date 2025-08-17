#include "BeeperHeroScreen.h"
#include "../core/Theme.h"
#include "../core/ScreenManager.h"
#include "../../config/settings.h"
#include <limits.h>

BeeperHeroScreen::BeeperHeroScreen(Adafruit_ST7789* display)
    : GameScreen(display, "BeeperHero", 44) {
    for (uint8_t i = 0; i < NUM_LANES; ++i) {
        lanesDirty[i] = true;
        laneMinY[i] = INT_MAX;
        laneMaxY[i] = 0;
    }
}

void BeeperHeroScreen::enter() {
    GameScreen::enter();
    player.begin(/*buzzerPin*/ BUZZER_PIN);
    started = false;
    staticBackgroundCached = false;
}

void BeeperHeroScreen::exit() {
    GameScreen::exit();
}

void BeeperHeroScreen::cleanup() {
    player.stop();
}

void BeeperHeroScreen::handleButtonPress(int button) {
    // Start playback on first interaction; map buttons to lane hits
    if (!started) {
        // Start by name (stub prints for now); visuals run via synthetic fallback
        player.playRingtoneByName("Mario");
        started = true;
        return;
    }
    if (button == ButtonInput::BUTTON_A) tryHitLane(0);
    else if (button == ButtonInput::BUTTON_B) tryHitLane(1);
    else if (button == ButtonInput::BUTTON_C) tryHitLane(2);
}

void BeeperHeroScreen::updateGame() {
    // Drive audio and update current note info
    player.update();

    // Spawn notes when the player reports a new note; otherwise synthetic fallback
    bool spawned = false;
    if (player.hasNoteInfo()) {
        unsigned long start = player.getNoteStartTime();
        if (start != lastNoteStartTime) {
            lastNoteStartTime = start;
            spawnNoteFromCurrent();
            spawned = true;
        }
    }
    if (!spawned) {
        static unsigned long lastSynthetic = 0;
        unsigned long now = millis();
        if (now - lastSynthetic >= 200) {
            lastSynthetic = now;
            // Random lane on synthetic tick
            uint8_t lane = random(0, NUM_LANES);
            for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
                if (!notes[i].active) {
                    notes[i].active = true;
                    notes[i].lane = lane;
                    notes[i].height = 6;
                    notes[i].prevY = StandardGameLayout::PLAY_AREA_TOP;
                    notes[i].y = StandardGameLayout::PLAY_AREA_TOP;
                    markLaneDirty(lane, notes[i].y, notes[i].height);
                    break;
                }
            }
        }
    }

    // Move existing notes downward and mark dirty lanes
    updateNotes();
}

void BeeperHeroScreen::drawGame() {
    // Clear only dirty lane regions, then draw active notes
    clearDirtyLanes();
    drawNotes();
}

void BeeperHeroScreen::drawStatic() {
    // Header and lanes
    StandardGameLayout::drawGameHeader(display, "BeeperHero");
    drawLanes();
    drawHitLine();
}

void BeeperHeroScreen::markLaneDirty(uint8_t lane, int y, int height) {
    if (lane >= NUM_LANES) return;
    lanesDirty[lane] = true;
    laneMinY[lane] = min(laneMinY[lane], y);
    laneMaxY[lane] = max(laneMaxY[lane], y + height);
}

void BeeperHeroScreen::clearDirtyLanes() {
    for (uint8_t lane = 0; lane < NUM_LANES; ++lane) {
        if (lanesDirty[lane]) {
            int x = StandardGameLayout::PLAY_AREA_LEFT + lane * LANE_WIDTH;
            int y = laneMinY[lane] == INT_MAX ? StandardGameLayout::PLAY_AREA_TOP : laneMinY[lane];
            int h = (laneMaxY[lane] - y);
            if (h <= 0) h = 1;
            display->fillRect(x, y, LANE_WIDTH, h, ThemeManager::getBackground());

            lanesDirty[lane] = false;
            laneMinY[lane] = INT_MAX;
            laneMaxY[lane] = 0;
        }
    }
}

void BeeperHeroScreen::drawLanes() {
    uint16_t border = ThemeManager::getBorder();
    for (int i = 1; i < NUM_LANES; ++i) {
        int x = StandardGameLayout::PLAY_AREA_LEFT + (i * LANE_WIDTH);
        display->drawFastVLine(x, StandardGameLayout::PLAY_AREA_TOP, LANE_HEIGHT, border);
    }
}

void BeeperHeroScreen::drawHitLine() {
    display->drawFastHLine(StandardGameLayout::PLAY_AREA_LEFT, HIT_LINE_Y,
                           StandardGameLayout::PLAY_AREA_WIDTH, ThemeManager::getAccent());
}

uint8_t BeeperHeroScreen::laneFromFrequency(uint16_t freq) const {
    // Simple split: low/mid/high -> lanes 0..2
    if (freq < 523) return 0;      // < C5
    if (freq < 784) return 1;      // < G5
    return 2;
}

void BeeperHeroScreen::spawnNoteFromCurrent() {
    if (!player.hasNoteInfo()) return;
    uint8_t lane = laneFromFrequency(player.getCurrentFrequency());
    // Find a free slot
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active) {
            notes[i].active = true;
            notes[i].lane = lane;
            notes[i].height = 6;
            notes[i].prevY = StandardGameLayout::PLAY_AREA_TOP;
            notes[i].y = StandardGameLayout::PLAY_AREA_TOP;
            markLaneDirty(lane, notes[i].y, notes[i].height);
            break;
        }
    }
}

void BeeperHeroScreen::updateNotes() {
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active) continue;
        notes[i].prevY = notes[i].y;
        notes[i].y += noteSpeedPx;
        if (notes[i].y > StandardGameLayout::PLAY_AREA_BOTTOM) {
            // Missed
            notes[i].active = false;
            combo = 0;
            // Mark lane region dirty: from previous to bottom
            markLaneDirty(notes[i].lane, notes[i].prevY, StandardGameLayout::PLAY_AREA_BOTTOM - notes[i].prevY);
        } else {
            // Mark both previous and new positions as dirty spans
            int minY = min(notes[i].prevY, notes[i].y);
            int maxY = max(notes[i].prevY + notes[i].height, notes[i].y + notes[i].height);
            markLaneDirty(notes[i].lane, minY, maxY - minY);
        }
    }
}

void BeeperHeroScreen::drawNotes() {
    uint16_t color = ThemeManager::getPrimaryText();
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active) continue;
        int x = StandardGameLayout::PLAY_AREA_LEFT + notes[i].lane * LANE_WIDTH + 4;
        int w = LANE_WIDTH - 8;
        display->fillRect(x, notes[i].y, w, notes[i].height, color);
    }
}

void BeeperHeroScreen::tryHitLane(uint8_t lane) {
    // Accept if a note crosses the hit window near HIT_LINE_Y
    const int HIT_WINDOW = 8;
    for (int i = 0; i < MAX_ACTIVE_NOTES; ++i) {
        if (!notes[i].active || notes[i].lane != lane) continue;
        int center = notes[i].y + notes[i].height / 2;
        if (abs(center - HIT_LINE_Y) <= HIT_WINDOW) {
            score += 50;
            combo += 1;
            // Clear the note area by marking lane dirty and deactivating
            markLaneDirty(lane, notes[i].y, notes[i].height);
            notes[i].active = false;
            // Update header with score
            StandardGameLayout::drawGameHeader(display, "BeeperHero", score, "Score");
            break;
        }
    }
}