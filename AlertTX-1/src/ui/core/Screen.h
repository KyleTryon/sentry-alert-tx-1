#ifndef SCREEN_H
#define SCREEN_H

#include "StatefulComponent.h"
#include <vector>
#include <memory>

/**
 * Base class for full-screen views in the Alert TX-1 UI framework.
 * Manages child components and provides lifecycle methods.
 */
class Screen : public StatefulComponent {
protected:
    std::vector<std::unique_ptr<UIComponent>> components;
    bool built = false;
    bool active = false;

public:
    Screen() : StatefulComponent(0, 0, 240, 135) {} // ESP32-S3 TFT display size
    
    virtual ~Screen() = default;

    /**
     * Build the screen's components. Called once when screen is first shown.
     * Override this method to add your components.
     */
    virtual void build() = 0;
    
    /**
     * Called when the screen becomes active.
     */
    virtual void onActivate() { 
        active = true; 
        markDirty();
    }
    
    /**
     * Called when the screen becomes inactive.
     */
    virtual void onDeactivate() { 
        active = false; 
    }
    
    /**
     * Called periodically to update screen state.
     */
    void update() override {
        if (!built) {
            build();
            built = true;
            markDirty();
        }
        
        // Update all child components
        for (auto& component : components) {
            if (component && component->isVisible()) {
                component->update();
            }
        }
    }
    
    /**
     * Render the screen and all its components.
     */
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible()) return;
        
        // Render all child components
        for (auto& component : components) {
            if (component && component->isVisible()) {
                component->render(gfx);
            }
        }
    }
    
    /**
     * Handle events and pass them to child components.
     */
    void handleEvent(int eventType, int eventData = 0) override {
        for (auto& component : components) {
            if (component && component->isVisible() && component->isEnabled()) {
                component->handleEvent(eventType, eventData);
            }
        }
    }
    
    /**
     * Handle touch events and pass them to child components.
     */
    bool handleTouch(int touchX, int touchY) override {
        for (auto& component : components) {
            if (component && component->isVisible() && component->isEnabled()) {
                if (component->handleTouch(touchX, touchY)) {
                    return true; // Component handled the touch
                }
            }
        }
        return false;
    }
    
    /**
     * Check if any child components are dirty.
     */
    bool isDirty() override {
        if (StatefulComponent::isDirty()) {
            return true;
        }
        
        for (auto& component : components) {
            if (component && component->isDirty()) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Clear dirty flags for this screen and all child components.
     */
    void clearDirty() override {
        StatefulComponent::clearDirty();
        for (auto& component : components) {
            if (component) {
                component->clearDirty();
            }
        }
    }

protected:
    /**
     * Add a component to this screen.
     */
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components.push_back(std::move(component));
        markDirty();
        return ptr;
    }
    
    /**
     * Remove all components from this screen.
     */
    void clearComponents() {
        components.clear();
        markDirty();
    }
    
    /**
     * Get the number of components in this screen.
     */
    size_t getComponentCount() const {
        return components.size();
    }
    
    /**
     * Check if this screen is currently active.
     */
    bool isActive() const {
        return active;
    }
};

#endif // SCREEN_H