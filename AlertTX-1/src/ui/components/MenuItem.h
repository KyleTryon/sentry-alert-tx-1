#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include "../icons/IconDefinitions.h"

// Forward declaration for callback function
typedef void (*MenuActionCallback)();

struct MenuItem {
    const char* label;        // Menu item text
    const Icon* icon;         // Icon to display (16x16)
    int id;                   // Unique identifier
    MenuActionCallback action; // Callback function when selected
    bool enabled;             // Whether item is selectable
    void* userData;           // Additional data if needed
    
    // Constructor for convenience
    MenuItem() : label(nullptr), icon(nullptr), id(0), action(nullptr), enabled(true), userData(nullptr) {}
    
    MenuItem(const char* lbl, const Icon* icn, int itemId, MenuActionCallback callback = nullptr) 
        : label(lbl), icon(icn), id(itemId), action(callback), enabled(true), userData(nullptr) {}
};

#endif // MENU_ITEM_H
