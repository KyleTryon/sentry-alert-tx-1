#ifndef STATEFUL_COMPONENT_H
#define STATEFUL_COMPONENT_H

#include "UIComponent.h"
#include <functional>

/**
 * Extends UIComponent with state management and dirty tracking.
 * Components that need to track internal state should inherit from this.
 */
class StatefulComponent : public UIComponent {
protected:
    bool dirty = true;
    unsigned long lastUpdate = 0;

public:
    StatefulComponent(int x = 0, int y = 0, int w = 0, int h = 0) 
        : UIComponent(x, y, w, h) {}
    
    virtual ~StatefulComponent() = default;

    /**
     * Update component state using a mutator function.
     * Automatically marks the component as dirty for re-rendering.
     */
    void setState(std::function<void()> mutator) {
        mutator();
        markDirty();
    }
    
    /**
     * Mark this component as needing a redraw.
     */
    virtual void markDirty() { 
        dirty = true; 
        lastUpdate = millis();
    }
    
    /**
     * Check if this component needs to be redrawn.
     */
    bool isDirty() override { 
        return dirty; 
    }
    
    /**
     * Clear the dirty flag after rendering.
     */
    void clearDirty() override { 
        dirty = false; 
    }
    
    /**
     * Force a redraw on the next render cycle.
     */
    void forceRedraw() {
        markDirty();
    }
    
    /**
     * Get the time of the last update in milliseconds.
     */
    unsigned long getLastUpdate() const {
        return lastUpdate;
    }
    
    // Override setters to mark dirty when changed
    void setPosition(int newX, int newY) override {
        if (x != newX || y != newY) {
            UIComponent::setPosition(newX, newY);
            markDirty();
        }
    }
    
    void setSize(int newW, int newH) override {
        if (w != newW || h != newH) {
            UIComponent::setSize(newW, newH);
            markDirty();
        }
    }
    
    void setVisible(bool vis) override {
        if (visible != vis) {
            UIComponent::setVisible(vis);
            markDirty();
        }
    }
    
    void setEnabled(bool en) override {
        if (enabled != en) {
            UIComponent::setEnabled(en);
            markDirty();
        }
    }
};

#endif // STATEFUL_COMPONENT_H