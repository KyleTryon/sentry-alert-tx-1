#include "SnakeScreen.h"
#include "../core/Theme.h"
#include "../core/ScreenManager.h"

SnakeScreen::SnakeScreen(Adafruit_ST7789* display)
    : Screen(display, "Snake", 43), snakeLength(6), dirX(1), dirY(0), grewLastStep(false),
      lastStepMs(0), stepIntervalMs(120) {
    // Initialize snake centered
    int startX = (StandardGameLayout::PLAY_AREA_WIDTH / 2) / CELL_SIZE;
    int startY = (StandardGameLayout::PLAY_AREA_HEIGHT / 2) / CELL_SIZE;
    for (int i = 0; i < snakeLength; ++i) {
        snakeX[i] = startX - i;
        snakeY[i] = startY;
    }
    placeFood();
}

void SnakeScreen::enter() {
    Screen::enter();
    StandardGameLayout::clearPlayArea(display, ThemeManager::getBackground());
    drawHeader();
    drawGrid();
}

void SnakeScreen::exit() {
    Screen::exit();
}

void SnakeScreen::update() {
    Screen::update();
    unsigned long now = millis();
    if (now - lastStepMs < stepIntervalMs) return;
    lastStepMs = now;
    step();
}

void SnakeScreen::draw() {
    if (!isActive()) return;
    // Draw incremental updates: head, body, clear tail if needed
    uint16_t snakeColor = ThemeManager::getAccent();
    uint16_t bg = ThemeManager::getBackground();

    // Draw head
    drawCell(snakeX[0], snakeY[0], snakeColor);
    // Draw body segments (optional incremental visual)
    for (int i = 1; i < snakeLength; ++i) {
        drawCell(snakeX[i], snakeY[i], snakeColor);
    }
    // Clear tail if not growing
    if (!grewLastStep) {
        int tailIdx = snakeLength;
        drawCell(snakeX[tailIdx], snakeY[tailIdx], bg);
    }
}

void SnakeScreen::handleButtonPress(int button) {
    if (button == ButtonInput::BUTTON_A) { // Up
        if (dirY == 0) { dirX = 0; dirY = -1; }
    } else if (button == ButtonInput::BUTTON_B) { // Down
        if (dirY == 0) { dirX = 0; dirY = 1; }
    } else if (button == ButtonInput::BUTTON_C) { // Select toggles horizontal
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
    // Border around play area
    StandardGameLayout::drawPlayAreaBorder(display, ThemeManager::getBorder());
    // Initial food
    placeFood();
    drawCell(foodX, foodY, ThemeManager::getPrimaryText());
}

void SnakeScreen::drawCell(int gx, int gy, uint16_t color) {
    int x = StandardGameLayout::PLAY_AREA_LEFT + gx * CELL_SIZE;
    int y = StandardGameLayout::PLAY_AREA_TOP + gy * CELL_SIZE;
    display->fillRect(x, y, CELL_SIZE, CELL_SIZE, color);
}

void SnakeScreen::step() {
    // Move body
    for (int i = snakeLength; i > 0; --i) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    // Move head
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    // Wrap/clamp within grid bounds
    if (snakeX[0] < 0) snakeX[0] = GRID_COLS - 1;
    if (snakeX[0] >= GRID_COLS) snakeX[0] = 0;
    if (snakeY[0] < 0) snakeY[0] = GRID_ROWS - 1;
    if (snakeY[0] >= GRID_ROWS) snakeY[0] = 0;

    // Eat food
    grewLastStep = false;
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength = min(snakeLength + 1, 127);
        grewLastStep = true;
        placeFood();
        drawHeader(); // update score
    }
}