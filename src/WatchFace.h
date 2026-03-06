#ifndef WATCH_FACE_H
#define WATCH_FACE_H

#include "lvgl.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Watch face states
typedef enum {
    WATCH_STATE_NORMAL = 0,
    WATCH_STATE_RECORDING,
    WATCH_STATE_BLE_CONNECTED,
    WATCH_STATE_BLE_DISCONNECTED
} WatchState_t;

// Watch face configuration
typedef struct {
    bool showSeconds;
    bool showDate;
    bool showBattery;
    bool showBluetooth;
    bool showRecording;
} WatchFaceConfig_t;

// Watch face context for state management
typedef struct {
    WatchState_t state;
    WatchFaceConfig_t config;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    bool isRecording;
    bool isBluetoothConnected;
    int8_t batteryLevel;
} WatchFaceContext_t;

// Watch Face API
typedef struct WatchFaceWatchFace_t {
    // LVGL objects
    lv_obj_t *timeLabel;
    lv_obj_t *dateLabel;
    lv_obj_t *bluetoothIcon;
    lv_obj_t *bluetoothLabel;
    lv_obj_t *recordingIcon;
    lv_obj_t *recordingLabel;
    lv_obj_t *batteryIcon;
    lv_obj_t *batteryLabel;
    
    // Context
    WatchFaceContext_t context;
} WatchFace_t;

// Initialize the watch face
void WatchFace_Init(WatchFace_t *watchFace);

// Deinitialize the watch face
void WatchFace_Deinit(WatchFace_t *watchFace);

// Update the watch face with new time data
void WatchFace_UpdateTime(WatchFace_t *watchFace, uint8_t hours, uint8_t minutes, uint8_t seconds,
                          uint8_t day, uint8_t month, uint16_t year);

// Update Bluetooth connection status
void WatchFace_UpdateBluetooth(WatchFace_t *watchFace, bool isConnected);

// Update recording status
void WatchFace_UpdateRecording(WatchFace_t *watchFace, bool isRecording);

// Update battery level
void WatchFace_UpdateBattery(WatchFace_t *watchFace, int8_t level);

// Set configuration
void WatchFace_SetConfig(WatchFace_t *watchFace, const WatchFaceConfig_t *config);

// Get current state
WatchState_t WatchFace_GetState(const WatchFace_t *watchFace);

// Draw the watch face
void WatchFace_Draw(WatchFace_t *watchFace);

#ifdef __cplusplus
}
#endif

#endif /* WATCH_FACE_H */
