#ifndef BEEPERHEROSCREEN_H
#define BEEPERHEROSCREEN_H

#include "../core/GameScreen.h"
#include "../core/StandardGameLayout.h"
#include "../../ringtones/RingtonePlayer.h"

class BeeperHeroScreen : public GameScreen {
public:
	BeeperHeroScreen(Adafruit_ST7789* display);
	~BeeperHeroScreen() override = default;

	void enter() override;
	void exit() override;
	void handleButtonPress(int button) override;

protected:
	void updateGame() override;
	void drawGame() override;
	void drawStatic() override;
	void cleanup() override;

private:
	// Lanes
	static const uint8_t NUM_LANES = 3;
	static const int LANE_WIDTH = StandardGameLayout::PLAY_AREA_WIDTH / NUM_LANES;
	static const int LANE_HEIGHT = StandardGameLayout::PLAY_AREA_HEIGHT;
	static const int HIT_LINE_Y = StandardGameLayout::PLAY_AREA_BOTTOM - 10;

	bool lanesDirty[NUM_LANES];
	int laneMinY[NUM_LANES];
	int laneMaxY[NUM_LANES];

	void markLaneDirty(uint8_t lane, int y, int height);
	void clearDirtyLanes();
	void drawLanes();
	void drawHitLine();

	// Notes
	struct Note { uint8_t lane; int y; int prevY; int height; bool active; };
	static const int MAX_ACTIVE_NOTES = 20;
	Note notes[MAX_ACTIVE_NOTES];
	int noteSpeedPx = 3;
	unsigned long lastNoteStartTime = 0;

	// Scoring
	int score = 0;
	int combo = 0;
	void spawnNoteFromCurrent();
	uint8_t laneFromFrequency(uint16_t freq) const;
	void updateNotes();
	void drawNotes();
	void tryHitLane(uint8_t lane);
	void removeNoteAt(int idx);

	// Audio integration
	RingtonePlayer player;
	bool started = false;
};

#endif // BEEPERHEROSCREEN_H