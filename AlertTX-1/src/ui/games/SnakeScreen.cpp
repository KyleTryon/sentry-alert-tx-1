#include "SnakeScreen.h"
#include "../core/Theme.h"
#include "../core/ScreenManager.h"

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
    if (now - lastStepMs < stepIntervalMs) return;
    lastStepMs = now;
    stepOnce();
}

void SnakeScreen::drawGame() {
    // Draw incremental updates: head, body, clear tail if needed
    uint16_t snakeColor = ThemeManager::getAccent();
    uint16_t bg = ThemeManager::getBackground();

    // Draw head
    drawCell(snakeX[0], snakeY[0], snakeColor);
    for (int i = 1; i < snakeLength; ++i) {
        drawCell(snakeX[i], snakeY[i], snakeColor);
    }
    if (!grewLastStep) {
        int tailIdx = snakeLength;
        drawCell(snakeX[tailIdx], snakeY[tailIdx], bg);
    }
}

void SnakeScreen::drawStatic() {
    drawHeader();
    drawGrid();
}

void SnakeScreen::handleButtonPress(int button) {
    if (button == ButtonInput::BUTTON_A) { // Up
        if (dirY == 0) { dirX = 0; dirY = -1; }
    } else if (button == ButtonInput::BUTTON_B) { // Down
        if (dirY == 0) { dirX = 0; dirY = 1; }
    } else if (button == ButtonInput::BUTTON_C) { // Toggle horizontal
        if (dirX == 0) { dirX = 1; dirY = 0; } else { dirX = -1; dirY = 0; }
    }
}

void SnakeScreen::placeFood() {
    foodX = random(0, GRID_COLS);
    foodY = random(0, GRID_ROWS);
}

void SnakeScreen::drawHeader() {
    int score = (snakeLength - 6) * 10;
    StandardGameLayout::drawGameHeader(display, "Snake", score);
}

void SnakeScreen::drawGrid() {
    StandardGameLayout::drawPlayAreaBorder(display, ThemeManager::getBorder());
    drawCell(foodX, foodY, ThemeManager::getPrimaryText());
}

void SnakeScreen::drawCell(int gx, int gy, uint16_t color) {
    int x = StandardGameLayout::PLAY_AREA_LEFT + gx * CELL_SIZE;
    int y = StandardGameLayout::PLAY_AREA_TOP + gy * CELL_SIZE;
    display->fillRect(x, y, CELL_SIZE, CELL_SIZE, color);
}

void SnakeScreen::stepOnce() {
    for (int i = snakeLength; i > 0; --i) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    if (snakeX[0] < 0) snakeX[0] = GRID_COLS - 1;
    if (snakeX[0] >= GRID_COLS) snakeX[0] = 0;
    if (snakeY[0] < 0) snakeY[0] = GRID_ROWS - 1;
    if (snakeY[0] >= GRID_ROWS) snakeY[0] = 0;

    grewLastStep = false;
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength = min(snakeLength + 1, 127);
        grewLastStep = true;
        placeFood();
        drawHeader();
    }
}