#ifndef SCREEN_H
#define SCREEN_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <functional>
#include "Component.h"
#include "Theme.h"
#include "RenderManager.h"

/**
 * Screen Base Class
 * 
 * Manages a collection of UI components and provides screen lifecycle
 * management for the Alert TX-1 framework. Screens can be navigated
 * between using the ScreenManager.
 * 
 * Memory Efficient Design:
 * - Fixed array of component pointers (no dynamic allocation)
 * - Minimal per-screen overhead
 * - Automatic component cleanup
 */

class Screen {
protected:
    Adafruit_ST7789* display;
    
    // Component management (fixed array for memory efficiency)
    static const int MAX_COMPONENTS = 10;  // Configurable limit per screen
    Component* components[MAX_COMPONENTS];
    int componentCount = 0;
    
    // Screen state
    bool active = false;
    bool needsFullRedraw = true;
    const char* screenName;
    
    // Screen identification
    int screenId;
    
    // Direct drawing support (for screens that don't use components)
    struct DirectDrawRegion {
        enum Type { STATIC, DYNAMIC };
        Type type;
        std::function<void()> drawFunc;
        bool needsRedraw = true;
    };
    
    static const int MAX_DRAW_REGIONS = 8;
    DirectDrawRegion drawRegions[MAX_DRAW_REGIONS];
    int drawRegionCount = 0;

public:
    Screen(Adafruit_ST7789* display, const char* name, int id = 0);
    virtual ~Screen();
    
    // Screen lifecycle - override in subclasses for custom behavior
    virtual void enter();   // Called when screen becomes active
    virtual void exit();    // Called when leaving screen
    virtual void update();  // Update all components (call in main loop)
    virtual void draw();    // Draw all components
    
    // Input handling - must be implemented by subclasses
    virtual void handleButtonPress(int button) = 0;
    
    // Component management
    bool addComponent(Component* component);
    bool removeComponent(Component* component);
    void clearComponents();
    Component* getComponent(int index) const;
    int getComponentCount() const { return componentCount; }
    
    // Screen state management
    bool isActive() const { return active; }
    void setActive(bool active);
    void markForFullRedraw() { 
        needsFullRedraw = true; 
        // Mark all regions as dirty
        for (auto& region : drawRegions) {
            region.needsRedraw = true;
        }
    }
    
    // Direct drawing support for screens that don't use components
    void addDrawRegion(DirectDrawRegion::Type type, std::function<void()> drawFunc);
    void markRegionDirty(DirectDrawRegion::Type type);
    void clearRegionDirty(DirectDrawRegion::Type type);
    
    // Backwards compatibility helpers
    bool isStaticContentDrawn() const;
    void markStaticContentDrawn();
    void markDynamicContentDirty();
    bool shouldRedrawDynamic() const;
    void clearDynamicRedrawFlag();
    
    // Screen identification
    const char* getName() const { return screenName; }
    int getId() const { return screenId; }
    
    // Validation and debugging
    virtual bool validate() const;
    void printComponents() const;
    
protected:
    // Helper methods for subclasses
    void clearScreen();
    void drawTitle(const char* title, int x = 30, int y = 20);
    
    // Component search helpers
    Component* findComponentByName(const char* name) const;
    int findComponentIndex(Component* component) const;
    
    // Layout helpers
    bool hasOverlappingComponents() const;
    void optimizeComponentOrder();  // Sort by draw order for efficiency

    // Cleanup hook for releasing resources (audio, allocations, etc.)
    virtual void cleanup() {}
};

/**
 * Button Constants for Input Handling
 * Maps to ButtonManager button indices
 */
namespace ButtonInput {
    const int BUTTON_A = 0;  // Up/Back
    const int BUTTON_B = 1;  // Down/Next  
    const int BUTTON_C = 2;  // Select/Enter
}

#endif // SCREEN_H
