#include "GamesScreen.h"
#include "../core/ScreenManager.h"

// Static content definitions
const char* GamesScreen::SCREEN_TITLE = "Games";

const char* GamesScreen::GAME_LIST[] = {
    "Snake",
    "Pong", 
    "Memory",
    "Simon",
    "Breakout"
};

const char* GamesScreen::GAME_DESCRIPTIONS[] = {
    "Classic snake game",
    "Two-player paddle game",
    "Match the sequence",
    "Follow the pattern",
    "Break all the blocks"
};

const int GamesScreen::GAME_COUNT = 5;

GamesScreen::GamesScreen(Adafruit_ST7789* display)
    : Screen(display, "Games", 1) {
    Serial.println("GamesScreen created");
}

GamesScreen::~GamesScreen() {
    Serial.println("GamesScreen destroyed");
}

void GamesScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("GAMES");
    Serial.println("Entered GamesScreen");
}

void GamesScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("GAMES");
    Serial.println("Exited GamesScreen");
}

void GamesScreen::update() {
    Screen::update();
    // GamesScreen doesn't need special update logic
    // Could add game preview animations here
}

void GamesScreen::draw() {
    Screen::draw();
    
    // Draw title
    DisplayUtils::drawTitle(display, SCREEN_TITLE);
    
    // Draw content
    drawContent();
}

void GamesScreen::handleButtonPress(int button) {
    Serial.printf("GamesScreen: Button %d pressed\n", button);
    
    // Currently no specific button actions for GamesScreen
    // Long press back navigation is handled globally in main loop
    // Future: Could add game selection and launching actions here
    
    Serial.println("GamesScreen: No specific action for this button");
}

// Private helper methods

void GamesScreen::drawContent() {
    display->setTextSize(1);
    
    // Draw header text
    display->setTextColor(ThemeManager::getSecondaryText());
    display->setCursor(15, START_Y);
    display->println("Available Games:");
    
    int currentY = START_Y + 20;  // Start below header
    
    // Draw each game
    for (int i = 0; i < GAME_COUNT; i++) {
        drawGame(i, currentY);
    }
    
    // Draw footer instructions
    display->setTextColor(ThemeManager::getSecondaryText());
    display->setCursor(15, currentY + 10);
    display->println("Long press any button");
    display->setCursor(15, currentY + 22);
    display->println("to go back");
}

void GamesScreen::drawGame(int gameIndex, int& currentY) {
    if (gameIndex >= GAME_COUNT) return;
    
    // Game name in accent color
    display->setTextColor(ThemeManager::getAccent());
    display->setCursor(GAME_INDENT, currentY);
    display->printf("• %s", GAME_LIST[gameIndex]);
    
    currentY += LINE_HEIGHT;
    
    // Game description in primary text color
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setCursor(DESC_INDENT, currentY);
    display->println(GAME_DESCRIPTIONS[gameIndex]);
    
    currentY += LINE_HEIGHT + 2;  // Extra spacing between games
}
