#include "AlertNotificationScreen.h"

AlertNotificationScreen* AlertNotificationScreen::instance = nullptr;

AlertNotificationScreen::AlertNotificationScreen(Adafruit_ST7789* display)
    : Screen(display, "AlertNotification", 99) {
    instance = this;
    
    // Set up draw regions for efficient rendering
    addDrawRegion(DirectDrawRegion::STATIC, [this, display]() { drawStaticContent(); });
    addDrawRegion(DirectDrawRegion::DYNAMIC, [this, display]() { drawDynamicContent(); });
    
    Serial.println("AlertNotificationScreen created");
}

AlertNotificationScreen::~AlertNotificationScreen() {
    if (instance == this) instance = nullptr;
    Serial.println("AlertNotificationScreen destroyed");
}

AlertNotificationScreen* AlertNotificationScreen::getInstance() {
    return instance;
}

void AlertNotificationScreen::enter() {
    Screen::enter();
    showTime = millis();
    animationFrame = 0;
    lastAnimationTime = millis();
    lastCountdownSecond = 0;
    
    // Flash LED to indicate notification
    // Note: Current LED class only supports single blink
    // TODO: Add multi-blink support to LED class
    
    Serial.println("AlertNotificationScreen: Showing notification");
    Serial.printf("  Title: %s\n", title);
    Serial.printf("  Message: %s\n", message);
    Serial.printf("  Auto-dismiss in: %lu ms\n", AUTO_DISMISS_TIME);
}

void AlertNotificationScreen::exit() {
    Screen::exit();
    Serial.println("AlertNotificationScreen: Dismissed");
}

void AlertNotificationScreen::update() {
    Screen::update();
    
    // Update animation
    updateAnimation();
    
    // Check if countdown second changed
    if (shouldAutoDismiss) {
        int remaining = getRemainingTime();
        if (remaining <= 0) {
            Serial.println("AlertNotificationScreen: Auto-dismissing");
            dismiss();
        } else {
            unsigned long currentSecond = remaining / 1000;
            if (currentSecond != lastCountdownSecond) {
                lastCountdownSecond = currentSecond;
                markDynamicContentDirty(); // Only redraw countdown area
            }
        }
    }
}

void AlertNotificationScreen::draw() {
    // Base class handles drawing based on dirty regions
    Screen::draw();
}

void AlertNotificationScreen::handleButtonPress(int button) {
    switch (button) {
        case ButtonInput::BUTTON_A:
            // Dismiss
            dismiss();
            break;
            
        case ButtonInput::BUTTON_B:
            // Toggle auto-dismiss
            shouldAutoDismiss = !shouldAutoDismiss;
            markDynamicContentDirty();
            break;
            
        case ButtonInput::BUTTON_C:
            // View details
            viewDetails();
            break;
    }
}

void AlertNotificationScreen::setMessage(const char* msgTitle, const char* msgBody, const char* msgTimestamp) {
    // Copy message data
    strncpy(title, msgTitle ? msgTitle : "New Alert", sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';
    
    strncpy(message, msgBody ? msgBody : "", sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';
    
    strncpy(timestamp, msgTimestamp ? msgTimestamp : "", sizeof(timestamp) - 1);
    timestamp[sizeof(timestamp) - 1] = '\0';
}

void AlertNotificationScreen::drawBackground() {
    // Draw a darkened overlay effect using filled rectangles for efficiency
    // Instead of pixel-by-pixel, use larger blocks for a similar effect
    uint16_t darkBg = display->color565(0, 0, 0); // Black overlay
    
    // Draw horizontal stripes for a dimming effect (much faster than pixels)
    for (int y = 0; y < DISPLAY_HEIGHT; y += 4) {
        display->fillRect(0, y, DISPLAY_WIDTH, 2, darkBg);
    }
}

void AlertNotificationScreen::drawPopupWindow() {
    // Draw popup background with border
    uint16_t bgColor = ThemeManager::getSurfaceBackground();
    uint16_t borderColor = ThemeManager::getBorder();
    uint16_t accentColor = ThemeManager::getAccent();
    
    // Draw shadow (offset by 2 pixels)
    display->fillRoundRect(POPUP_X + 2, POPUP_Y + 2, POPUP_WIDTH, POPUP_HEIGHT, 
                          BORDER_RADIUS, display->color565(0, 0, 0));
    
    // Draw main popup background
    display->fillRoundRect(POPUP_X, POPUP_Y, POPUP_WIDTH, POPUP_HEIGHT, 
                          BORDER_RADIUS, bgColor);
    
    // Draw border with accent color for urgency
    display->drawRoundRect(POPUP_X, POPUP_Y, POPUP_WIDTH, POPUP_HEIGHT, 
                          BORDER_RADIUS, accentColor);
    display->drawRoundRect(POPUP_X + 1, POPUP_Y + 1, POPUP_WIDTH - 2, POPUP_HEIGHT - 2, 
                          BORDER_RADIUS - 1, accentColor);
}

void AlertNotificationScreen::drawHeader() {
    // Draw header with icon and "NEW ALERT" text
    int headerY = POPUP_Y + PADDING;
    uint16_t headerColor = ThemeManager::getAccent();
    
    // Draw alert icon (bell symbol using text)
    display->setTextSize(2);
    display->setTextColor(headerColor);
    display->setCursor(POPUP_X + PADDING, headerY);
    display->print("\x07"); // Bell character
    
    // Draw "NEW ALERT" text
    display->setTextSize(2);
    display->setTextColor(headerColor);
    const char* headerText = "NEW ALERT";
    int textWidth = strlen(headerText) * 12; // 6 pixels per char * 2 size
    int textX = POPUP_X + (POPUP_WIDTH - textWidth) / 2;
    display->setCursor(textX, headerY);
    display->print(headerText);
    
    // Draw separator line
    int separatorY = headerY + 20;
    display->drawFastHLine(POPUP_X + PADDING, separatorY, 
                          POPUP_WIDTH - (PADDING * 2), ThemeManager::getBorder());
}

void AlertNotificationScreen::drawMessage() {
    // Draw title
    int contentY = POPUP_Y + PADDING + 30;
    display->setTextSize(1);
    display->setTextColor(ThemeManager::getPrimaryText());
    
    // Center and truncate title if needed
    char truncatedTitle[25];
    strncpy(truncatedTitle, title, sizeof(truncatedTitle) - 1);
    truncatedTitle[sizeof(truncatedTitle) - 1] = '\0';
    
    int titleWidth = strlen(truncatedTitle) * 6;
    int titleX = POPUP_X + (POPUP_WIDTH - titleWidth) / 2;
    display->setCursor(titleX, contentY);
    display->print(truncatedTitle);
    
    // Draw message preview (first line only)
    contentY += 15;
    display->setTextColor(ThemeManager::getSecondaryText());
    
    // Calculate how many characters fit in the popup width
    int maxChars = (POPUP_WIDTH - (PADDING * 2)) / 6;
    char truncatedMsg[30];
    int copyLen = (maxChars < 29) ? maxChars : 29;
    strncpy(truncatedMsg, message, copyLen);
    truncatedMsg[copyLen] = '\0';
    
    // Add ellipsis if truncated
    if (strlen(message) > maxChars) {
        strcat(truncatedMsg, "...");
    }
    
    int msgX = POPUP_X + PADDING;
    display->setCursor(msgX, contentY);
    display->print(truncatedMsg);
}

void AlertNotificationScreen::drawActions() {
    // Draw action buttons at bottom
    int actionsY = POPUP_Y + POPUP_HEIGHT - 25;
    display->setTextSize(1);
    
    // Draw button hints
    uint16_t buttonColor = ThemeManager::getPrimaryText();
    uint16_t labelColor = ThemeManager::getSecondaryText();
    
    // [A] Dismiss
    int dismissX = POPUP_X + PADDING;
    display->setTextColor(buttonColor);
    display->setCursor(dismissX, actionsY);
    display->print("[A]");
    display->setTextColor(labelColor);
    display->print(" Dismiss");
    
    // [C] View
    const char* viewText = "[C] View";
    int viewWidth = strlen(viewText) * 6;
    int viewX = POPUP_X + POPUP_WIDTH - PADDING - viewWidth;
    display->setCursor(viewX, actionsY);
    display->setTextColor(buttonColor);
    display->print("[C]");
    display->setTextColor(labelColor);
    display->print(" View");
}

void AlertNotificationScreen::drawCountdown() {
    // Draw countdown timer
    int remaining = getRemainingTime();
    if (remaining > 0) {
        int seconds = (remaining + 999) / 1000; // Round up
        
        // Draw countdown in top-right corner of popup
        char countdown[4];
        snprintf(countdown, sizeof(countdown), "%ds", seconds);
        
        display->setTextSize(1);
        display->setTextColor(ThemeManager::getSecondaryText());
        
        int countdownWidth = strlen(countdown) * 6;
        int countdownX = POPUP_X + POPUP_WIDTH - PADDING - countdownWidth;
        int countdownY = POPUP_Y + PADDING;
        
        display->setCursor(countdownX, countdownY);
        display->print(countdown);
    }
}

void AlertNotificationScreen::dismiss() {
    // Simply pop back to previous screen
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->popScreen();
    }
}

void AlertNotificationScreen::viewDetails() {
    // Navigate to AlertsScreen
    // The message is already added to AlertsScreen, so just navigate there
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        // First pop this notification screen
        manager->popScreen();
        
        // Then push AlertsScreen if we're not already there
        Screen* currentScreen = manager->getCurrentScreen();
        if (currentScreen && strcmp(currentScreen->getName(), "Alerts") != 0) {
            // Get alerts screen instance from MainMenuScreen
            // This is a bit hacky but works with current architecture
            AlertsScreen* alertsScreen = AlertsScreen::getInstance();
            if (alertsScreen) {
                manager->pushScreen(alertsScreen);
            }
        }
    }
}

int AlertNotificationScreen::getRemainingTime() const {
    if (!shouldAutoDismiss) return AUTO_DISMISS_TIME;
    
    unsigned long elapsed = millis() - showTime;
    if (elapsed >= AUTO_DISMISS_TIME) return 0;
    
    return AUTO_DISMISS_TIME - elapsed;
}

void AlertNotificationScreen::updateAnimation() {
    unsigned long now = millis();
    if (now - lastAnimationTime >= ANIMATION_SPEED) {
        animationFrame = (animationFrame + 1) % 4;
        lastAnimationTime = now;
        
        // Only redraw if we're doing animation (future enhancement)
        // For now, no animation to save battery
    }
}

void AlertNotificationScreen::drawStaticContent() {
    // Draw background and popup window (doesn't change)
    drawBackground();
    drawPopupWindow();
    drawHeader();
    drawMessage();
    drawActions();
}

void AlertNotificationScreen::drawDynamicContent() {
    // Draw countdown if auto-dismiss is enabled
    if (shouldAutoDismiss) {
        drawCountdown();
    }
}
