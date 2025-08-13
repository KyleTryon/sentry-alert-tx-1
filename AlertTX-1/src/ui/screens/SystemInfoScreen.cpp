#include "SystemInfoScreen.h"
#include "../core/Theme.h"

void SystemInfoScreen::refreshMetrics() {
	// Battery sampling
	batteryVoltage = readBatteryVoltage();
	batteryPercent = estimateBatteryPercent(batteryVoltage);
}

void SystemInfoScreen::update() {
    Screen::update();

    // Update metrics occasionally
    unsigned long now = millis();
    if (now - lastMetricsUpdateMs > 1000) {
        lastMetricsUpdateMs = now;
        refreshMetrics();

    // Fetch current values (SSID from build-time/SettingsManager; no separate active vs configured)
    bool connected = (WiFi.status() == WL_CONNECTED);
    String cfgSsid = SettingsManager::getWifiSsid();
    String ip = connected ? WiFi.localIP().toString() : String("-");

        // Determine if anything changed
        if (connected != lastConnected ||
            cfgSsid != lastCfgSsid ||
            ip != lastIp ||
            batteryPercent != lastBatteryPercent) {
            lastConnected = connected;
            lastCfgSsid = cfgSsid;
            lastIp = ip;
            lastBatteryPercent = batteryPercent;
            shouldRedraw = true;
        }
    }
}

void SystemInfoScreen::draw() {
    if (!isActive()) return;
    if (shouldRedraw) {
        shouldRedraw = false;
        drawContent();
    }
}

void SystemInfoScreen::drawContent() {
    // Draw full screen only when data changed
    display->fillScreen(ThemeManager::getBackground());
    DisplayUtils::drawTitle(display, "System Info");

    int x = 10;
    int y = 40;
    int line = 16;

    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    display->setCursor(x, y);       display->print("WiFi SSID: "); display->print(lastCfgSsid);
    y += line;
    display->setCursor(x, y);       display->print("Connected: "); display->print(lastConnected ? "Yes" : "No");
    y += line;
    display->setCursor(x, y);       display->print("IP: "); display->print(lastIp);
    y += line * 2;
    display->setCursor(x, y);       display->print("Battery: "); display->print(batteryPercent); display->print("% ("); display->print(batteryVoltage, 2); display->print(" V)");
}

float SystemInfoScreen::readBatteryVoltage() {
	// TODO: Replace with actual ADC read once power manager exists
	// Placeholder returns nominal voltage if USB powered; otherwise a fixed value
	#ifdef USB_POWERED
	return 4.10f;
	#else
	return 3.90f;
	#endif
}

int SystemInfoScreen::estimateBatteryPercent(float vbat) {
	// Simple LiPo linear approximation between 3.5V (0%) and 4.2V (100%)
	if (vbat <= 3.50f) return 0;
	if (vbat >= 4.20f) return 100;
	return (int)(((vbat - 3.50f) / (4.20f - 3.50f)) * 100.0f + 0.5f);
}

