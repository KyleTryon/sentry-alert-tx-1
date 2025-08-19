#ifndef RINGTONES_SCREEN_H
#define RINGTONES_SCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../components/MenuContainer.h"

class RingtonesScreen : public Screen {
private:
    MenuContainer* ringtoneMenu;
    int lastPreviewIndex;
    bool autoPreviewEnabled;
    int pendingPreviewIndex;
    unsigned long previewDueAtMs;
    static const unsigned long PREVIEW_DEBOUNCE_MS = 200; // ms

public:
    RingtonesScreen(Adafruit_ST7789* display);
    ~RingtonesScreen() override;

    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    void handleButtonPress(int button) override;

private:
    void buildMenu();
    void previewIndex(int index);
    void persistSelection(int index);
};

#endif // RINGTONES_SCREEN_H


