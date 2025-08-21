#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <Adafruit_ST7789.h>
#include "../../config/DisplayConfig.h"
#include "Theme.h"

/**
 * RenderManager
 * 
 * Centralized rendering management for the Alert TX-1 framework.
 * Provides efficient rendering patterns that work for both component-based
 * and direct-drawing screens.
 * 
 * Features:
 * - Dirty rectangle tracking
 * - Batch rendering operations
 * - Frame rate limiting
 * - Render statistics for optimization
 */

class RenderManager {
public:
    struct DirtyRect {
        int x, y, width, height;
        bool isDirty;
        
        void markDirty() { isDirty = true; }
        void clear() { isDirty = false; }
        void setBounds(int nx, int ny, int nw, int nh) {
            x = nx; y = ny; width = nw; height = nh;
            isDirty = true;
        }
    };
    
    struct RenderStats {
        unsigned long lastFrameTime = 0;
        unsigned long frameCount = 0;
        unsigned long totalDrawTime = 0;
        int fps = 0;
        
        void reset() {
            lastFrameTime = millis();
            frameCount = 0;
            totalDrawTime = 0;
            fps = 0;
        }
        
        void updateFPS() {
            unsigned long now = millis();
            unsigned long elapsed = now - lastFrameTime;
            if (elapsed >= 1000) {
                fps = (frameCount * 1000) / elapsed;
                frameCount = 0;
                lastFrameTime = now;
            }
        }
    };
    
private:
    static const int MAX_DIRTY_RECTS = 16;
    DirtyRect dirtyRects[MAX_DIRTY_RECTS];
    int dirtyRectCount = 0;
    
    Adafruit_ST7789* display;
    RenderStats stats;
    
    // Frame timing
    unsigned long targetFrameTime = 16; // 60 FPS default
    unsigned long lastRenderTime = 0;
    
    // Full screen redraw tracking
    bool needsFullRedraw = false;
    bool staticContentDrawn = false;
    
public:
    RenderManager(Adafruit_ST7789* display) : display(display) {
        clearAllDirtyRects();
    }
    
    // Dirty rectangle management
    void addDirtyRect(int x, int y, int width, int height) {
        if (dirtyRectCount < MAX_DIRTY_RECTS) {
            dirtyRects[dirtyRectCount].setBounds(x, y, width, height);
            dirtyRectCount++;
        } else {
            // Too many dirty rects - force full redraw
            markFullScreenDirty();
        }
    }
    
    void markFullScreenDirty() {
        needsFullRedraw = true;
        staticContentDrawn = false;
        clearAllDirtyRects();
    }
    
    void clearAllDirtyRects() {
        for (int i = 0; i < MAX_DIRTY_RECTS; i++) {
            dirtyRects[i].clear();
        }
        dirtyRectCount = 0;
    }
    
    // Redraw state queries
    bool needsRedraw() const {
        if (needsFullRedraw) return true;
        for (int i = 0; i < dirtyRectCount; i++) {
            if (dirtyRects[i].isDirty) return true;
        }
        return false;
    }
    
    bool isStaticContentDrawn() const { return staticContentDrawn; }
    void markStaticContentDrawn() { staticContentDrawn = true; }
    
    // Frame rate control
    bool shouldRenderFrame() {
        unsigned long now = millis();
        if (now - lastRenderTime >= targetFrameTime) {
            lastRenderTime = now;
            return true;
        }
        return false;
    }
    
    void setTargetFPS(int fps) {
        if (fps <= 0) fps = 60;
        targetFrameTime = 1000 / fps;
    }
    
    // Rendering helpers
    void beginFrame() {
        stats.frameCount++;
        
        if (needsFullRedraw) {
            display->fillScreen(ThemeManager::getBackground());
            needsFullRedraw = false;
        }
    }
    
    void endFrame() {
        stats.updateFPS();
        clearAllDirtyRects();
    }
    
    // Clear specific dirty rectangles
    void clearDirtyRects(uint16_t color = 0) {
        if (color == 0) color = ThemeManager::getBackground();
        
        for (int i = 0; i < dirtyRectCount; i++) {
            if (dirtyRects[i].isDirty) {
                display->fillRect(dirtyRects[i].x, dirtyRects[i].y,
                                 dirtyRects[i].width, dirtyRects[i].height,
                                 color);
            }
        }
    }
    
    // Statistics
    int getFPS() const { return stats.fps; }
    void resetStats() { stats.reset(); }
    
    // Debug helpers
    void drawDirtyRects(uint16_t color = ST77XX_RED) {
        #ifdef DEBUG_RENDER
        for (int i = 0; i < dirtyRectCount; i++) {
            if (dirtyRects[i].isDirty) {
                display->drawRect(dirtyRects[i].x, dirtyRects[i].y,
                                 dirtyRects[i].width, dirtyRects[i].height,
                                 color);
            }
        }
        #endif
    }
};

/**
 * Global render manager instance
 * Screens can access this for centralized rendering control
 */
class GlobalRenderManager {
private:
    static RenderManager* instance;
    
public:
    static void initialize(Adafruit_ST7789* display) {
        if (!instance) {
            instance = new RenderManager(display);
        }
    }
    
    static RenderManager* getInstance() {
        return instance;
    }
    
    static void cleanup() {
        delete instance;
        instance = nullptr;
    }
};

#endif // RENDER_MANAGER_H
