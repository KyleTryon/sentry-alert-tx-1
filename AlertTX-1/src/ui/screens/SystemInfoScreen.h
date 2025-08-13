#ifndef SYSTEMINFOSCREEN_H
#define SYSTEMINFOSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../../config/SettingsManager.h"
#include <WiFi.h>

/**
 * SystemInfoScreen
 *
 * Displays runtime system information:
 * - Configured WiFi SSID (from SettingsManager)
 * - Current connection status and active SSID/IP
 * - Battery percentage estimate
 */
class SystemInfoScreen : public Screen {
public:
    SystemInfoScreen(Adafruit_ST7789* display)
         : Screen(display, "SystemInfo", 0), batteryPercent(0), batteryVoltage(0.0f), lastRenderMs(0), shouldRedraw(true),
           lastConnected(false), lastCfgSsid(""), lastIp(""), lastBatteryPercent(-1), lastMetricsUpdateMs(0) {}

 	void enter() override {
 		Screen::enter();
 		refreshMetrics();
 	}

	void update() override;

	void draw() override;

 	void handleButtonPress(int /*button*/) override {
 		// No interactive elements for now
 	}

private:
 	int batteryPercent;
 	float batteryVoltage;
	unsigned long lastRenderMs;
    bool shouldRedraw;
    bool lastConnected;
    String lastCfgSsid;
    String lastIp;
    int lastBatteryPercent;
    unsigned long lastMetricsUpdateMs;

 	void refreshMetrics();
	void drawContent();
 	static float readBatteryVoltage();
 	static int estimateBatteryPercent(float vbat);
};

#endif // SYSTEMINFOSCREEN_H

