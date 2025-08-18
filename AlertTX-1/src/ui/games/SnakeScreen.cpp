#include "SnakeScreen.h"
#include "../core/Theme.h"

SnakeScreen::SnakeScreen(Adafruit_ST7789* display)
    : GameScreen(display, "Snake", 43), snakeLength(6), dirX(1), dirY(0), grewLastStep(false),
      lastStepMs(0), stepIntervalMs(120) {
    int startX = (StandardGameLayout::PLAY_AREA_WIDTH / 2) / CELL_SIZE;
    int startY = (StandardGameLayout::PLAY_AREA_HEIGHT / 2) / CELL_SIZE;
    for (int i = 0; i < snakeLength; ++i) {
        snakeX[i] = startX - i;
        snakeY[i] = startY;
    }
    placeFood();
}

void SnakeScreen::enter() {
    GameScreen::enter();
    setTargetFPS(60);
}

void SnakeScreen::updateGame() {
    unsigned long now = millis();
    if (paused) return;
    if (now - lastStepMs < stepIntervalMs) return;
    lastStepMs = now;
    stepOnce();
}

void SnakeScreen::drawGame() {
    // Draw incremental updates: head/body and clear tail if needed
    uint16_t snakeColor = ThemeManager::getAccent();
    uint16_t bg = ThemeManager::getBackground();

    // Draw snake
    for (int i = 0; i < snakeLength; ++i) {
        drawCell(snakeX[i], snakeY[i], snakeColor);
    }
    // Draw food
    drawCell(foodX, foodY, ThemeManager::getPrimaryText());

    // Clear the trailing cell when we did not grow
    if (!grewLastStep) {
        int tailIdx = snakeLength;
        int x = StandardGameLayout::PLAY_AREA_LEFT + snakeX[tailIdx] * CELL_SIZE;
        int y = StandardGameLayout::PLAY_AREA_TOP + snakeY[tailIdx] * CELL_SIZE;
        display->fillRect(x, y, CELL_SIZE - 1, CELL_SIZE - 1, bg);
    }
}

void SnakeScreen::drawStatic() {
    drawHeader();
    drawGrid();
}

void SnakeScreen::handleButtonPress(int button) {
    if (button == ButtonInput::BUTTON_A) {
        // Turn left relative to current direction
        int oldDx = dirX, oldDy = dirY;
        dirX = -oldDy;
        dirY = oldDx;
    } else if (button == ButtonInput::BUTTON_B) {
        // Turn right
        int oldDx = dirX, oldDy = dirY;
        dirX = oldDy;
        dirY = -oldDx;
    } else if (button == ButtonInput::BUTTON_C) {
        paused = !paused;
    }
}

void SnakeScreen::resetGame() {
    snakeLength = 6;
    dirX = 1; dirY = 0;
    int startX = (StandardGameLayout::PLAY_AREA_WIDTH / 2) / CELL_SIZE;
    int startY = (StandardGameLayout::PLAY_AREA_HEIGHT / 2) / CELL_SIZE;
    for (int i = 0; i < snakeLength; ++i) {
        snakeX[i] = startX - i;
        snakeY[i] = startY;
    }
    placeFood();
    grewLastStep = false;
}

void SnakeScreen::placeFood() {
    int tries = 0;
    do {
        foodX = random(0, GRID_COLS);
        foodY = random(0, GRID_ROWS);
        tries++;
    } while (isOnSnake(foodX, foodY) && tries < 200);
}

bool SnakeScreen::isOnSnake(int gx, int gy) const {
    for (int i = 0; i < snakeLength; ++i) {
        if (snakeX[i] == gx && snakeY[i] == gy) return true;
    }
    return false;
}

void SnakeScreen::drawHeader() {
    int score = (snakeLength - 6) * 10;
    StandardGameLayout::drawGameHeader(display, "Snake", score);
}

void SnakeScreen::drawGrid() {
    uint16_t border = ThemeManager::getBorder();
    int w = StandardGameLayout::PLAY_AREA_WIDTH;
    int h = StandardGameLayout::PLAY_AREA_HEIGHT;
    display->drawRect(StandardGameLayout::PLAY_AREA_LEFT, StandardGameLayout::PLAY_AREA_TOP, w, h, border);
}

void SnakeScreen::drawCell(int gx, int gy, uint16_t color) {
    int x = StandardGameLayout::PLAY_AREA_LEFT + gx * CELL_SIZE;
    int y = StandardGameLayout::PLAY_AREA_TOP + gy * CELL_SIZE;
    display->fillRect(x, y, CELL_SIZE - 1, CELL_SIZE - 1, color);
}

void SnakeScreen::stepOnce() {
    // Compute next head with wrapping
    int nextX = (snakeX[0] + dirX + GRID_COLS) % GRID_COLS;
    int nextY = (snakeY[0] + dirY + GRID_ROWS) % GRID_ROWS;

    // Self-collision -> reset
    for (int i = 0; i < snakeLength; ++i) {
        if (snakeX[i] == nextX && snakeY[i] == nextY) {
            resetGame();
            return;
        }
    }

    // Shift body
    int tailPrevX = snakeX[snakeLength - 1];
    int tailPrevY = snakeY[snakeLength - 1];
    for (int i = snakeLength - 1; i > 0; --i) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] = nextX;
    snakeY[0] = nextY;

    // Food
    grewLastStep = false;
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        if (snakeLength < MAX_SNAKE_LEN) {
            snakeX[snakeLength] = tailPrevX;
            snakeY[snakeLength] = tailPrevY;
            snakeLength++;
            grewLastStep = true;
        }
        placeFood();
    }
}


