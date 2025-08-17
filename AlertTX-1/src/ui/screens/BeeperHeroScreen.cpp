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
    // Map buttons to lanes for demo
    if (!started) {
        // Start a default melody when first interaction occurs
        player.playRingtone(ringtone_data::Mario);
        started = true;
    }
}

void BeeperHeroScreen::updateGame() {
    // Drive audio
    player.update();

    // Demo note visualization: mark random lane dirty regions periodically
    // In a full implementation, translate current note timing to Y positions
    static unsigned long lastMark = 0;
    unsigned long now = millis();
    if (now - lastMark > 120) {
        lastMark = now;
        uint8_t lane = random(0, NUM_LANES);
        int y = StandardGameLayout::PLAY_AREA_TOP + random(0, LANE_HEIGHT - 6);
        int h = 6;
        markLaneDirty(lane, y, h);
    }
}

void BeeperHeroScreen::drawGame() {
    clearDirtyLanes();
    // Draw demo falling notes could be placed here using RenderBatch
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