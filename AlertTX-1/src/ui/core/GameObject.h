#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Adafruit_ST7789.h>
#include <Arduino.h>

class GameObject {
protected:
    int x = 0, y = 0;
    int prevX = 0, prevY = 0;
    int width = 0, height = 0;
    uint16_t color = 0xFFFF;
    bool visible = true;
    bool moved = false;

public:
    GameObject() = default;
    GameObject(int x, int y, int w, int h, uint16_t color)
        : x(x), y(y), prevX(x), prevY(y), width(w), height(h), color(color) {}

    virtual ~GameObject() = default;

    void setPosition(int newX, int newY) {
        prevX = x; prevY = y;
        x = newX; y = newY;
        moved = (prevX != x || prevY != y);
    }

    void setSize(int w, int h) { width = w; height = h; }
    void setColor(uint16_t c) { color = c; }

    void setVisible(bool v) { visible = v; }
    bool isVisible() const { return visible; }

    void clearPrevious(Adafruit_ST7789* display, uint16_t bgColor) {
        if (moved && display) {
            display->fillRect(prevX, prevY, width, height, bgColor);
        }
    }

    virtual void draw(Adafruit_ST7789* display) {
        if (visible && display) {
            display->fillRect(x, y, width, height, color);
        }
        moved = false;
    }

    bool intersects(const GameObject& other) const {
        return !(x + width <= other.x || other.x + other.width <= x ||
                 y + height <= other.y || other.y + other.height <= y);
    }

    bool isInBounds(int left, int top, int right, int bottom) const {
        return x >= left && y >= top && (x + width) <= right && (y + height) <= bottom;
    }
};

#endif // GAME_OBJECT_H