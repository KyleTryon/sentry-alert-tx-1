#ifndef INPUTROUTER_H
#define INPUTROUTER_H

#include <Arduino.h>
#include "ScreenManager.h"
#include "../../hardware/ButtonManager.h"
#include "../../power/PowerManager.h"

class InputRouter {
public:
	InputRouter(ScreenManager* screenManager, ButtonManager* buttonManager)
		: manager(screenManager), buttons(buttonManager) {}

	void update() {
		if (!manager || !buttons) return;
		buttons->update();

		unsigned long now = millis();
		// Back via long-press (any button)
		bool longPress = buttons->isLongPressed(ButtonManager::BUTTON_A) ||
		               buttons->isLongPressed(ButtonManager::BUTTON_B) ||
		               buttons->isLongPressed(ButtonManager::BUTTON_C);
		if (longPress) {
			if (now - lastBackMs > BACK_DEBOUNCE_MS) {
				manager->popScreen();
				lastBackMs = now;
				suppressSelectUntilRelease = true;
			}
		}

		// Clear suppression once all released
		if (!buttons->isPressed(ButtonManager::BUTTON_A) &&
		    !buttons->isPressed(ButtonManager::BUTTON_B) &&
		    !buttons->isPressed(ButtonManager::BUTTON_C)) {
			suppressSelectUntilRelease = false;
		}

		// Route discrete presses to current screen
		bool anyActivity = false;
		if (buttons->wasPressed(ButtonManager::BUTTON_A)) {
			repeatStartMs[ButtonManager::BUTTON_A] = now;
			lastRepeatMs[ButtonManager::BUTTON_A] = now;
			manager->handleButtonPress(ButtonInput::BUTTON_A);
			anyActivity = true;
		}
		if (buttons->wasPressed(ButtonManager::BUTTON_B)) {
			repeatStartMs[ButtonManager::BUTTON_B] = now;
			lastRepeatMs[ButtonManager::BUTTON_B] = now;
			manager->handleButtonPress(ButtonInput::BUTTON_B);
			anyActivity = true;
		}
		// Only treat Select as click if it was a short click (not long press)
		if (buttons->wasReleased(ButtonManager::BUTTON_C) && buttons->wasShortClick(ButtonManager::BUTTON_C)) {
			if (!suppressSelectUntilRelease) {
				manager->handleButtonPress(ButtonInput::BUTTON_C);
				anyActivity = true;
			}
		}
		// Count releases of A/B as activity as well
		if (buttons->wasReleased(ButtonManager::BUTTON_A) || buttons->wasReleased(ButtonManager::BUTTON_B)) {
			anyActivity = true;
		}

		// Auto-repeat for held A/B
		autoRepeat(now, ButtonManager::BUTTON_A, ButtonInput::BUTTON_A);
		autoRepeat(now, ButtonManager::BUTTON_B, ButtonInput::BUTTON_B);

		// Notify power manager if any user interaction occurred
		if (anyActivity) {
			PowerManager::notifyActivity();
		}
	}

private:
	ScreenManager* manager;
	ButtonManager* buttons;
	unsigned long lastBackMs = 0;
	bool suppressSelectUntilRelease = false;
	static const unsigned long BACK_DEBOUNCE_MS = 400;

	// Held repeat configuration
    static const unsigned long REPEAT_START_DELAY_MS = 250; // delay before repeating
    static const unsigned long REPEAT_RATE_MS = 70;         // repeat interval
	unsigned long repeatStartMs[3] = {0,0,0};
	unsigned long lastRepeatMs[3] = {0,0,0};

	void autoRepeat(unsigned long now, int buttonIndex, int routedButton) {
		if (buttons->isPressed(buttonIndex)) {
			if (repeatStartMs[buttonIndex] == 0) {
				repeatStartMs[buttonIndex] = now;
				lastRepeatMs[buttonIndex] = now;
				return;
			}
			if ((now - repeatStartMs[buttonIndex]) >= REPEAT_START_DELAY_MS &&
			    (now - lastRepeatMs[buttonIndex]) >= REPEAT_RATE_MS) {
				manager->handleButtonPress(routedButton);
				lastRepeatMs[buttonIndex] = now;
			}
		} else {
			repeatStartMs[buttonIndex] = 0;
			lastRepeatMs[buttonIndex] = 0;
		}
	}
};

#endif // INPUTROUTER_H

