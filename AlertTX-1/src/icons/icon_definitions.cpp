#include <Arduino.h>
#include "Icon.h"

// Include all icon headers
#include "alert_16.h"
#include "battery-1_16.h"
#include "battery-2_16.h"
#include "battery-charging_16.h"
#include "battery-full_16.h"
#include "battery_16.h"
#include "bug_16.h"
#include "cellular-signal-0_16.h"
#include "cellular-signal-1_16.h"
#include "cellular-signal-2_16.h"
#include "cellular-signal-3_16.h"
#include "cellular-signal-off_16.h"
#include "chevron-down_16.h"
#include "chevron-left_16.h"
#include "chevron-right_16.h"
#include "chevron-up_16.h"
#include "draft_16.h"
#include "mail-unread_16.h"
#include "mail_16.h"
#include "music_16.h"
#include "pause-icon_16.h"
#include "play_16.h"
#include "sentry_logo_32.h"
#include "volume-minus_16.h"
#include "volume-plus_16.h"
#include "volume-x_16.h"

// Define all icon data arrays and structs

// alert (from alert_16.h)
const uint16_t* alert_data_ptr = alert_data;
const Icon alert = { 0, 0, ALERT_WIDTH, ALERT_HEIGHT, alert_data };

// battery_1 (from battery-1_16.h)
const uint16_t* battery_1_data_ptr = battery_1_data;
const Icon battery_1 = { 0, 0, BATTERY_1_WIDTH, BATTERY_1_HEIGHT, battery_1_data };

// battery_2 (from battery-2_16.h)
const uint16_t* battery_2_data_ptr = battery_2_data;
const Icon battery_2 = { 0, 0, BATTERY_2_WIDTH, BATTERY_2_HEIGHT, battery_2_data };

// battery_charging (from battery-charging_16.h)
const uint16_t* battery_charging_data_ptr = battery_charging_data;
const Icon battery_charging = { 0, 0, BATTERY_CHARGING_WIDTH, BATTERY_CHARGING_HEIGHT, battery_charging_data };

// battery_full (from battery-full_16.h)
const uint16_t* battery_full_data_ptr = battery_full_data;
const Icon battery_full = { 0, 0, BATTERY_FULL_WIDTH, BATTERY_FULL_HEIGHT, battery_full_data };

// battery (from battery_16.h)
const uint16_t* battery_data_ptr = battery_data;
const Icon battery = { 0, 0, BATTERY_WIDTH, BATTERY_HEIGHT, battery_data };

// bug (from bug_16.h)
const uint16_t* bug_data_ptr = bug_data;
const Icon bug = { 0, 0, BUG_WIDTH, BUG_HEIGHT, bug_data };

// cellular_signal_0 (from cellular-signal-0_16.h)
const uint16_t* cellular_signal_0_data_ptr = cellular_signal_0_data;
const Icon cellular_signal_0 = { 0, 0, CELLULAR_SIGNAL_0_WIDTH, CELLULAR_SIGNAL_0_HEIGHT, cellular_signal_0_data };

// cellular_signal_1 (from cellular-signal-1_16.h)
const uint16_t* cellular_signal_1_data_ptr = cellular_signal_1_data;
const Icon cellular_signal_1 = { 0, 0, CELLULAR_SIGNAL_1_WIDTH, CELLULAR_SIGNAL_1_HEIGHT, cellular_signal_1_data };

// cellular_signal_2 (from cellular-signal-2_16.h)
const uint16_t* cellular_signal_2_data_ptr = cellular_signal_2_data;
const Icon cellular_signal_2 = { 0, 0, CELLULAR_SIGNAL_2_WIDTH, CELLULAR_SIGNAL_2_HEIGHT, cellular_signal_2_data };

// cellular_signal_3 (from cellular-signal-3_16.h)
const uint16_t* cellular_signal_3_data_ptr = cellular_signal_3_data;
const Icon cellular_signal_3 = { 0, 0, CELLULAR_SIGNAL_3_WIDTH, CELLULAR_SIGNAL_3_HEIGHT, cellular_signal_3_data };

// cellular_signal_off (from cellular-signal-off_16.h)
const uint16_t* cellular_signal_off_data_ptr = cellular_signal_off_data;
const Icon cellular_signal_off = { 0, 0, CELLULAR_SIGNAL_OFF_WIDTH, CELLULAR_SIGNAL_OFF_HEIGHT, cellular_signal_off_data };

// chevron_down (from chevron-down_16.h)
const uint16_t* chevron_down_data_ptr = chevron_down_data;
const Icon chevron_down = { 0, 0, CHEVRON_DOWN_WIDTH, CHEVRON_DOWN_HEIGHT, chevron_down_data };

// chevron_left (from chevron-left_16.h)
const uint16_t* chevron_left_data_ptr = chevron_left_data;
const Icon chevron_left = { 0, 0, CHEVRON_LEFT_WIDTH, CHEVRON_LEFT_HEIGHT, chevron_left_data };

// chevron_right (from chevron-right_16.h)
const uint16_t* chevron_right_data_ptr = chevron_right_data;
const Icon chevron_right = { 0, 0, CHEVRON_RIGHT_WIDTH, CHEVRON_RIGHT_HEIGHT, chevron_right_data };

// chevron_up (from chevron-up_16.h)
const uint16_t* chevron_up_data_ptr = chevron_up_data;
const Icon chevron_up = { 0, 0, CHEVRON_UP_WIDTH, CHEVRON_UP_HEIGHT, chevron_up_data };

// draft (from draft_16.h)
const uint16_t* draft_data_ptr = draft_data;
const Icon draft = { 0, 0, DRAFT_WIDTH, DRAFT_HEIGHT, draft_data };

// mail_unread (from mail-unread_16.h)
const uint16_t* mail_unread_data_ptr = mail_unread_data;
const Icon mail_unread = { 0, 0, MAIL_UNREAD_WIDTH, MAIL_UNREAD_HEIGHT, mail_unread_data };

// mail (from mail_16.h)
const uint16_t* mail_data_ptr = mail_data;
const Icon mail = { 0, 0, MAIL_WIDTH, MAIL_HEIGHT, mail_data };

// music (from music_16.h)
const uint16_t* music_data_ptr = music_data;
const Icon music = { 0, 0, MUSIC_WIDTH, MUSIC_HEIGHT, music_data };

// pause_icon (from pause-icon_16.h)
const uint16_t* pause_icon_data_ptr = pause_icon_data;
const Icon pause_icon = { 0, 0, PAUSE_ICON_WIDTH, PAUSE_ICON_HEIGHT, pause_icon_data };

// play (from play_16.h)
const uint16_t* play_data_ptr = play_data;
const Icon play = { 0, 0, PLAY_WIDTH, PLAY_HEIGHT, play_data };

// sentry_logo (from sentry_logo_32.h)
const uint16_t* sentry_logo_data_ptr = sentry_logo_data;
const Icon sentry_logo = { 0, 0, SENTRY_LOGO_WIDTH, SENTRY_LOGO_HEIGHT, sentry_logo_data };

// volume_minus (from volume-minus_16.h)
const uint16_t* volume_minus_data_ptr = volume_minus_data;
const Icon volume_minus = { 0, 0, VOLUME_MINUS_WIDTH, VOLUME_MINUS_HEIGHT, volume_minus_data };

// volume_plus (from volume-plus_16.h)
const uint16_t* volume_plus_data_ptr = volume_plus_data;
const Icon volume_plus = { 0, 0, VOLUME_PLUS_WIDTH, VOLUME_PLUS_HEIGHT, volume_plus_data };

// volume_x (from volume-x_16.h)
const uint16_t* volume_x_data_ptr = volume_x_data;
const Icon volume_x = { 0, 0, VOLUME_X_WIDTH, VOLUME_X_HEIGHT, volume_x_data };
