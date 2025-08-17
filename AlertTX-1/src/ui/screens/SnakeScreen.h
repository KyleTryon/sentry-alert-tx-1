#ifndef SNAKESCREEN_H
#define SNAKESCREEN_H

#include "../core/Screen.h"
#include "../core/StandardGameLayout.h"

class SnakeScreen : public Screen {
public:
	SnakeScreen(Adafruit_ST7789* display);
	~SnakeScreen() override = default;

	void enter() override;
	void exit() override;
	void update() override;
	void draw() override;
	void handleButtonPress(int button) override;

private:
	// Grid config
	static const int CELL_SIZE = 6;
	static const int GRID_COLS = StandardGameLayout::PLAY_AREA_WIDTH / CELL_SIZE;
	static const int GRID_ROWS = StandardGameLayout::PLAY_AREA_HEIGHT / CELL_SIZE;

	// Snake state
	int snakeX[128];
	int snakeY[128];
	int snakeLength;
	int dirX;
	int dirY;
	int foodX;
	int foodY;
	bool grewLastStep;

	// Timing
	unsigned long lastStepMs;
	unsigned long stepIntervalMs;

	// Methods
	void placeFood();
	void drawHeader();
	void drawGrid();
	void drawCell(int gx, int gy, uint16_t color);
	void step();
};

#endif // SNAKESCREEN_H