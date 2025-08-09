#ifndef RINGTONES_SCREEN_H
#define RINGTONES_SCREEN_H

#include "../core/Screen.h"
#include "../components/Label.h"
#include "../components/Button.h"
#include "../components/Menu.h"
#include "../../ringtones/RingtonePlayer.h"
#include "../../ringtones/ringtone_data.h"

/**
 * Ringtones screen for the Alert TX-1 device.
 * Displays available ringtones and allows playback and selection.
 */
class RingtonesScreen : public Screen {
private:
    Label* titleLabel = nullptr;
    Label* statusLabel = nullptr;
    Menu* ringtoneMenu = nullptr;
    Button* playButton = nullptr;
    Button* stopButton = nullptr;
    Button* backButton = nullptr;
    
    // Hardware reference
    RingtonePlayer* ringtonePlayer = nullptr;
    
    // State
    String currentStatus = "Select a ringtone";
    int selectedRingtone = 0;
    bool isPlaying = false;

public:
    RingtonesScreen(RingtonePlayer* player = nullptr) : ringtonePlayer(player) {}

    /**
     * Build the ringtones screen components.
     */
    void build() override {
        // Title
        titleLabel = addComponent<Label>(10, 5, 220, 15, "Ringtones", COLOR_WHITE);
        titleLabel->setTextSize(1);
        titleLabel->setCenterAlign(true);
        titleLabel->setBackgroundColor(COLOR_BLUE, true);
        
        // Status label
        statusLabel = addComponent<Label>(10, 25, 220, 12, currentStatus, COLOR_GREEN);
        statusLabel->setTextSize(1);
        statusLabel->setCenterAlign(true);
        
        // Ringtone menu
        ringtoneMenu = addComponent<Menu>(10, 40, 220, 65);
        ringtoneMenu->setItemHeight(13);
        ringtoneMenu->setColors(COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK);
        
        // Populate ringtone menu from generated data
        populateRingtoneMenu();
        
        // Control buttons
        playButton = addComponent<Button>(10, 110, 50, 20, "Play", [this]() {
            playSelectedRingtone();
        });
        
        stopButton = addComponent<Button>(70, 110, 50, 20, "Stop", [this]() {
            stopPlayback();
        });
        
        backButton = addComponent<Button>(180, 110, 50, 20, "Back", [this]() {
            goBack();
        });
        
        // Set initial button states
        playButton->setHighlighted(true);
        stopButton->setEnabled(false);
    }
    
    /**
     * Update the ringtones screen state.
     */
    void update() override {
        Screen::update();
        
        // Update playing status
        bool wasPlaying = isPlaying;
        if (ringtonePlayer) {
            isPlaying = ringtonePlayer->isPlaying();
        }
        
        // Update UI if playback status changed
        if (wasPlaying != isPlaying) {
            updatePlaybackStatus();
        }
        
        // Update selected ringtone
        if (ringtoneMenu) {
            int newSelected = ringtoneMenu->getSelectedIndex();
            if (newSelected != selectedRingtone) {
                selectedRingtone = newSelected;
                updateSelectedRingtone();
            }
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
     * Populate the ringtone menu with available ringtones.
     */
    void populateRingtoneMenu() {
        if (!ringtoneMenu || !ringtonePlayer) return;
        
        // Get ringtone count from the player
        int count = ringtonePlayer->getRingtoneCount();
        
        for (int i = 0; i < count; i++) {
            const char* name = ringtonePlayer->getRingtoneName(i);
            if (name) {
                ringtoneMenu->addItem(String(name), [this, i]() {
                    selectRingtone(i);
                });
            }
        }
        
        // If no ringtones are available, add a message
        if (count == 0) {
            ringtoneMenu->addItem("No ringtones available", nullptr, false);
        }
    }
    
    /**
     * Update the current status display.
     */
    void updateStatus() {
        String newStatus = "Select a ringtone";
        
        if (isPlaying) {
            String ringtoneName = ringtoneMenu ? ringtoneMenu->getSelectedText() : "Unknown";
            newStatus = "Playing: " + ringtoneName;
        } else if (selectedRingtone >= 0) {
            String ringtoneName = ringtoneMenu ? ringtoneMenu->getSelectedText() : "Unknown";
            newStatus = "Selected: " + ringtoneName;
        }
        
        if (statusLabel && currentStatus != newStatus) {
            currentStatus = newStatus;
            statusLabel->setText(currentStatus);
        }
    }
    
    /**
     * Update playback status and button states.
     */
    void updatePlaybackStatus() {
        if (playButton && stopButton) {
            if (isPlaying) {
                playButton->setEnabled(false);
                stopButton->setEnabled(true);
                stopButton->setHighlighted(true);
                playButton->setHighlighted(false);
            } else {
                playButton->setEnabled(true);
                stopButton->setEnabled(false);
                playButton->setHighlighted(true);
                stopButton->setHighlighted(false);
            }
        }
        
        updateStatus();
    }
    
    /**
     * Update selected ringtone display.
     */
    void updateSelectedRingtone() {
        updateStatus();
    }
    
    /**
     * Select a ringtone by index.
     */
    void selectRingtone(int index) {
        if (ringtoneMenu) {
            ringtoneMenu->setSelectedIndex(index);
            selectedRingtone = index;
            updateSelectedRingtone();
        }
    }
    
    /**
     * Play the currently selected ringtone.
     */
    void playSelectedRingtone() {
        if (!ringtonePlayer || isPlaying) return;
        
        if (selectedRingtone >= 0 && selectedRingtone < ringtonePlayer->getRingtoneCount()) {
            Serial.printf("Playing ringtone %d\n", selectedRingtone);
            ringtonePlayer->playRingtoneByIndex(selectedRingtone);
            isPlaying = true;
            updatePlaybackStatus();
        }
    }
    
    /**
     * Stop current playback.
     */
    void stopPlayback() {
        if (!ringtonePlayer || !isPlaying) return;
        
        Serial.println("Stopping ringtone playback");
        ringtonePlayer->stop();
        isPlaying = false;
        updatePlaybackStatus();
    }
    
    /**
     * Go back to the previous screen.
     */
    void goBack() {
        Serial.println("Returning to home screen");
        // This would be handled by the UIManager to navigate back
        // For now, just stop any playing ringtone
        if (isPlaying) {
            stopPlayback();
        }
    }
    
    /**
     * Handle button navigation events.
     */
    void handleEvent(int eventType, int eventData = 0) override {
        Screen::handleEvent(eventType, eventData);
        
        // Additional event handling for ringtone screen
        switch (eventType) {
            case 10: // Play button pressed
                playSelectedRingtone();
                break;
                
            case 11: // Stop button pressed
                stopPlayback();
                break;
                
            case 12: // Back button pressed
                goBack();
                break;
                
            case 13: // Menu navigation
                if (ringtoneMenu) {
                    if (eventData == 1) { // Up
                        ringtoneMenu->selectPrevious();
                    } else if (eventData == 2) { // Down
                        ringtoneMenu->selectNext();
                    } else if (eventData == 3) { // Select
                        ringtoneMenu->activateSelected();
                    }
                }
                break;
        }
    }
};

#endif // RINGTONES_SCREEN_H