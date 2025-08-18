#include "PongScreen.h"
#include "../core/Theme.h"

PongScreen::PongScreen(Adafruit_ST7789* display)
    : GameScreen(display, "Pong", 42) {
    // Court bounds (use StandardGameLayout play area)
    courtLeft = StandardGameLayout::PLAY_AREA_LEFT;
    courtRight = StandardGameLayout::PLAY_AREA_RIGHT;
    courtTop = StandardGameLayout::PLAY_AREA_TOP;
    courtBottom = StandardGameLayout::PLAY_AREA_BOTTOM;

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
    updateScoreDisplay();
    fullRedraw();
}

void PongScreen::resetBall() {
    ballX = (courtLeft + courtRight) / 2;
    ballY = (courtTop + courtBottom) / 2;
    prevBallX = ballX;
    prevBallY = ballY;
    velX = -2; // start toward player
    velY = (random(0, 2) == 0) ? 1 : -1;
    // Reset AI target to current position with new serve
    aiTargetY = paddleAiY;
}

void PongScreen::aiMove() {
    // Update target occasionally to simulate reaction delay
    updateAiTarget();

    // Move toward target with speed limit and dead zone
    int error = aiTargetY - paddleAiY;
    if (abs(error) <= aiDeadZone) return;
    if (error > 0) {
        paddleAiY += min(aiMaxSpeed, error);
    } else {
        paddleAiY -= min(aiMaxSpeed, -error);
    }
}

void PongScreen::updateAiTarget() {
    unsigned long now = millis();
    // Respect both base reaction interval and randomized hesitation windows
    if (now < aiNextDecisionTimeMs) return;
    if (now - lastAiUpdateMs < aiReactionIntervalMs) return;
    lastAiUpdateMs = now;

    // Predict where the ball will be at AI paddle's x with simple reflection
    int aiX = courtRight - 4 - paddleWidth;
    int predictedY = predictBallYAtX(aiX);

    // Add random tracking error
    int jitter = random(-aiErrorPixels, aiErrorPixels + 1);
    predictedY += jitter;

    // Convert desired center to top position (clamp to court)
    int desiredTop = predictedY - paddleHeight / 2;
    if (desiredTop < courtTop) desiredTop = courtTop;
    if (desiredTop + paddleHeight > courtBottom) desiredTop = courtBottom - paddleHeight;
    aiTargetY = desiredTop;

    // Schedule next decision with random input delay
    unsigned long extraDelay = (unsigned long)random(aiDelayMinMs, aiDelayMaxMs + 1);
    // Occasional additional hesitation
    if (random(0, 100) < aiHesitateChance) {
        extraDelay += (unsigned long)random(60, 180);
    }
    aiNextDecisionTimeMs = now + extraDelay;
}

int PongScreen::predictBallYAtX(int targetX) const {
    // Simulate ball trajectory with current position and velocity,
    // reflecting off top/bottom walls until reaching targetX
    int simX = ballX;
    int simY = ballY;
    int vx = velX;
    int vy = velY;

    // If ball moving away from AI, bias toward center with mild randomness
    if (vx < 0) {
        return (courtTop + courtBottom) / 2 + random(-10, 11);
    }

    // Step simulation conservatively (bounded iterations)
    for (int steps = 0; steps < 400; ++steps) {
        simX += vx;
        simY += vy;
        if (simY <= courtTop) { simY = courtTop; vy = -vy; }
        if (simY + ballSize >= courtBottom) { simY = courtBottom - ballSize; vy = -vy; }
        if (simX + ballSize >= targetX) break;
    }
    // Return predicted center Y
    return simY + ballSize / 2;
}

void PongScreen::clampPaddles() {
    if (paddlePlayerY < courtTop) paddlePlayerY = courtTop;
    if (paddlePlayerY + paddleHeight > courtBottom) paddlePlayerY = courtBottom - paddleHeight;
    if (paddleAiY < courtTop) paddleAiY = courtTop;
    if (paddleAiY + paddleHeight > courtBottom) paddleAiY = courtBottom - paddleHeight;
}

void PongScreen::updateGame() {
    clampPaddles();
    aiMove();
    // Move ball
    prevBallX = ballX;
    prevBallY = ballY;
    ballX += velX;
    ballY += velY;

    // Collide top/bottom
    if (ballY <= courtTop || ballY + ballSize >= courtBottom) {
        velY = -velY;
        if (ballY <= courtTop) ballY = courtTop;
        if (ballY + ballSize >= courtBottom) ballY = courtBottom - ballSize;
    }

    // Collide paddles
    int playerX = courtLeft + 4;
    if (ballX <= playerX + paddleWidth && ballX >= playerX - ballSize) {
        if (ballY + ballSize >= paddlePlayerY && ballY <= paddlePlayerY + paddleHeight) {
            velX = -velX;
            ballX = playerX + paddleWidth;
        }
    }
    int aiX = courtRight - 4 - paddleWidth;
    if (ballX + ballSize >= aiX && ballX <= aiX + paddleWidth) {
        if (ballY + ballSize >= paddleAiY && ballY <= paddleAiY + paddleHeight) {
            velX = -velX;
            ballX = aiX - ballSize;
        }
    }

    // Scoring
    if (ballX < courtLeft - 4) { // AI scores
        aiScore++;
        updateScoreDisplay();
        pendingFullRedraw = true;
        resetBall();
    } else if (ballX > courtRight + 4) { // Player scores
        playerScore++;
        updateScoreDisplay();
        pendingFullRedraw = true;
        resetBall();
    }
}

void PongScreen::drawGame() {
    if (pendingFullRedraw) {
        fullRedraw();
        pendingFullRedraw = false;
    }
    clearPrevious();
    drawObjects();
}

void PongScreen::drawStatic() {
    // Header and court
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
    // Always draw paddles at their current positions to ensure visibility
    // Frame SPI transaction is handled by GameScreen::draw()
    display->fillRect(playerX, paddlePlayerY, paddleWidth, paddleHeight, accent);
    display->fillRect(aiX, paddleAiY, paddleWidth, paddleHeight, accent);
    // Ball
    display->fillRect(ballX, ballY, ballSize, ballSize, ThemeManager::getPrimaryText());
}

void PongScreen::clearPrevious() {
    uint16_t bg = ThemeManager::getBackground();
    int playerX = courtLeft + 4;
    int aiX = courtRight - 4 - paddleWidth;
    // Frame SPI transaction is handled by GameScreen::draw()
    // Clear previous ball
    display->fillRect(prevBallX, prevBallY, ballSize, ballSize, bg);
    // Redraw center line if overwritten
    redrawCenterLineSegmentIn(prevBallX, prevBallY, ballSize, ballSize);
    // Clear only uncovered strips for paddles if they moved
    int deltaPlayer = paddlePlayerY - prevPaddlePlayerY;
    if (deltaPlayer != 0) {
        if (deltaPlayer > 0) {
            // Moved down: clear strip above new paddle top
            display->fillRect(playerX, prevPaddlePlayerY, paddleWidth, deltaPlayer, bg);
        } else {
            // Moved up: clear strip below new paddle bottom
            int clearY = paddlePlayerY + paddleHeight;
            int clearH = -deltaPlayer;
            display->fillRect(playerX, clearY, paddleWidth, clearH, bg);
        }
    }
    int deltaAi = paddleAiY - prevPaddleAiY;
    if (deltaAi != 0) {
        if (deltaAi > 0) {
            // Moved down
            display->fillRect(aiX, prevPaddleAiY, paddleWidth, deltaAi, bg);
        } else {
            // Moved up
            int clearY = paddleAiY + paddleHeight;
            int clearH = -deltaAi;
            display->fillRect(aiX, clearY, paddleWidth, clearH, bg);
        }
    }
    // End of incremental clears
    // Update previous positions after clearing (so draw step knows if moved)
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
    // Reset previous positions to avoid clearing current
    prevBallX = ballX;
    prevBallY = ballY;
    prevPaddlePlayerY = paddlePlayerY;
    prevPaddleAiY = paddleAiY;
}

void PongScreen::handleButtonPress(int button) {
    if (button == ButtonInput::BUTTON_A) {
        paddlePlayerY -= 4;
        clampPaddles();
    } else if (button == ButtonInput::BUTTON_B) {
        paddlePlayerY += 4;
        clampPaddles();
    }
}


