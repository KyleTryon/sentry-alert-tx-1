#include "EventSystem.h"
#include <Arduino.h>

void EventSystem::pushEvent(UIEventType type, int id, void* data) {
    // Calculate next position
    int nextTail = (queueTail + 1) % MAX_EVENTS;
    
    // Check if queue is full
    if (nextTail == queueHead) {
        // Queue is full, drop oldest event
        queueHead = (queueHead + 1) % MAX_EVENTS;
    }
    
    // Add new event
    eventQueue[queueTail].type = type;
    eventQueue[queueTail].buttonId = id;
    eventQueue[queueTail].menuIndex = id;
    eventQueue[queueTail].data = data;
    eventQueue[queueTail].timestamp = millis();
    
    queueTail = nextTail;
}

bool EventSystem::popEvent(UIEvent& event) {
    if (isEmpty()) {
        return false;
    }
    
    // Get event from head
    event = eventQueue[queueHead];
    
    // Advance head
    queueHead = (queueHead + 1) % MAX_EVENTS;
    
    return true;
}

void EventSystem::clear() {
    queueHead = 0;
    queueTail = 0;
}

bool EventSystem::isEmpty() const {
    return queueHead == queueTail;
}

int EventSystem::getQueueSize() const {
    if (queueTail >= queueHead) {
        return queueTail - queueHead;
    } else {
        return MAX_EVENTS - queueHead + queueTail;
    }
}
