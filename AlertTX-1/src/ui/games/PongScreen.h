#ifndef PONG_GAME_SCREEN_H
#define PONG_GAME_SCREEN_H

#include "../core/GameScreen.h"
#include "../core/StandardGameLayout.h"

class PongScreen : public GameScreen {
public:
    PongScreen(Adafruit_ST7789* display);
    ~PongScreen() override = default;

    void enter() override;
    void handleButtonPress(int button) override;

protected:
    void updateGame() override;
    void drawGame() override;
    void drawStatic() override;

private:
    // Game state
    int ballX, ballY;
    int velX, velY;
    int paddlePlayerY;
    int paddleAiY;
    int prevBallX, prevBallY;
    int prevPaddlePlayerY;
    int prevPaddleAiY;
    int paddleHeight;
    int paddleWidth;
    int ballSize;
    int playerScore = 0;
    int aiScore = 0;
    int courtLeft, courtRight, courtTop, courtBottom;
    bool pendingFullRedraw = false;

    // AI behavior tuning
    int aiTargetY = 0;                         // Desired top position of AI paddle
    unsigned long lastAiUpdateMs = 0;          // Last time AI target was updated
    unsigned long aiReactionIntervalMs = 120;  // How often AI re-targets (ms)
    int aiMaxSpeed = 2;                        // Max AI paddle speed (px/frame)
    int aiErrorPixels = 8;                     // Random tracking error (px)
    int aiDeadZone = 3;                        // Dead zone around target (px)
    // Random input delay scheduling
    unsigned long aiNextDecisionTimeMs = 0;    // Timestamp when AI can make next decision
    int aiDelayMinMs = 80;                     // Min extra delay per decision
    int aiDelayMaxMs = 620;                    // Max extra delay per decision
    int aiHesitateChance = 25;                 // % chance to hesitate further

    // Helpers
    void resetBall();
    void aiMove();
    void updateAiTarget();
    int predictBallYAtX(int targetX) const;
    void clampPaddles();
    void drawCourt();
    void drawObjects();
    void clearPrevious();
    void redrawCenterLineSegmentIn(int x0, int y0, int w, int h);
    void updateScoreDisplay();
    void fullRedraw();
};

#endif // PONG_GAME_SCREEN_H


