#ifndef RENDER_BATCH_H
#define RENDER_BATCH_H

#include <Adafruit_ST7789.h>
#include <Arduino.h>

class RenderBatch {
private:
    struct BatchedRect {
        int x, y, width, height;
        uint16_t color;
    };

    static const uint8_t MAX_BATCH_SIZE = 32;
    BatchedRect rects[MAX_BATCH_SIZE];
    uint8_t count = 0;

    static int compareColor(const void* a, const void* b) {
        const BatchedRect* ra = static_cast<const BatchedRect*>(a);
        const BatchedRect* rb = static_cast<const BatchedRect*>(b);
        if (ra->color < rb->color) return -1;
        if (ra->color > rb->color) return 1;
        return 0;
    }

public:
    void addRect(int x, int y, int w, int h, uint16_t color) {
        if (count < MAX_BATCH_SIZE) {
            rects[count] = {x, y, w, h, color};
            count++;
        }
    }

    void flush(Adafruit_ST7789* display) {
        if (!display || count == 0) return;
        qsort(rects, count, sizeof(BatchedRect), compareColor);
        for (uint8_t i = 0; i < count; i++) {
            display->fillRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height, rects[i].color);
        }
        count = 0;
    }
};

#endif // RENDER_BATCH_H