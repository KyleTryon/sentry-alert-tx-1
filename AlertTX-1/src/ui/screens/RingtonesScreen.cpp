#include "RingtonesScreen.h"
#include "../core/ScreenManager.h"
#include "../../config/SettingsManager.h"
#include "../../ringtones/RingtonePlayer.h"

RingtonesScreen::RingtonesScreen(Adafruit_ST7789* display)
    : Screen(display, "Ringtones", 3), ringtoneMenu(nullptr), lastPreviewIndex(-1), autoPreviewEnabled(true), pendingPreviewIndex(-1), previewDueAtMs(0) {
    ringtoneMenu = new MenuContainer(display, 10, 50);
    addComponent(ringtoneMenu);
    buildMenu();
    
    // Set up draw regions for efficient rendering
    addDrawRegion(DirectDrawRegion::STATIC, [this, display]() { 
        DisplayUtils::drawTitle(display, "Ringtones");
    });
}

RingtonesScreen::~RingtonesScreen() {
}

void RingtonesScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("RINGTONES");
    int saved = SettingsManager::getRingtoneIndex();
    if (saved >= 0 && saved < ringtonePlayer.getRingtoneCount()) {
        ringtoneMenu->setSelectedIndex(saved);
        previewIndex(saved);
    } else if (ringtonePlayer.getRingtoneCount() > 0) {
        ringtoneMenu->setSelectedIndex(0);
        previewIndex(0);
    }
}

void RingtonesScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("RINGTONES");
    ringtonePlayer.stop();
}

void RingtonesScreen::update() {
    Screen::update();
    if (autoPreviewEnabled && pendingPreviewIndex >= 0 && millis() >= previewDueAtMs) {
        previewIndex(pendingPreviewIndex);
        pendingPreviewIndex = -1;
    }
}

void RingtonesScreen::draw() {
    // Base class handles drawing based on dirty regions
    Screen::draw();
}

void RingtonesScreen::handleButtonPress(int button) {
    if (ringtoneMenu) {
        // Navigate list and auto-preview via selection-changed callback
        ringtoneMenu->handleButtonPress(button);
        // Confirm selection on Button C (2): persist and go back
        if (button == 2) {
            int idx = ringtoneMenu->getSelectedIndex();
            persistSelection(idx);
            ScreenManager* manager = GlobalScreenManager::getInstance();
            if (manager) manager->popScreen();
        }
    }
}

void RingtonesScreen::buildMenu() {
    ringtoneMenu->clear();
    int count = ringtonePlayer.getRingtoneCount();
    for (int i = 0; i < count && i < 8; i++) {
        const char* name = ringtonePlayer.getRingtoneName(i);
        int idx = i;
        ringtoneMenu->addMenuItem(name ? name : "(unnamed)", idx, [this, idx]() {
            // Enter key: confirm selection
            persistSelection(idx);
            ScreenManager* manager = GlobalScreenManager::getInstance();
            if (manager) manager->popScreen();
        });
    }
    ringtoneMenu->setOnSelectionChanged([this](int newIndex){
        // Debounce actual playback to avoid rapid restarts
        pendingPreviewIndex = newIndex;
        previewDueAtMs = millis() + PREVIEW_DEBOUNCE_MS;
    });
    ringtoneMenu->autoLayout();
}

void RingtonesScreen::previewIndex(int index) {
    if (!autoPreviewEnabled) return;
    if (index < 0 || index >= ringtonePlayer.getRingtoneCount()) return;
    if (lastPreviewIndex == index) return;
    // Stop any current preview, then play new
    ringtonePlayer.stop();
    ringtonePlayer.playRingtoneByIndex(index);
    lastPreviewIndex = index;
}

void RingtonesScreen::persistSelection(int index) {
    if (index < 0 || index >= ringtonePlayer.getRingtoneCount()) return;
    SettingsManager::setRingtoneIndex(index);
}


