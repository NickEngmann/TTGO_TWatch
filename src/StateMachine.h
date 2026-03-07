/*
 * StateMachine.h - Main Application State Machine
 * 
 * Handles state transitions between IDLE, RECORDING, STREAMING, and SLEEP
 * Pure logic implementation testable on native builds
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// State Machine States
typedef enum {
    STATE_IDLE = 0,
    STATE_RECORDING,
    STATE_STREAMING,
    STATE_SLEEP,
    STATE_INVALID
} AppState_t;

// State Machine Event Types
typedef enum {
    EVENT_NONE = 0,
    EVENT_BLE_CONNECTED,
    EVENT_BLE_DISCONNECTED,
    EVENT_BUTTON_PRESSED,
    EVENT_BUTTON_RELEASED,
    EVENT_LOW_BATTERY,
    EVENT_HIGH_BATTERY,
    EVENT_START_RECORDING,
    EVENT_STOP_RECORDING,
    EVENT_START_STREAMING,
    EVENT_STOP_STREAMING,
    EVENT_WAKE_UP,
    EVENT_ENTER_SLEEP
} AppEvent_t;

// State Machine Callbacks
typedef void (*StateEnterCallback_t)(AppState_t state);
typedef void (*StateExitCallback_t)(AppState_t state);
typedef void (*StateUpdateCallback_t)(AppState_t state, uint32_t elapsed_ms);

// State Machine Configuration
typedef struct {
    StateEnterCallback_t on_enter;
    StateExitCallback_t on_exit;
    StateUpdateCallback_t on_update;
    uint32_t sleep_threshold_ms;  // Time before entering sleep
    uint32_t recording_buffer_size; // Audio buffer size in bytes
    uint32_t streaming_sample_rate; // Audio sample rate
} StateMachineConfig_t;

// State Machine Context
typedef struct {
    AppState_t current_state;
    AppState_t previous_state;
    uint32_t state_entry_time;
    uint32_t idle_timer;
    bool ble_connected;
    bool recording_active;
    bool streaming_active;
    uint32_t audio_buffer_offset;
    uint32_t audio_buffer_size;
    StateMachineConfig_t config;
} StateMachineContext_t;

// State Machine API
void StateMachine_Init(StateMachineContext_t* ctx, const StateMachineConfig_t* config);
void StateMachine_ProcessEvent(StateMachineContext_t* ctx, AppEvent_t event);
void StateMachine_Update(StateMachineContext_t* ctx, uint32_t elapsed_ms);
AppState_t StateMachine_GetCurrentState(const StateMachineContext_t* ctx);
bool StateMachine_IsRecording(const StateMachineContext_t* ctx);
bool StateMachine_IsStreaming(const StateMachineContext_t* ctx);
bool StateMachine_IsSleeping(const StateMachineContext_t* ctx);
bool StateMachine_IsIdle(const StateMachineContext_t* ctx);
bool StateMachine_IsBleConnected(const StateMachineContext_t* ctx);

// State transition helpers
void StateMachine_SetBleConnected(StateMachineContext_t* ctx, bool connected);
void StateMachine_SetRecordingActive(StateMachineContext_t* ctx, bool active);
void StateMachine_SetStreamingActive(StateMachineContext_t* ctx, bool active);

#ifdef __cplusplus
}
#endif

#endif /* STATE_MACHINE_H */
