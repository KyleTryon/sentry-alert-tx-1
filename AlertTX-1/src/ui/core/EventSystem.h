#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <stdint.h>

enum class UIEventType {
    BUTTON_PRESS,
    BUTTON_RELEASE,
    BUTTON_LONG_PRESS,
    MENU_SELECT,
    MENU_NAVIGATE,
    SCREEN_CHANGE,
    RENDER_REQUEST,
    THEME_CHANGE
};

enum ButtonAction {
    ACTION_NONE,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_SELECT,
    ACTION_BACK,
    ACTION_LONG_PRESS
};

struct UIEvent {
    UIEventType type;
    int buttonId;           // For button events
    int menuIndex;          // For menu events
    void* data;             // Additional event data
    unsigned long timestamp;
};

class EventSystem {
private:
    static const int MAX_EVENTS = 16;
    UIEvent eventQueue[MAX_EVENTS];
    int queueHead = 0;
    int queueTail = 0;
    
public:
    void pushEvent(UIEventType type, int id = 0, void* data = nullptr);
    bool popEvent(UIEvent& event);
    void clear();
    bool isEmpty() const;
    int getQueueSize() const;
};

#endif // EVENT_SYSTEM_H
