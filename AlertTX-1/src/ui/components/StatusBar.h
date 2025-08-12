#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "../core/Theme.h"
#include "../renderer/DisplayRenderer.h"
#include "IconRenderer.h"

class StatusBar {
private:
    int y = 0;               // Top of screen
    int height = 16;         // Status bar height
    bool needsRedraw = true;
    
    // Status data
    int batteryLevel = 100;       // 0-100%
    bool wifiConnected = false;
    bool bluetoothConnected = false;
    char timeString[9] = "00:00:00";
    
    // Layout constants
    static const int ICON_SIZE = 12;
    static const int ICON_SPACING = 18;
    static const int LEFT_MARGIN = 4;
    static const int RIGHT_MARGIN = 4;
    
public:
    StatusBar();
    
    // Main drawing function
    void draw(DisplayRenderer* renderer, const Theme& theme);
    
    // Data updates
    void updateBatteryLevel(int percentage);
    void updateWiFiStatus(bool connected);
    void updateBluetoothStatus(bool connected);
    void updateTime(const char* timeStr);
    void updateTime(int hours, int minutes, int seconds);
    
    // State management
    void markForRedraw() { needsRedraw = true; }
    bool hasChanged() const { return needsRedraw; }
    
    // Layout info
    int getHeight() const { return height; }
    int getY() const { return y; }
    
private:
    void drawBatteryIcon(DisplayRenderer* renderer, int x, int y, int level, const Theme& theme);
    void drawWiFiIcon(DisplayRenderer* renderer, int x, int y, bool connected, const Theme& theme);
    void drawBluetoothIcon(DisplayRenderer* renderer, int x, int y, bool connected, const Theme& theme);
    void drawTime(DisplayRenderer* renderer, int x, int y, const char* time, const Theme& theme);
    void drawSeparator(DisplayRenderer* renderer, int x, int y, const Theme& theme);
};

#endif // STATUS_BAR_H
