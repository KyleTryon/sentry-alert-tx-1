#ifndef ALERTSSCREEN_H
#define ALERTSSCREEN_H

#include "../core/Screen.h"
#include "../../config/DisplayConfig.h"
#include "../core/DisplayUtils.h"
#include "../core/ScreenManager.h"
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
    static const int LIST_START_Y = MENU_START_Y; // Align with other screens
    static const int ICON_PADDING_X = 10;
    static const int TEXT_PADDING_X = 10;

    // Redraw control
    bool listDirty = true;

    // Forward-declared detail screen class
    class AlertDetailScreen;
    AlertDetailScreen* detailScreen = nullptr;

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
    void invalidateList() { listDirty = true; }

    void moveUp();
    void moveDown();
    void openDetail();

    // Toggle helper retained if needed elsewhere
    void toggleRead();

    // Detail screen definition
    class AlertDetailScreen : public Screen {
    private:
        AlertsScreen* parent;
        AlertMessage message;
    public:
        AlertDetailScreen(Adafruit_ST7789* display, AlertsScreen* parentRef)
            : Screen(display, "AlertDetail", 100), parent(parentRef) {}
        void setMessage(const AlertMessage& m) { message = m; }
        void enter() override { Screen::enter(); }
        void exit() override { Screen::exit(); }
        void update() override { Screen::update(); }
        void draw() override {
            Screen::draw();
            // Title bar
            DisplayUtils::drawTitle(display, "Alert");
            // Body area
            display->fillRect(0, LIST_START_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT - LIST_START_Y, ThemeManager::getSurfaceBackground());
            display->setTextColor(ThemeManager::getPrimaryText());
            display->setTextSize(1);
            int x = 10;
            int y = LIST_START_Y + 4;
            // Title
            display->setCursor(x, y);
            display->println(message.title);
            y += 12;
            // Timestamp (dim)
            display->setTextColor(ThemeManager::getSecondaryText());
            display->setCursor(x, y);
            display->println(message.timestamp);
            y += 12;
            // Body
            display->setTextColor(ThemeManager::getPrimaryText());
            display->setCursor(x, y);
            // Simple wrap: draw in chunks that fit width
            int maxWidth = DISPLAY_WIDTH - 2 * x;
            const char* p = message.message;
            char line[48];
            while (*p) {
                int len = 0;
                line[0] = '\0';
                // accumulate until width would overflow or newline
                while (p[len] && p[len] != '\n') {
                    strncpy(line, p, len + 1);
                    line[len + 1] = '\0';
                    if (DisplayUtils::getTextWidth(display, line, 1) > maxWidth) {
                        line[len] = '\0';
                        break;
                    }
                    len++;
                    if (len >= (int)sizeof(line) - 1) break;
                }
                if (len == 0) {
                    // single word too long or newline
                    display->println(" ");
                    p += (p[0] == '\n') ? 1 : 0;
                } else {
                    display->println(line);
                    p += len;
                }
                if (*p == '\n') { p++; }
                y += 10;
                if (y > DISPLAY_HEIGHT - 10) break;
                display->setCursor(x, y);
            }
        }
        void handleButtonPress(int button) override {
            // Back is handled globally via long-press; short press C can also go back
            if (button == ButtonInput::BUTTON_C) {
                GlobalScreenManager::getInstance()->popScreen();
            }
        }
    };
};

#endif // ALERTSSCREEN_H
