#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "../core/Screen.h"
#include "../components/Label.h"
#include "../components/Button.h"
#include "../components/ProgressBar.h"
#include "../components/Menu.h"
#include "../../hardware/ButtonManager.h"
#include "../../ringtones/RingtonePlayer.h"

/**
 * Home screen for the Alert TX-1 device.
 * Demonstrates the UI framework capabilities and provides main navigation.
 */
class HomeScreen : public Screen {
private:
    Label* titleLabel = nullptr;
    Label* statusLabel = nullptr;
    Button* ringtonesButton = nullptr;
    Button* gameButton = nullptr;
    Button* settingsButton = nullptr;
    ProgressBar* batteryBar = nullptr;
    Menu* quickMenu = nullptr;
    
    // Hardware references
    ButtonManager* buttonManager = nullptr;
    RingtonePlayer* ringtonePlayer = nullptr;
    
    // State
    String deviceStatus = "Ready";
    float batteryLevel = 0.85f;  // 85%
    bool isConnected = false;
    unsigned long lastUpdate = 0;

public:
    HomeScreen(ButtonManager* btnMgr = nullptr, RingtonePlayer* player = nullptr) 
        : buttonManager(btnMgr), ringtonePlayer(player) {}

    /**
     * Build the home screen components.
     */
    void build() override {
        // Title
        titleLabel = addComponent<Label>(10, 10, 220, 20, "Alert TX-1", COLOR_WHITE);
        titleLabel->setTextSize(2);
        titleLabel->setCenterAlign(true);
        titleLabel->setBackgroundColor(COLOR_BLUE, true);
        
        // Status label
        statusLabel = addComponent<Label>(10, 35, 150, 15, deviceStatus, COLOR_GREEN);
        statusLabel->setTextSize(1);
        
        // Battery indicator
        batteryBar = addComponent<ProgressBar>(170, 35, 60, 10, batteryLevel);
        batteryBar->setColors(COLOR_GREEN, COLOR_DARK_GRAY, COLOR_GRAY);
        batteryBar->setShowPercentage(false);
        
        // Main buttons
        ringtonesButton = addComponent<Button>(10, 55, 70, 25, "Ringtones", [this]() {
            onRingtonesClicked();
        });
        
        gameButton = addComponent<Button>(85, 55, 70, 25, "Game", [this]() {
            onGameClicked();
        });
        
        settingsButton = addComponent<Button>(160, 55, 70, 25, "Settings", [this]() {
            onSettingsClicked();
        });
        
        // Quick access menu
        quickMenu = addComponent<Menu>(10, 90, 220, 40);
        quickMenu->setItemHeight(12);
        quickMenu->addItem("Play Last Ringtone", [this]() { playLastRingtone(); });
        quickMenu->addItem("Start BeeperHero", [this]() { startGame(); });
        quickMenu->addItem("Device Info", [this]() { showDeviceInfo(); });
        quickMenu->addItem("Power Options", [this]() { showPowerOptions(); });
        
        // Set initial button highlighting
        ringtonesButton->setHighlighted(true);
    }
    
    /**
     * Update the home screen state.
     */
    void update() override {
        Screen::update();
        
        unsigned long now = millis();
        if (now - lastUpdate > 1000) { // Update every second
            updateStatus();
            lastUpdate = now;
        }
        
        // Handle hardware button input
        if (buttonManager) {
            handleButtonInput();
        }
    }
    
    /**
     * Handle screen activation.
     */
    void onActivate() override {
        Screen::onActivate();
        updateStatus();
    }

private:
    /**
     * Update device status and battery level.
     */
    void updateStatus() {
        // Update battery level (simulate battery monitoring)
        // In real implementation, this would read from battery monitor
        static float batteryTrend = -0.001f;
        batteryLevel += batteryTrend;
        if (batteryLevel <= 0.2f || batteryLevel >= 1.0f) {
            batteryTrend = -batteryTrend;
        }
        batteryLevel = constrain(batteryLevel, 0.0f, 1.0f);
        
        if (batteryBar) {
            batteryBar->setProgress(batteryLevel);
            
            // Change color based on battery level
            uint16_t batteryColor = COLOR_GREEN;
            if (batteryLevel < 0.3f) {
                batteryColor = COLOR_RED;
            } else if (batteryLevel < 0.5f) {
                batteryColor = COLOR_ORANGE;
            }
            batteryBar->setColors(batteryColor);
        }
        
        // Update status text
        String newStatus = "Ready";
        if (ringtonePlayer && ringtonePlayer->isPlaying()) {
            newStatus = "Playing";
        } else if (batteryLevel < 0.2f) {
            newStatus = "Low Battery";
        }
        
        if (statusLabel && deviceStatus != newStatus) {
            deviceStatus = newStatus;
            statusLabel->setText(deviceStatus);
            
            // Change status color
            uint16_t statusColor = COLOR_GREEN;
            if (deviceStatus == "Low Battery") {
                statusColor = COLOR_RED;
            } else if (deviceStatus == "Playing") {
                statusColor = COLOR_YELLOW;
            }
            statusLabel->setTextColor(statusColor);
        }
    }
    
    /**
     * Handle physical button input.
     */
    void handleButtonInput() {
        // This would integrate with your existing ButtonManager
        // For now, we'll simulate button handling
        
        // Example button mapping:
        // Button A: Navigate left/up
        // Button B: Navigate right/down  
        // Button C: Select/Enter
        
        static int highlightedButton = 0;
        static unsigned long lastButtonTime = 0;
        unsigned long now = millis();
        
        if (now - lastButtonTime < 200) return; // Debounce
        
        // Simulate button press detection
        // In real implementation, this would use buttonManager->isPressed(BUTTON_X)
        
        if (false) { // Replace with actual button detection
            // Navigate between buttons
            unhighlightAllButtons();
            highlightedButton = (highlightedButton + 1) % 3;
            highlightButton(highlightedButton);
            lastButtonTime = now;
        }
        
        if (false) { // Replace with actual button detection
            // Activate highlighted button
            activateButton(highlightedButton);
            lastButtonTime = now;
        }
    }
    
    /**
     * Remove highlighting from all buttons.
     */
    void unhighlightAllButtons() {
        if (ringtonesButton) ringtonesButton->setHighlighted(false);
        if (gameButton) gameButton->setHighlighted(false);
        if (settingsButton) settingsButton->setHighlighted(false);
    }
    
    /**
     * Highlight a specific button.
     */
    void highlightButton(int index) {
        switch (index) {
            case 0:
                if (ringtonesButton) ringtonesButton->setHighlighted(true);
                break;
            case 1:
                if (gameButton) gameButton->setHighlighted(true);
                break;
            case 2:
                if (settingsButton) settingsButton->setHighlighted(true);
                break;
        }
    }
    
    /**
     * Activate a specific button.
     */
    void activateButton(int index) {
        switch (index) {
            case 0:
                onRingtonesClicked();
                break;
            case 1:
                onGameClicked();
                break;
            case 2:
                onSettingsClicked();
                break;
        }
    }
    
    /**
     * Handle ringtones button click.
     */
    void onRingtonesClicked() {
        // Navigate to ringtones screen
        // This would be handled by the UIManager
        Serial.println("Navigating to Ringtones screen");
    }
    
    /**
     * Handle game button click.
     */
    void onGameClicked() {
        // Navigate to game screen
        Serial.println("Navigating to Game screen");
    }
    
    /**
     * Handle settings button click.
     */
    void onSettingsClicked() {
        // Navigate to settings screen
        Serial.println("Navigating to Settings screen");
    }
    
    /**
     * Play the last used ringtone.
     */
    void playLastRingtone() {
        if (ringtonePlayer) {
            // Play a default ringtone for demo
            ringtonePlayer->playRingtoneByName("Mario");
            Serial.println("Playing last ringtone");
        }
    }
    
    /**
     * Start the BeeperHero game.
     */
    void startGame() {
        Serial.println("Starting BeeperHero game");
        // This would transition to the game screen
    }
    
    /**
     * Show device information.
     */
    void showDeviceInfo() {
        Serial.println("Device: Alert TX-1");
        Serial.println("Platform: ESP32-S3");
        Serial.printf("Battery: %.0f%%\n", batteryLevel * 100);
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    }
    
    /**
     * Show power options.
     */
    void showPowerOptions() {
        Serial.println("Power options:");
        Serial.println("- Sleep mode");
        Serial.println("- Deep sleep");
        Serial.println("- Restart");
    }
};

#endif // HOME_SCREEN_H