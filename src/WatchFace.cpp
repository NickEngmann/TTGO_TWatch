#include "WatchFace.h"
#include "TTGO.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

#ifdef NATIVE_BUILD
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#endif

// LVGL color definitions for watch face
#define WATCH_COLOR_BG            0x202020
#define WATCH_COLOR_TIME          0xFFFFFF
#define WATCH_COLOR_DATE          0x888888
#define WATCH_COLOR_BLUETOOTH_OFF 0x555555
#define WATCH_COLOR_BLUETOOTH_ON  0x00FF00
#define WATCH_COLOR_RECORDING_OFF 0x555555
#define WATCH_COLOR_RECORDING_ON  0xFF0000
#define WATCH_COLOR_BATTERY       0x00FF00

#ifdef NATIVE_BUILD
// Native build mock implementations
static bool mock_bluetooth_connected = false;
static bool mock_recording = false;
static int8_t mock_battery_level = -1;
static uint8_t mock_hours = 12;
static uint8_t mock_minutes = 30;
static uint8_t mock_seconds = 0;
static uint8_t mock_day = 1;
static uint8_t mock_month = 1;
static uint16_t mock_year = 2024;

void WatchFaceMock_SetBluetooth(bool connected) { mock_bluetooth_connected = connected; }
void WatchFaceMock_SetRecording(bool recording) { mock_recording = recording; }
void WatchFaceMock_SetBattery(int8_t level) { mock_battery_level = level; }
void WatchFaceMock_SetTime(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y) {
    mock_hours = h; mock_minutes = m; mock_seconds = s;
    mock_day = d; mock_month = mo; mock_year = y;
}
bool WatchFaceMock_GetBluetooth() { return mock_bluetooth_connected; }
bool WatchFaceMock_GetRecording() { return mock_recording; }
int8_t WatchFaceMock_GetBattery() { return mock_battery_level; }
uint8_t WatchFaceMock_GetHours() { return mock_hours; }
uint8_t WatchFaceMock_GetMinutes() { return mock_minutes; }
uint8_t WatchFaceMock_GetSeconds() { return mock_seconds; }
uint8_t WatchFaceMock_GetDay() { return mock_day; }
uint8_t WatchFaceMock_GetMonth() { return mock_month; }
uint16_t WatchFaceMock_GetYear() { return mock_year; }
#endif

static void UpdateBluetoothIcon(WatchFace_t *watchFace) {
    if (!watchFace->context.config.showBluetooth) {
        if (watchFace->bluetoothIcon) lv_obj_add_flag(watchFace->bluetoothIcon, LV_OBJ_FLAG_HIDDEN);
        if (watchFace->bluetoothLabel) lv_obj_add_flag(watchFace->bluetoothLabel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    
    if (watchFace->bluetoothIcon) {
        lv_obj_clear_flag(watchFace->bluetoothIcon, LV_OBJ_FLAG_HIDDEN);
        if (watchFace->context.isBluetoothConnected) {
            lv_obj_set_style_bg_color(watchFace->bluetoothIcon, lv_color_hex(WATCH_COLOR_BLUETOOTH_ON), 0);
            lv_label_set_text(watchFace->bluetoothLabel, "Connected");
            lv_obj_set_style_text_color(watchFace->bluetoothLabel, lv_color_hex(WATCH_COLOR_BLUETOOTH_ON), 0);
        } else {
            lv_obj_set_style_bg_color(watchFace->bluetoothIcon, lv_color_hex(WATCH_COLOR_BLUETOOTH_OFF), 0);
            lv_label_set_text(watchFace->bluetoothLabel, "Disconnected");
            lv_obj_set_style_text_color(watchFace->bluetoothLabel, lv_color_hex(WATCH_COLOR_BLUETOOTH_OFF), 0);
        }
    }
}

static void UpdateRecordingIndicator(WatchFace_t *watchFace) {
    if (!watchFace->context.config.showRecording) {
        if (watchFace->recordingIcon) lv_obj_add_flag(watchFace->recordingIcon, LV_OBJ_FLAG_HIDDEN);
        if (watchFace->recordingLabel) lv_obj_add_flag(watchFace->recordingLabel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    
    if (watchFace->recordingIcon) {
        lv_obj_clear_flag(watchFace->recordingIcon, LV_OBJ_FLAG_HIDDEN);
        if (watchFace->context.isRecording) {
            lv_obj_set_style_bg_color(watchFace->recordingIcon, lv_color_hex(WATCH_COLOR_RECORDING_ON), 0);
            lv_label_set_text(watchFace->recordingLabel, "REC");
            lv_obj_set_style_text_color(watchFace->recordingLabel, lv_color_hex(WATCH_COLOR_RECORDING_ON), 0);
            lv_obj_set_style_anim_speed(watchFace->recordingIcon, 100, 0);
        } else {
            lv_obj_set_style_bg_color(watchFace->recordingIcon, lv_color_hex(WATCH_COLOR_RECORDING_OFF), 0);
            lv_label_set_text(watchFace->recordingLabel, "");
            lv_obj_set_style_text_color(watchFace->recordingLabel, lv_color_hex(WATCH_COLOR_RECORDING_OFF), 0);
            lv_obj_set_style_anim_speed(watchFace->recordingIcon, 0, 0);
        }
    }
}

static void UpdateBatteryIndicator(WatchFace_t *watchFace) {
    if (!watchFace->context.config.showBattery) {
        if (watchFace->batteryIcon) lv_obj_add_flag(watchFace->batteryIcon, LV_OBJ_FLAG_HIDDEN);
        if (watchFace->batteryLabel) lv_obj_add_flag(watchFace->batteryLabel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    
    if (watchFace->batteryIcon && watchFace->batteryLabel) {
        lv_obj_clear_flag(watchFace->batteryIcon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(watchFace->batteryLabel, LV_OBJ_FLAG_HIDDEN);
        
        if (watchFace->context.batteryLevel >= 0) {
            char buffer[8];
            snprintf(buffer, sizeof(buffer), "%d%%", watchFace->context.batteryLevel);
            lv_label_set_text(watchFace->batteryLabel, buffer);
            
            // Color based on battery level
            int color;
            if (watchFace->context.batteryLevel > 50) {
                color = WATCH_COLOR_BATTERY;
            } else if (watchFace->context.batteryLevel > 20) {
                color = 0xFFFF00;
            } else {
                color = 0xFF0000;
            }
            lv_obj_set_style_bg_color(watchFace->batteryIcon, lv_color_hex(color), 0);
            lv_obj_set_style_text_color(watchFace->batteryLabel, lv_color_hex(color), 0);
        } else {
            lv_label_set_text(watchFace->batteryLabel, "--");
            lv_obj_set_style_bg_color(watchFace->batteryIcon, lv_color_hex(0x555555), 0);
        }
    }
}

static void UpdateTimeDisplay(WatchFace_t *watchFace) {
    if (watchFace->timeLabel) {
        char timeBuffer[16];
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d",
                 watchFace->context.hours,
                 watchFace->context.minutes,
                 watchFace->context.seconds);
        lv_label_set_text(watchFace->timeLabel, timeBuffer);
    }
}

static void UpdateDateDisplay(WatchFace_t *watchFace) {
    if (watchFace->dateLabel && watchFace->context.config.showDate) {
        char dateBuffer[20];
        snprintf(dateBuffer, sizeof(dateBuffer), "%02d/%02d/%04d",
                 watchFace->context.day,
                 watchFace->context.month,
                 watchFace->context.year);
        lv_label_set_text(watchFace->dateLabel, dateBuffer);
    }
}

void WatchFace_Init(WatchFace_t *watchFace) {
    if (!watchFace) return;
    
    // Initialize context with defaults
    memset(&watchFace->context, 0, sizeof(WatchFaceContext_t));
    watchFace->context.config.showSeconds = true;
    watchFace->context.config.showDate = true;
    watchFace->context.config.showBattery = true;
    watchFace->context.config.showBluetooth = true;
    watchFace->context.config.showRecording = true;
    watchFace->context.batteryLevel = -1;
    watchFace->context.isBluetoothConnected = false;
    watchFace->context.isRecording = false;
    
    // Initialize LVGL objects (will be created by LVGL setup)
    watchFace->timeLabel = NULL;
    watchFace->dateLabel = NULL;
    watchFace->bluetoothIcon = NULL;
    watchFace->bluetoothLabel = NULL;
    watchFace->recordingIcon = NULL;
    watchFace->recordingLabel = NULL;
    watchFace->batteryIcon = NULL;
    watchFace->batteryLabel = NULL;
    
    // Set initial state
    watchFace->context.state = WATCH_STATE_NORMAL;
}

void WatchFace_Deinit(WatchFace_t *watchFace) {
    if (!watchFace) return;
    
    // Clear LVGL objects
    if (watchFace->timeLabel) {
        lv_obj_del(watchFace->timeLabel);
        watchFace->timeLabel = NULL;
    }
    if (watchFace->dateLabel) {
        lv_obj_del(watchFace->dateLabel);
        watchFace->dateLabel = NULL;
    }
    if (watchFace->bluetoothIcon) {
        lv_obj_del(watchFace->bluetoothIcon);
        watchFace->bluetoothIcon = NULL;
    }
    if (watchFace->bluetoothLabel) {
        lv_obj_del(watchFace->bluetoothLabel);
        watchFace->bluetoothLabel = NULL;
    }
    if (watchFace->recordingIcon) {
        lv_obj_del(watchFace->recordingIcon);
        watchFace->recordingIcon = NULL;
    }
    if (watchFace->recordingLabel) {
        lv_obj_del(watchFace->recordingLabel);
        watchFace->recordingLabel = NULL;
    }
    if (watchFace->batteryIcon) {
        lv_obj_del(watchFace->batteryIcon);
        watchFace->batteryIcon = NULL;
    }
    if (watchFace->batteryLabel) {
        lv_obj_del(watchFace->batteryLabel);
        watchFace->batteryLabel = NULL;
    }
    
    watchFace->context.state = WATCH_STATE_NORMAL;
}

void WatchFace_UpdateTime(WatchFace_t *watchFace, uint8_t hours, uint8_t minutes, uint8_t seconds,
                          uint8_t day, uint8_t month, uint16_t year) {
    if (!watchFace) return;
    
    watchFace->context.hours = hours;
    watchFace->context.minutes = minutes;
    watchFace->context.seconds = seconds;
    watchFace->context.day = day;
    watchFace->context.month = month;
    watchFace->context.year = year;
    
    // Update state based on recording
    if (watchFace->context.isRecording) {
        watchFace->context.state = WATCH_STATE_RECORDING;
    } else if (watchFace->context.isBluetoothConnected) {
        watchFace->context.state = WATCH_STATE_BLE_CONNECTED;
    } else {
        watchFace->context.state = WATCH_STATE_NORMAL;
    }
    
    UpdateTimeDisplay(watchFace);
    UpdateDateDisplay(watchFace);
}

void WatchFace_UpdateBluetooth(WatchFace_t *watchFace, bool isConnected) {
    if (!watchFace) return;
    
    watchFace->context.isBluetoothConnected = isConnected;
    
    // Update state
    if (watchFace->context.isRecording) {
        watchFace->context.state = WATCH_STATE_RECORDING;
    } else if (isConnected) {
        watchFace->context.state = WATCH_STATE_BLE_CONNECTED;
    } else {
        watchFace->context.state = WATCH_STATE_BLE_DISCONNECTED;
    }
    
    UpdateBluetoothIcon(watchFace);
}

void WatchFace_UpdateRecording(WatchFace_t *watchFace, bool isRecording) {
    if (!watchFace) return;
    
    watchFace->context.isRecording = isRecording;
    
    // Update state
    if (isRecording) {
        watchFace->context.state = WATCH_STATE_RECORDING;
    } else if (watchFace->context.isBluetoothConnected) {
        watchFace->context.state = WATCH_STATE_BLE_CONNECTED;
    } else {
        watchFace->context.state = WATCH_STATE_NORMAL;
    }
    
    UpdateRecordingIndicator(watchFace);
}

void WatchFace_UpdateBattery(WatchFace_t *watchFace, int8_t level) {
    if (!watchFace) return;
    
    watchFace->context.batteryLevel = level;
    UpdateBatteryIndicator(watchFace);
}

void WatchFace_SetConfig(WatchFace_t *watchFace, const WatchFaceConfig_t *config) {
    if (!watchFace || !config) return;
    
    watchFace->context.config.showSeconds = config->showSeconds;
    watchFace->context.config.showDate = config->showDate;
    watchFace->context.config.showBattery = config->showBattery;
    watchFace->context.config.showBluetooth = config->showBluetooth;
    watchFace->context.config.showRecording = config->showRecording;
    
    // Re-update all indicators with new config
    UpdateBluetoothIcon(watchFace);
    UpdateRecordingIndicator(watchFace);
    UpdateBatteryIndicator(watchFace);
}

WatchState_t WatchFace_GetState(const WatchFace_t *watchFace) {
    if (!watchFace) return WATCH_STATE_NORMAL;
    return watchFace->context.state;
}

void WatchFace_Draw(WatchFace_t *watchFace) {
    if (!watchFace) return;
    
    // Trigger LVGL refresh
    lv_obj_invalidate(lv_scr_act());
}
