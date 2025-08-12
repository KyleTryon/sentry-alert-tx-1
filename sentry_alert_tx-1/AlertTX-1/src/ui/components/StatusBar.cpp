#include "StatusBar.h"
#include <Arduino.h>
#include <stdio.h>

StatusBar::StatusBar() {
    // Initialize with default values
    batteryLevel = 100;
    wifiConnected = false;
    bluetoothConnected = false;
    strcpy(timeString, "00:00:00");
    needsRedraw = true;
}

void StatusBar::draw(DisplayRenderer* renderer, const Theme& theme) {
    if (!renderer || !needsRedraw) return;
    
    // Clear status bar area
    renderer->fillRect(0, y, 240, height, theme.background);
    
    int currentX = LEFT_MARGIN;
    
    // Draw battery icon on the left
    drawBatteryIcon(renderer, currentX, y + 2, batteryLevel, theme);
    currentX += ICON_SPACING;
    
    // Draw WiFi icon
    drawWiFiIcon(renderer, currentX, y + 2, wifiConnected, theme);
    currentX += ICON_SPACING;
    
    // Draw Bluetooth icon
    drawBluetoothIcon(renderer, currentX, y + 2, bluetoothConnected, theme);
    
    // Draw time on the right side
    int timeWidth = strlen(timeString) * 6; // Assuming 6px wide font
    drawTime(renderer, 240 - RIGHT_MARGIN - timeWidth, y + 4, timeString, theme);
    
    needsRedraw = false;
}

void StatusBar::updateBatteryLevel(int percentage) {
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    
    if (batteryLevel != percentage) {
        batteryLevel = percentage;
        needsRedraw = true;
    }
}

void StatusBar::updateWiFiStatus(bool connected) {
    if (wifiConnected != connected) {
        wifiConnected = connected;
        needsRedraw = true;
    }
}

void StatusBar::updateBluetoothStatus(bool connected) {
    if (bluetoothConnected != connected) {
        bluetoothConnected = connected;
        needsRedraw = true;
    }
}

void StatusBar::updateTime(const char* timeStr) {
    if (timeStr && strncmp(timeString, timeStr, 8) != 0) {
        strncpy(timeString, timeStr, 8);
        timeString[8] = '\0';
        needsRedraw = true;
    }
}

void StatusBar::updateTime(int hours, int minutes, int seconds) {
    char newTimeString[9];
    snprintf(newTimeString, sizeof(newTimeString), "%02d:%02d:%02d", hours, minutes, seconds);
    updateTime(newTimeString);
}

void StatusBar::drawBatteryIcon(DisplayRenderer* renderer, int x, int y, int level, const Theme& theme) {
    // Choose appropriate battery icon based on level
    const Icon* batteryIcon;
    
    if (level > 75) {
        batteryIcon = ICON_BATTERY_FULL;
    } else if (level > 50) {
        batteryIcon = ICON_BATTERY;
    } else if (level > 25) {
        batteryIcon = ICON_BATTERY_LOW;
    } else {
        batteryIcon = ICON_BATTERY_LOW;
    }
    
    // Use appropriate color based on battery level
    uint16_t iconColor;
    if (level > 20) {
        iconColor = theme.foreground;
    } else {
        iconColor = RGB565_AMBER; // Warning color for low battery
    }
    
    if (batteryIcon) {
        IconRenderer::drawIcon(renderer, *batteryIcon, x, y, iconColor);
    } else {
        // Fallback: simple battery outline
        renderer->drawRect(x, y, 12, 6, iconColor);
        renderer->drawRect(x + 12, y + 2, 2, 2, iconColor);
        
        // Fill based on level
        int fillWidth = (level * 10) / 100;
        if (fillWidth > 0) {
            renderer->fillRect(x + 1, y + 1, fillWidth, 4, iconColor);
        }
    }
}

void StatusBar::drawWiFiIcon(DisplayRenderer* renderer, int x, int y, bool connected, const Theme& theme) {
    uint16_t iconColor = connected ? theme.foreground : theme.secondary;
    
    const Icon* wifiIcon = connected ? ICON_WIFI_3 : ICON_WIFI_0;
    
    if (wifiIcon) {
        IconRenderer::drawIcon(renderer, *wifiIcon, x, y, iconColor);
    } else {
        // Fallback: simple WiFi icon (3 arcs)
        for (int i = 0; i < 3; i++) {
            int radius = 3 + i;
            renderer->drawCircle(x + 6, y + 6, radius, iconColor);
        }
    }
}

void StatusBar::drawBluetoothIcon(DisplayRenderer* renderer, int x, int y, bool connected, const Theme& theme) {
    uint16_t iconColor = connected ? theme.foreground : theme.secondary;
    
    // Simple Bluetooth icon
    // Draw a stylized "B" shape
    renderer->drawLine(x + 4, y, x + 4, y + 12, iconColor);
    renderer->drawLine(x + 4, y, x + 8, y + 3, iconColor);
    renderer->drawLine(x + 8, y + 3, x + 4, y + 6, iconColor);
    renderer->drawLine(x + 4, y + 6, x + 8, y + 9, iconColor);
    renderer->drawLine(x + 8, y + 9, x + 4, y + 12, iconColor);
}

void StatusBar::drawTime(DisplayRenderer* renderer, int x, int y, const char* time, const Theme& theme) {
    renderer->setTextColor(theme.foreground);
    renderer->setTextSize(1);
    renderer->setCursor(x, y);
    renderer->print(time);
}

void StatusBar::drawSeparator(DisplayRenderer* renderer, int x, int y, const Theme& theme) {
    // Draw a small vertical line separator
    renderer->drawLine(x, y + 2, x, y + height - 4, theme.secondary);
}