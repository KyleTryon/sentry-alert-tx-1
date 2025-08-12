#ifndef ICON_DEFINITIONS_H
#define ICON_DEFINITIONS_H

// Include the base Icon structure
#include "../../icons/Icon.h"

// Include all existing icon headers - using relative path from UI directory
#include "../../../AlertTX-1/src/icons/alert.h"
#include "../../../AlertTX-1/src/icons/battery.h"
#include "../../../AlertTX-1/src/icons/battery-1.h"
#include "../../../AlertTX-1/src/icons/battery-2.h"
#include "../../../AlertTX-1/src/icons/battery-charging.h"
#include "../../../AlertTX-1/src/icons/battery-full.h"
#include "../../../AlertTX-1/src/icons/bug.h"
#include "../../../AlertTX-1/src/icons/cellular-signal-0.h"
#include "../../../AlertTX-1/src/icons/cellular-signal-1.h"
#include "../../../AlertTX-1/src/icons/cellular-signal-2.h"
#include "../../../AlertTX-1/src/icons/cellular-signal-3.h"
#include "../../../AlertTX-1/src/icons/cellular-signal-off.h"
#include "../../../AlertTX-1/src/icons/chevron-down.h"
#include "../../../AlertTX-1/src/icons/chevron-left.h"
#include "../../../AlertTX-1/src/icons/chevron-right.h"
#include "../../../AlertTX-1/src/icons/chevron-up.h"
#include "../../../AlertTX-1/src/icons/draft.h"
#include "../../../AlertTX-1/src/icons/mail.h"
#include "../../../AlertTX-1/src/icons/mail-unread.h"
#include "../../../AlertTX-1/src/icons/music.h"
#include "../../../AlertTX-1/src/icons/pause.h"
#include "../../../AlertTX-1/src/icons/play.h"
#include "../../../AlertTX-1/src/icons/volume-minus.h"
#include "../../../AlertTX-1/src/icons/volume-plus.h"
#include "../../../AlertTX-1/src/icons/volume-x.h"

// Convenience definitions for commonly used icons
extern const Icon* const ICON_ALERT;
extern const Icon* const ICON_BATTERY;
extern const Icon* const ICON_BATTERY_LOW;
extern const Icon* const ICON_BATTERY_CHARGING;
extern const Icon* const ICON_BATTERY_FULL;
extern const Icon* const ICON_GAMES;        // Using bug icon as placeholder
extern const Icon* const ICON_SETTINGS;     // Using chevron-right as placeholder
extern const Icon* const ICON_MUSIC;
extern const Icon* const ICON_PLAY;
extern const Icon* const ICON_PAUSE;
extern const Icon* const ICON_MAIL;
extern const Icon* const ICON_WIFI_0;       // No signal
extern const Icon* const ICON_WIFI_1;       // Low signal
extern const Icon* const ICON_WIFI_2;       // Medium signal
extern const Icon* const ICON_WIFI_3;       // Full signal

#endif // ICON_DEFINITIONS_H