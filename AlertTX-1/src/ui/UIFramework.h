#ifndef UI_FRAMEWORK_H
#define UI_FRAMEWORK_H

/**
 * Alert TX-1 UI Framework
 * 
 * A React-like declarative UI system for Arduino/ESP32 with:
 * - Component-based architecture
 * - Declarative rendering
 * - State management with dirty tracking
 * - Non-blocking operation
 * - Integration with Adafruit GFX displays
 */

// Core framework classes
#include "core/UIComponent.h"
#include "core/StatefulComponent.h"
#include "core/Screen.h"
#include "core/UIManager.h"

// Renderer utilities
#include "renderer/Renderer.h"

// Basic UI components
#include "components/Label.h"
#include "components/Button.h"
#include "components/ProgressBar.h"
#include "components/Menu.h"

// Pre-built screens
#include "screens/HomeScreen.h"
#include "screens/RingtonesScreen.h"

/**
 * UI Framework version and metadata
 */
namespace UIFramework {
    static const char* VERSION = "1.0.0";
    static const char* BUILD_DATE = __DATE__;
    static const char* DESCRIPTION = "React-like UI Framework for Alert TX-1";
    
    /**
     * Initialize the UI framework with required display reference.
     */
    static void initialize() {
        Serial.println("Alert TX-1 UI Framework v" + String(VERSION));
        Serial.println("Build: " + String(BUILD_DATE));
    }
}

#endif // UI_FRAMEWORK_H