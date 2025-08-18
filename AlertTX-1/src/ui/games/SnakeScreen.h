#ifndef SNAKE_GAME_SCREEN_H
#define SNAKE_GAME_SCREEN_H

#include "../core/GameScreen.h"
#include "../core/StandardGameLayout.h"

class SnakeScreen : public GameScreen {
public:
    SnakeScreen(Adafruit_ST7789* display);
    ~SnakeScreen() override = default;

    void enter() override;
    void handleButtonPress(int button) override;

protected:
    void updateGame() override;
    void drawGame() override;
    void drawStatic() override;

private:
    // Grid config
    static const int CELL_SIZE = 6;
    static const int GRID_COLS = StandardGameLayout::PLAY_AREA_WIDTH / CELL_SIZE;
    static const int GRID_ROWS = StandardGameLayout::PLAY_AREA_HEIGHT / CELL_SIZE;

    // Snake state
    static const int MAX_SNAKE_LEN = GRID_COLS * GRID_ROWS;
    int snakeX[MAX_SNAKE_LEN];
    int snakeY[MAX_SNAKE_LEN];
    int snakeLength;
    int dirX;
    int dirY;
    int foodX;
    int foodY;
    bool grewLastStep;
    bool paused = false;

    // Timing
    unsigned long lastStepMs;
    unsigned long stepIntervalMs;

    // Methods
    void resetGame();
    void placeFood();
    bool isOnSnake(int gx, int gy) const;
    void drawHeader();
    void drawGrid();
    void drawCell(int gx, int gy, uint16_t color);
    void stepOnce();
};

#endif // SNAKE_GAME_SCREEN_H


