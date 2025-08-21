#ifndef ALERTNOTIFICATIONSCREEN_H
#define ALERTNOTIFICATIONSCREEN_H

#include "../core/Screen.h"
#include "../../config/DisplayConfig.h"
#include "../core/DisplayUtils.h"
#include "../core/ScreenManager.h"
#include "AlertsScreen.h"

/**
 * AlertNotificationScreen
 * 
 * Popup-style notification screen that appears when new MQTT alerts arrive.
 * Shows a centered notification with title and preview, with options to
 * dismiss or view the full alert.
 * 
 * Features:
 * - Auto-dismiss after 10 seconds
 * - Centered popup design
 * - Quick actions (dismiss/view)
 */

class AlertNotificationScreen : public Screen {
private:
    // Message data
    char title[64];
    char message[96];
    char timestamp[24];
    
    // Timing
    unsigned long showTime = 0;
    static const unsigned long AUTO_DISMISS_TIME = 10000; // 10 seconds
    bool shouldAutoDismiss = true;
    
    // UI Layout
    static const int POPUP_WIDTH = 200;
    static const int POPUP_HEIGHT = 120;
    static const int POPUP_X = (DISPLAY_WIDTH - POPUP_WIDTH) / 2;
    static const int POPUP_Y = (DISPLAY_HEIGHT - POPUP_HEIGHT) / 2;
    static const int BORDER_RADIUS = 8;
    static const int PADDING = 12;
    
    // Animation
    int animationFrame = 0;
    unsigned long lastAnimationTime = 0;
    static const unsigned long ANIMATION_SPEED = 100; // ms per frame
    
    // Countdown tracking
    unsigned long lastCountdownSecond = 0;
    
public:
    AlertNotificationScreen(Adafruit_ST7789* display);
    ~AlertNotificationScreen();
    
    // Screen interface
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    void handleButtonPress(int button) override;
    
    // Set the message to display
    void setMessage(const char* msgTitle, const char* msgBody, const char* msgTimestamp);
    
    // Control auto-dismiss
    void setAutoDismiss(bool enabled) { shouldAutoDismiss = enabled; }
    
    // Get singleton instance
    static AlertNotificationScreen* getInstance();
    
private:
    static AlertNotificationScreen* instance;
    
    // Drawing helpers
    void drawBackground();
    void drawPopupWindow();
    void drawHeader();
    void drawMessage();
    void drawActions();
    void drawCountdown();
    
    // Actions
    void dismiss();
    void viewDetails();
    
    // Helpers
    int getRemainingTime() const;
    void updateAnimation();
    
    // Centralized rendering methods
    void drawStaticContent();
    void drawDynamicContent();
};

#endif // ALERTNOTIFICATIONSCREEN_H
