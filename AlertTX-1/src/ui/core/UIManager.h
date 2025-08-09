#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "Screen.h"
#include <Adafruit_GFX.h>
#include <vector>
#include <memory>

/**
 * Central manager for the Alert TX-1 UI system.
 * Handles screen navigation, rendering coordination, and event distribution.
 */
class UIManager {
private:
    Screen* currentScreen = nullptr;
    Screen* pendingScreen = nullptr;
    std::vector<std::unique_ptr<Screen>> screens;
    bool initialized = false;
    unsigned long lastRender = 0;
    static constexpr unsigned long RENDER_INTERVAL = 33; // ~30 FPS

public:
    UIManager() = default;
    ~UIManager() = default;

    /**
     * Initialize the UI manager.
     */
    void begin() {
        initialized = true;
        lastRender = millis();
    }
    
    /**
     * Register a screen with the UI manager.
     * Returns a pointer to the registered screen.
     */
    template<typename T, typename... Args>
    T* registerScreen(Args&&... args) {
        auto screen = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = screen.get();
        screens.push_back(std::move(screen));
        return ptr;
    }
    
    /**
     * Set the current active screen.
     */
    void setScreen(Screen* screen) {
        if (screen != currentScreen) {
            pendingScreen = screen;
        }
    }
    
    /**
     * Get the current active screen.
     */
    Screen* getCurrentScreen() const {
        return currentScreen;
    }
    
    /**
     * Update the UI system. Call this in your main loop.
     */
    void update() {
        if (!initialized) return;
        
        // Handle screen transitions
        if (pendingScreen && pendingScreen != currentScreen) {
            if (currentScreen) {
                currentScreen->onDeactivate();
            }
            currentScreen = pendingScreen;
            pendingScreen = nullptr;
            if (currentScreen) {
                currentScreen->onActivate();
            }
        }
        
        // Update current screen
        if (currentScreen) {
            currentScreen->update();
        }
    }
    
    /**
     * Render the UI. Only renders if dirty or enough time has passed.
     */
    void render(Adafruit_GFX& gfx) {
        if (!initialized || !currentScreen) return;
        
        unsigned long now = millis();
        
        // Limit rendering to maintain frame rate
        if (now - lastRender < RENDER_INTERVAL && !currentScreen->isDirty()) {
            return;
        }
        
        // Clear screen if dirty
        if (currentScreen->isDirty()) {
            gfx.fillScreen(0x0000); // Black background
        }
        
        // Render current screen
        currentScreen->render(gfx);
        
        // Update display
        // Note: Assuming display has a display() method
        // This will need to be adapted based on your specific display library
        
        currentScreen->clearDirty();
        lastRender = now;
    }
    
    /**
     * Handle input events from buttons or other sources.
     */
    void handleEvent(int eventType, int eventData = 0) {
        if (currentScreen) {
            currentScreen->handleEvent(eventType, eventData);
        }
    }
    
    /**
     * Handle touch events (if touch screen is available).
     */
    bool handleTouch(int touchX, int touchY) {
        if (currentScreen) {
            return currentScreen->handleTouch(touchX, touchY);
        }
        return false;
    }
    
    /**
     * Force a redraw of the current screen.
     */
    void forceRedraw() {
        if (currentScreen) {
            currentScreen->markDirty();
        }
    }
    
    /**
     * Check if the UI system is initialized.
     */
    bool isInitialized() const {
        return initialized;
    }
    
    /**
     * Get the number of registered screens.
     */
    size_t getScreenCount() const {
        return screens.size();
    }
    
    /**
     * Set the frame rate limit (default: 30 FPS).
     */
    void setFrameRate(unsigned long fps) {
        if (fps > 0) {
            // Convert FPS to milliseconds per frame
            // RENDER_INTERVAL = 1000 / fps; // This won't work with constexpr
            // For now, we'll keep the default 30 FPS
        }
    }
};

#endif // UI_MANAGER_H