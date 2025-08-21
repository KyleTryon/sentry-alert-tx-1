#include "AlertsScreen.h"
#include "../core/ScreenManager.h"
#include "../../config/SettingsManager.h"

AlertsScreen* AlertsScreen::instance = nullptr;

AlertsScreen::AlertsScreen(Adafruit_ST7789* display)
    : Screen(display, "Alerts", 1) {
    instance = this;
    messageCount = 0;
    selectedIndex = 0;
    scrollOffset = 0;
    
    // Set up draw regions for efficient rendering
    addDrawRegion(DirectDrawRegion::STATIC, [this, display]() { drawHeader(); });
    addDrawRegion(DirectDrawRegion::DYNAMIC, [this, display]() { drawList(); });
    
    Serial.println("AlertsScreen created");
}

AlertsScreen::~AlertsScreen() {
    if (instance == this) instance = nullptr;
    Serial.println("AlertsScreen destroyed");
}

AlertsScreen* AlertsScreen::getInstance() {
    return instance;
}

void AlertsScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("ALERTS");
    Serial.println("Entered AlertsScreen");
}

void AlertsScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("ALERTS");
    Serial.println("Exited AlertsScreen");
}

void AlertsScreen::update() {
    Screen::update();
}

void AlertsScreen::draw() {
    // Base class handles drawing based on dirty regions
    Screen::draw();
}

void AlertsScreen::drawHeader() {
    DisplayUtils::drawTitle(display, "Alerts");
}

void AlertsScreen::drawList() {
    int availableHeight = DISPLAY_HEIGHT - LIST_START_Y - 4;
    visibleRows = availableHeight / ROW_HEIGHT;
    if (visibleRows < 1) visibleRows = 1;

    // Clear list area and draw a top separator to avoid artifacts under the title
    display->fillRect(0, LIST_START_Y, DISPLAY_WIDTH, availableHeight, ThemeManager::getSurfaceBackground());
    DisplayUtils::drawSeparatorLine(display, LIST_START_Y - 1, ThemeManager::getBorder());

    if (messageCount == 0) {
        // Empty state placeholder (treated like a read message)
        const char* emptyMsg = "No new messages";
        int midY = LIST_START_Y + (availableHeight / 2) - 4;
        DisplayUtils::centerTextWithColor(display, emptyMsg, 1, midY, ThemeManager::getSecondaryText());
        return;
    }

    int endIndex = min(messageCount, scrollOffset + visibleRows);
    int y = LIST_START_Y + 2;
    for (int i = scrollOffset; i < endIndex; ++i) {
        drawRow(i, y);
        y += ROW_HEIGHT;
    }

    display->setTextSize(1);
    display->setTextColor(ThemeManager::getSecondaryText());
    if (scrollOffset > 0) {
        display->setCursor(DISPLAY_WIDTH - 10, LIST_START_Y + 2);
        display->print('^');
    }
    if (scrollOffset + visibleRows < messageCount) {
        display->setCursor(DISPLAY_WIDTH - 10, LIST_START_Y + availableHeight - 10);
        display->print('v');
    }
}

void AlertsScreen::drawRow(int index, int y) {
    const bool isSelected = (index == selectedIndex);
    const AlertMessage& msg = messages[index];

    uint16_t bg = isSelected ? ThemeManager::getAccent() : ThemeManager::getSurfaceBackground();
    uint16_t fg = isSelected ? ThemeManager::getSelectedText() : ThemeManager::getPrimaryText();
    display->fillRect(1, y, DISPLAY_WIDTH - 2, ROW_HEIGHT - 2, bg);

    display->setTextSize(1);
    display->setTextColor(fg);
    int textX = ICON_PADDING_X + TEXT_PADDING_X; // no icon
    int textY = y + 6;
    display->setCursor(textX, textY);
    char titleBuf[22];
    strncpy(titleBuf, msg.title, sizeof(titleBuf) - 1);
    titleBuf[sizeof(titleBuf) - 1] = '\0';
    display->print(titleBuf);
    if (strlen(msg.timestamp) > 0) {
        int tsWidth = DisplayUtils::getTextWidth(display, msg.timestamp, 1);
        display->setCursor(DISPLAY_WIDTH - tsWidth - 6, textY);
        display->print(msg.timestamp);
    }
    display->setCursor(textX, y + 16);
    char bodyBuf[28];
    strncpy(bodyBuf, msg.message, sizeof(bodyBuf) - 1);
    bodyBuf[sizeof(bodyBuf) - 1] = '\0';
    display->print(bodyBuf);
}

void AlertsScreen::handleButtonPress(int button) {
    switch (button) {
        case ButtonInput::BUTTON_A:
            moveUp();
            break;
        case ButtonInput::BUTTON_B:
            moveDown();
            break;
        case ButtonInput::BUTTON_C:
            openDetail();
            break;
    }
}

void AlertsScreen::ensureSelectionVisible() {
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + visibleRows) {
        scrollOffset = selectedIndex - visibleRows + 1;
    }
}

void AlertsScreen::moveUp() {
    if (messageCount == 0) return;
    int old = selectedIndex;
    selectedIndex = (selectedIndex - 1 + messageCount) % messageCount;
    ensureSelectionVisible();
    markDynamicContentDirty();
    Serial.printf("AlertsScreen: %d -> %d (up)\n", old, selectedIndex);
}

void AlertsScreen::moveDown() {
    if (messageCount == 0) return;
    int old = selectedIndex;
    selectedIndex = (selectedIndex + 1) % messageCount;
    ensureSelectionVisible();
    markDynamicContentDirty();
    Serial.printf("AlertsScreen: %d -> %d (down)\n", old, selectedIndex);
}

void AlertsScreen::openDetail() {
    if (selectedIndex < 0 || selectedIndex >= messageCount) return;
    // Mark as read
    messages[selectedIndex].unread = false;
    // Lazily create detail screen
    if (!detailScreen) {
        detailScreen = new AlertDetailScreen(display, this);
    }
    detailScreen->setMessage(messages[selectedIndex]);
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(detailScreen);
    }
}

void AlertsScreen::toggleRead() {
    if (selectedIndex < 0 || selectedIndex >= messageCount) return;
    messages[selectedIndex].unread = !messages[selectedIndex].unread;
    markDynamicContentDirty();
}

void AlertsScreen::addMessage(const char* title, const char* body, const char* timestamp, bool playTone) {
    if (messageCount >= MAX_MESSAGES) {
        for (int i = MAX_MESSAGES - 1; i > 0; --i) {
            messages[i] = messages[i - 1];
        }
        messageCount = MAX_MESSAGES - 1;
    } else {
        for (int i = messageCount; i > 0; --i) {
            messages[i] = messages[i - 1];
        }
    }

    AlertMessage& m = messages[0];
    strncpy(m.title, title ? title : "(No title)", sizeof(m.title) - 1);
    m.title[sizeof(m.title) - 1] = '\0';
    strncpy(m.message, body ? body : "", sizeof(m.message) - 1);
    m.message[sizeof(m.message) - 1] = '\0';
    strncpy(m.timestamp, timestamp ? timestamp : "", sizeof(m.timestamp) - 1);
    m.timestamp[sizeof(m.timestamp) - 1] = '\0';
    m.unread = true;

    messageCount++;
    selectedIndex = 0;
    scrollOffset = 0;

    if (playTone) {
        int idx = SettingsManager::getRingtoneIndex();
        if (idx < 0) idx = 0;
        ringtonePlayer.playRingtoneByIndex(idx);
    }

    markDynamicContentDirty();
}
