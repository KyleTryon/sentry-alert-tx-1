#include "PongScreen.h"
#include "../core/Theme.h"
#include "../core/ScreenManager.h"
#include "../../config/DisplayConfig.h"
#include "../core/StandardGameLayout.h"

PongScreen::PongScreen(Adafruit_ST7789* display)
    : GameScreen(display, "Pong", 42), needsRedraw(true) {
    // Court bounds (leave margin)
    courtLeft = 6;
    courtRight = 234;
    courtTop = 30;
    courtBottom = 125;

    paddleWidth = 3;
    paddleHeight = 20;
    ballSize = 3;

    paddlePlayerY = (courtTop + courtBottom - paddleHeight) / 2;
    paddleAiY = paddlePlayerY;
    prevPaddlePlayerY = paddlePlayerY;
    prevPaddleAiY = paddleAiY;
    resetBall();
}

void PongScreen::enter() {
    GameScreen::enter();
    setTargetFPS(60);
    needsRedraw = true;
    updateScoreDisplay(); // draw header (title + scores)
    fullRedraw();
}

void PongScreen::resetBall() {
    ballX = (courtLeft + courtRight) / 2;
    ballY = (courtTop + courtBottom) / 2;
    prevBallX = ballX;
    prevBallY = ballY;
    velX = -1.5; // start toward player
    velY = (random(0, 2) == 0) ? 1 : -1; // avoid perfectly flat
}

void PongScreen::aiMove() {
    int center = paddleAiY + paddleHeight / 2;
    int error = ballY - center;
    if (error > 1) paddleAiY += 1;
    else if (error < -1) paddleAiY -= 1;
}

void PongScreen::clampPaddles() {
    if (paddlePlayerY < courtTop) paddlePlayerY = courtTop;
    if (paddlePlayerY + paddleHeight > courtBottom) paddlePlayerY = courtBottom - paddleHeight;
    if (paddleAiY < courtTop) paddleAiY = courtTop;
    if (paddleAiY + paddleHeight > courtBottom) paddleAiY = courtBottom - paddleHeight;
}

void PongScreen::updateGame() {
    clampPaddles();

    // AI
    aiMove();

    // Move ball
    prevBallX = ballX;
    prevBallY = ballY;
    ballX += velX;
    ballY += velY;
    needsRedraw = true; // animate each tick

    // Collide top/bottom
    if (ballY <= courtTop || ballY + ballSize >= courtBottom) {
        velY = -velY;
        ballY = max(courtTop, min(ballY, courtBottom - ballSize));
        needsRedraw = true;
    }

    // Collide player paddle (left)
    int playerX = courtLeft + 4;
    if (ballX <= playerX + paddleWidth && ballX >= playerX - ballSize) {
        if (ballY + ballSize >= paddlePlayerY && ballY <= paddlePlayerY + paddleHeight) {
            velX = -velX;
            ballX = playerX + paddleWidth; // prevent sticking
            needsRedraw = true;
        }
    }

    // Collide AI paddle (right)
    int aiX = courtRight - 4 - paddleWidth;
    if (ballX + ballSize >= aiX && ballX <= aiX + paddleWidth) {
        if (ballY + ballSize >= paddleAiY && ballY <= paddleAiY + paddleHeight) {
            velX = -velX;
            ballX = aiX - ballSize; // prevent sticking
            needsRedraw = true;
        }
    }

    // Out of bounds -> score and reset
    if (ballX < courtLeft - 4) { // AI scores
        aiScore++;
        updateScoreDisplay();
        pendingFullRedraw = true;
        delay(300);
        resetBall();
        needsRedraw = true;
    } else if (ballX > courtRight + 4) { // Player scores
        playerScore++;
        updateScoreDisplay();
        pendingFullRedraw = true;
        delay(300);
        resetBall();
        needsRedraw = true;
    }
}

void PongScreen::drawGame() {
    if (pendingFullRedraw) {
        fullRedraw();
        pendingFullRedraw = false;
    }
    static unsigned long lastCourtRedraw = 0;
    unsigned long now = millis();
    if (now - lastCourtRedraw > 1000) { // redraw static court less often
        drawCourt();
        lastCourtRedraw = now;
    }
    clearPrevious();
    drawObjects();
    needsRedraw = false;
}

void PongScreen::drawStatic() {
    updateScoreDisplay();
    drawCourt();
}

void PongScreen::drawCourt() {
    uint16_t bg = ThemeManager::getBackground();
    int courtW = (courtRight - courtLeft);
    int courtH = (courtBottom - courtTop);
    display->fillRect(courtLeft - 1, courtTop - 1, courtW + 2, courtH + 2, bg);
    // Borders
    uint16_t border = ThemeManager::getBorder();
    display->drawRect(courtLeft, courtTop, courtW, courtH, border);
    // Center line
    for (int y = courtTop; y < courtBottom; y += 6) {
        display->drawFastVLine((courtLeft + courtRight) / 2, y, 3, ThemeManager::getSecondaryText());
    }
}

void PongScreen::drawObjects() {
    uint16_t accent = ThemeManager::getAccent();
    int playerX = courtLeft + 4;
    int aiX = courtRight - 4 - paddleWidth;
    // Paddles
    display->fillRect(playerX, paddlePlayerY, paddleWidth, paddleHeight, accent);
    display->fillRect(aiX, paddleAiY, paddleWidth, paddleHeight, accent);
    // Ball (use a visible color across themes)
    display->fillRect(ballX, ballY, ballSize, ballSize, ThemeManager::getPrimaryText());
}

void PongScreen::clearPrevious() {
    uint16_t bg = ThemeManager::getBackground();
    int playerX = courtLeft + 4;
    int aiX = courtRight - 4 - paddleWidth;
    // Clear previous ball
    display->fillRect(prevBallX, prevBallY, ballSize, ballSize, bg);
    if (prevBallX < courtLeft - 1 || prevBallX > courtRight) {
        int x0 = CLAMP_X(prevBallX);
        int w = ballSize;
        if (x0 + w > DISPLAY_WIDTH) w = DISPLAY_WIDTH - x0;
        if (w > 0) {
            display->fillRect(x0, 0, w, DISPLAY_HEIGHT, bg);
            updateScoreDisplay();
            if (x0 <= courtLeft && x0 + w >= courtLeft) {
                display->drawFastVLine(courtLeft, courtTop, courtBottom - courtTop, ThemeManager::getBorder());
            }
            if (x0 <= (courtRight - 1) && x0 + w >= (courtRight - 1)) {
                display->drawFastVLine(courtRight - 1, courtTop, courtBottom - courtTop, ThemeManager::getBorder());
            }
        }
    }
    redrawCenterLineSegmentIn(prevBallX, prevBallY, ballSize, ballSize);
    // Clear previous paddles
    display->fillRect(playerX, prevPaddlePlayerY, paddleWidth, paddleHeight, bg);
    display->fillRect(aiX, prevPaddleAiY, paddleWidth, paddleHeight, bg);
    prevPaddlePlayerY = paddlePlayerY;
    prevPaddleAiY = paddleAiY;
}

void PongScreen::redrawCenterLineSegmentIn(int x0, int y0, int w, int h) {
    int centerX = (courtLeft + courtRight) / 2;
    if (x0 <= centerX && centerX < x0 + w) {
        for (int y = courtTop; y < courtBottom; y += 6) {
            if (y + 3 >= y0 && y <= y0 + h) {
                display->drawFastVLine(centerX, y, 3, ThemeManager::getSecondaryText());
            }
        }
    }
}

void PongScreen::updateScoreDisplay() {
    StandardGameLayout::drawGameHeader(display, "Pong");
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    int scoreY = StandardGameLayout::HEADER_HEIGHT - 7;
    display->setCursor(10, scoreY);
    display->print("You: ");
    display->print(playerScore);
    display->setCursor(240 - 60, scoreY);
    display->print("AI: ");
    display->print(aiScore);
}

void PongScreen::fullRedraw() {
    uint16_t bg = ThemeManager::getBackground();
    display->fillScreen(bg);
    updateScoreDisplay();
    drawCourt();
    prevBallX = ballX;
    prevBallY = ballY;
    prevPaddlePlayerY = paddlePlayerY;
    prevPaddleAiY = paddleAiY;
}

void PongScreen::handleButtonPress(int button) {
    if (button == ButtonInput::BUTTON_A) { // Up
        paddlePlayerY -= 4;
        clampPaddles();
        needsRedraw = true;
    } else if (button == ButtonInput::BUTTON_B) { // Down
        paddlePlayerY += 4;
        clampPaddles();
        needsRedraw = true;
    }
}