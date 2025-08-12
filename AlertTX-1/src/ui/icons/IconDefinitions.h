#ifndef ICON_DEFINITIONS_H
#define ICON_DEFINITIONS_H

// Include the base Icon structure
#include "../../icons/Icon.h"

// Include all existing icon headers - using correct relative paths
#include "../../icons/alert.h"
#include "../../icons/battery.h"
#include "../../icons/battery-1.h"
#include "../../icons/battery-2.h"
#include "../../icons/battery-charging.h"
#include "../../icons/battery-full.h"
#include "../../icons/bug.h"
#include "../../icons/cellular-signal-0.h"
#include "../../icons/cellular-signal-1.h"
#include "../../icons/cellular-signal-2.h"
#include "../../icons/cellular-signal-3.h"
#include "../../icons/cellular-signal-off.h"
#include "../../icons/chevron-down.h"
#include "../../icons/chevron-left.h"
#include "../../icons/chevron-right.h"
#include "../../icons/chevron-up.h"
#include "../../icons/draft.h"
#include "../../icons/mail.h"
#include "../../icons/mail-unread.h"
#include "../../icons/music.h"
#include "../../icons/pause-icon.h"
#include "../../icons/play.h"
#include "../../icons/volume-minus.h"
#include "../../icons/volume-plus.h"
#include "../../icons/volume-x.h"

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
