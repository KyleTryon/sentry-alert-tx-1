#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include "Screen.h"

class GameScreen : public Screen {
protected:
    // Shared timing system
    unsigned long lastUpdateMs = 0;
    unsigned long targetFrameTime = 16; // 60 FPS default

    // Game area management
    int gameLeft = 0, gameRight = 0, gameTop = 0, gameBottom = 0;
    bool staticBackgroundCached = false;
    unsigned long lastStaticRedraw = 0;

public:
    GameScreen(Adafruit_ST7789* display, const char* name, int id = 0)
        : Screen(display, name, id) {}
    virtual ~GameScreen() = default;

    // Game hooks
    virtual void updateGame() = 0;    // Game-specific update logic
    virtual void drawGame() = 0;      // Game-specific rendering
    virtual void drawStatic() = 0;    // Static background elements

    // Shared utilities
    bool shouldUpdateFrame() {
        unsigned long now = millis();
        if (now - lastUpdateMs < targetFrameTime) return false;
        lastUpdateMs = now;
        return true;
    }

    void setTargetFPS(int fps) {
        if (fps <= 0) fps = 60;
        targetFrameTime = (unsigned long)(1000UL / (unsigned long)fps);
    }

    void update() override {
        Screen::update();
        if (!shouldUpdateFrame()) return;
        updateGame();
    }

    void draw() override {
        if (!isActive()) return;
        if (!staticBackgroundCached) {
            drawStatic();
            staticBackgroundCached = true;
            lastStaticRedraw = millis();
        }
        drawGame();
    }
};

#endif // GAME_SCREEN_H