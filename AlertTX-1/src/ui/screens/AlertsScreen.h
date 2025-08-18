#ifndef ALERTSSCREEN_H
#define ALERTSSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../../icons/mail_16.h"
#include "../../icons/mail-unread_16.h"
#include "../../ringtones/RingtonePlayer.h"

/**
 * AlertsScreen
 * 
 * List view for incoming alerts (email/message style) with read/unread state.
 */

class AlertsScreen : public Screen {
private:
    struct AlertMessage {
        char title[64];
        char message[96];
        char timestamp[24];
        bool unread;
    };

    static const int MAX_MESSAGES = 20;
    AlertMessage messages[MAX_MESSAGES];
    int messageCount = 0;

    int selectedIndex = 0;
    int scrollOffset = 0;
    int visibleRows = 4;

    static const int ROW_HEIGHT = 28;
    static const int LIST_START_Y = 40;
    static const int ICON_PADDING_X = 10;
    static const int TEXT_PADDING_X = 10;

public:
    AlertsScreen(Adafruit_ST7789* display);
    ~AlertsScreen();
    
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    void handleButtonPress(int button) override;

    void addMessage(const char* title, const char* body, const char* timestamp, bool playTone = true);

    static AlertsScreen* getInstance();

private:
    static AlertsScreen* instance;

    void drawHeader();
    void drawList();
    void drawRow(int index, int y);
    void ensureSelectionVisible();

    void moveUp();
    void moveDown();
    void toggleRead();
};

#endif // ALERTSSCREEN_H
