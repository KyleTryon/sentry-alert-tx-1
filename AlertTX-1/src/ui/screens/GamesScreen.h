#ifndef GAMESSCREEN_H
#define GAMESSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../components/MenuContainer.h"

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
    MenuContainer* gamesMenu;
    static GamesScreen* instance;
    
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
    void setupMenu();
    static void pongCallback();
    static void snakeCallback();
    static void beeperHeroCallback();
    void onPongSelected();
    void onSnakeSelected();
    void onBeeperHeroSelected();
    void navigateToPong();
    void navigateToSnake();
    void navigateToBeeperHero();
};

#endif // GAMESSCREEN_H
