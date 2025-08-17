#ifndef PONGSCREEN_H
#define PONGSCREEN_H

#include "../core/GameScreen.h"
#include "../core/DisplayUtils.h"

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
	bool needsRedraw;
    bool pendingFullRedraw = false;

	// Helpers
	void resetBall();
	void aiMove();
	void clampPaddles();
	void drawCourt();
	void drawObjects();
    void clearPrevious();
    void redrawCenterLineSegmentIn(int x0, int y0, int w, int h);
    void updateScoreDisplay();
    void fullRedraw();
};

#endif // PONGSCREEN_H