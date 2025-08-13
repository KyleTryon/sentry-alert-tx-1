#include "GamesScreen.h"
#include "../core/ScreenManager.h"
#include "PongScreen.h"

GamesScreen* GamesScreen::instance = nullptr;

GamesScreen::GamesScreen(Adafruit_ST7789* display)
    : Screen(display, "Games", 1), gamesMenu(nullptr), pongScreen(nullptr) {
    Serial.println("GamesScreen created");
    instance = this;
}

GamesScreen::~GamesScreen() {
    Serial.println("GamesScreen destroyed");
    instance = nullptr;
}

void GamesScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("GAMES");
    Serial.println("Entered GamesScreen");
    if (!gamesMenu) {
        gamesMenu = new MenuContainer(display, 10, 50);
        addComponent(gamesMenu);
        setupMenu();
    } else {
        gamesMenu->setVisible(true);
        gamesMenu->setSelectedIndex(0);
        gamesMenu->markDirty();
    }
}

void GamesScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("GAMES");
    Serial.println("Exited GamesScreen");
}

void GamesScreen::update() {
    Screen::update();
    // GamesScreen doesn't need special update logic
    // Could add game preview animations here
}

void GamesScreen::draw() {
    Screen::draw();
    DisplayUtils::drawTitle(display, "Games");
}

void GamesScreen::handleButtonPress(int button) {
    Serial.printf("GamesScreen: Button %d pressed\n", button);
    if (gamesMenu) {
        gamesMenu->handleButtonPress(button);
    }
}

// Private helper methods

void GamesScreen::setupMenu() {
    if (!gamesMenu) return;
    gamesMenu->clear();
    gamesMenu->addMenuItem("Pong", 1, pongCallback);
    gamesMenu->autoLayout();
}

void GamesScreen::pongCallback() {
    if (instance) instance->onPongSelected();
}

void GamesScreen::onPongSelected() {
    navigateToPong();
}

void GamesScreen::navigateToPong() {
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (!manager) return;
    // Create a fresh instance each time and transfer ownership to the manager
    PongScreen* fresh = new PongScreen(display);
    manager->pushScreen(fresh, true /*takeOwnership*/);
    // No per-screen cooldown; global input router/back debounce handles stale inputs
}
