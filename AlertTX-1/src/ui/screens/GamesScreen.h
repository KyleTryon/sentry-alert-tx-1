#ifndef GAMESSCREEN_H
#define GAMESSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"

/**
 * GamesScreen
 * 
 * Demo screen for games functionality. Shows available games
 * and game-related information.
 * 
 * Features:
 * - Display available games
 * - Show game descriptions
 * - Long press for back navigation
 * - Future: game selection and launching
 */

class GamesScreen : public Screen {
private:
    // Display content
    static const char* SCREEN_TITLE;
    static const char* GAME_LIST[];
    static const char* GAME_DESCRIPTIONS[];
    static const int GAME_COUNT;
    
    // Layout constants
    static const int LINE_HEIGHT = 14;
    static const int START_Y = 40;
    static const int GAME_INDENT = 20;
    static const int DESC_INDENT = 25;
    
public:
    GamesScreen(Adafruit_ST7789* display);
    ~GamesScreen();
    
    // Screen lifecycle
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    // Input handling
    void handleButtonPress(int button) override;
    
private:
    // Drawing helpers
    void drawContent();
    void drawGame(int gameIndex, int& currentY);
};

#endif // GAMESSCREEN_H
