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
    SystemInfoScreen(Adafruit_ST7789* display);

 	 		void enter() override {
 			Screen::enter();
 			refreshMetrics();
 			shouldRedraw = true;
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
	void drawLabels();
	void drawValues();
 	static float readBatteryVoltage();
 	static int estimateBatteryPercent(float vbat);
};

#endif // SYSTEMINFOSCREEN_H

