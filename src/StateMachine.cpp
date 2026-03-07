/*
 * StateMachine.cpp - Main Application State Machine Implementation
 * 
 * Handles state transitions between IDLE, RECORDING, STREAMING, and SLEEP
 * Pure logic implementation testable on native builds
 */

#include "StateMachine.h"
#include <string.h>

// Internal state transition function
static void StateMachine_Transition(StateMachineContext_t* ctx, AppState_t new_state);

// State-specific handlers
static void StateMachine_HandleIdle(StateMachineContext_t* ctx, AppEvent_t event);
static void StateMachine_HandleRecording(StateMachineContext_t* ctx, AppEvent_t event);
static void StateMachine_HandleStreaming(StateMachineContext_t* ctx, AppEvent_t event);
static void StateMachine_HandleSleep(StateMachineContext_t* ctx, AppEvent_t event);

void StateMachine_Init(StateMachineContext_t* ctx, const StateMachineConfig_t* config) {
    if (!ctx || !config) return;
    
    memset(ctx, 0, sizeof(StateMachineContext_t));
    
    ctx->current_state = STATE_IDLE;
    ctx->previous_state = STATE_IDLE;
    ctx->state_entry_time = 0;
    ctx->idle_timer = 0;
    ctx->ble_connected = false;
    ctx->recording_active = false;
    ctx->streaming_active = false;
    ctx->audio_buffer_offset = 0;
    ctx->audio_buffer_size = 0;
    
    // Copy configuration
    memcpy(&ctx->config, config, sizeof(StateMachineConfig_t));
}

void StateMachine_ProcessEvent(StateMachineContext_t* ctx, AppEvent_t event) {
    if (!ctx) return;
    
    switch (ctx->current_state) {
        case STATE_IDLE:
            StateMachine_HandleIdle(ctx, event);
            break;
        case STATE_RECORDING:
            StateMachine_HandleRecording(ctx, event);
            break;
        case STATE_STREAMING:
            StateMachine_HandleStreaming(ctx, event);
            break;
        case STATE_SLEEP:
            StateMachine_HandleSleep(ctx, event);
            break;
        default:
            // Invalid state, reset to idle
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
    }
}

void StateMachine_Update(StateMachineContext_t* ctx, uint32_t elapsed_ms) {
    if (!ctx) return;
    
    // Update idle timer for sleep transition
    if (ctx->current_state == STATE_IDLE) {
        ctx->idle_timer += elapsed_ms;
        
        // Check if we should enter sleep mode
        if (ctx->idle_timer >= ctx->config.sleep_threshold_ms) {
            StateMachine_Transition(ctx, STATE_SLEEP);
        }
    } else {
        // Reset idle timer when not in idle state
        ctx->idle_timer = 0;
    }
    
    // Call state update callback if configured
    if (ctx->config.on_update) {
        ctx->config.on_update(ctx->current_state, elapsed_ms);
    }
}

AppState_t StateMachine_GetCurrentState(const StateMachineContext_t* ctx) {
    if (!ctx) return STATE_INVALID;
    return ctx->current_state;
}

bool StateMachine_IsRecording(const StateMachineContext_t* ctx) {
    if (!ctx) return false;
    return ctx->current_state == STATE_RECORDING || ctx->recording_active;
}

bool StateMachine_IsStreaming(const StateMachineContext_t* ctx) {
    if (!ctx) return false;
    return ctx->current_state == STATE_STREAMING || ctx->streaming_active;
}

bool StateMachine_IsSleeping(const StateMachineContext_t* ctx) {
    if (!ctx) return false;
    return ctx->current_state == STATE_SLEEP;
}

bool StateMachine_IsIdle(const StateMachineContext_t* ctx) {
    if (!ctx) return false;
    return ctx->current_state == STATE_IDLE;
}

bool StateMachine_IsBleConnected(const StateMachineContext_t* ctx) {
    if (!ctx) return false;
    return ctx->ble_connected;
}

void StateMachine_SetBleConnected(StateMachineContext_t* ctx, bool connected) {
    if (!ctx) return;
    ctx->ble_connected = connected;
}

void StateMachine_SetRecordingActive(StateMachineContext_t* ctx, bool active) {
    if (!ctx) return;
    ctx->recording_active = active;
}

void StateMachine_SetStreamingActive(StateMachineContext_t* ctx, bool active) {
    if (!ctx) return;
    ctx->streaming_active = active;
}

// Internal state transition function
static void StateMachine_Transition(StateMachineContext_t* ctx, AppState_t new_state) {
    if (!ctx) return;
    
    // Call exit callback if configured
    if (ctx->config.on_exit && ctx->current_state != STATE_INVALID) {
        ctx->config.on_exit(ctx->current_state);
    }
    
    // Update previous state
    ctx->previous_state = ctx->current_state;
    
    // Update current state
    ctx->current_state = new_state;
    
    // Reset state entry time
    ctx->state_entry_time = 0;
    
    // Reset idle timer on state change
    ctx->idle_timer = 0;
    
    // Call enter callback if configured
    if (ctx->config.on_enter && new_state != STATE_INVALID) {
        ctx->config.on_enter(new_state);
    }
}

// State handlers
static void StateMachine_HandleIdle(StateMachineContext_t* ctx, AppEvent_t event) {
    switch (event) {
        case EVENT_BLE_CONNECTED:
            // BLE connected, can start recording if configured
            if (ctx->config.recording_buffer_size > 0) {
                StateMachine_Transition(ctx, STATE_RECORDING);
            }
            break;
            
        case EVENT_START_RECORDING:
            // Start recording directly
            StateMachine_Transition(ctx, STATE_RECORDING);
            break;
            
        case EVENT_LOW_BATTERY:
            // Enter sleep mode for power saving
            StateMachine_Transition(ctx, STATE_SLEEP);
            break;
            
        case EVENT_ENTER_SLEEP:
            // Manual sleep entry
            StateMachine_Transition(ctx, STATE_SLEEP);
            break;
            
        default:
            // No action for other events in idle state
            break;
    }
}

static void StateMachine_HandleRecording(StateMachineContext_t* ctx, AppEvent_t event) {
    switch (event) {
        case EVENT_STOP_RECORDING:
            // Stop recording, transition to streaming if BLE connected
            if (ctx->ble_connected) {
                StateMachine_Transition(ctx, STATE_STREAMING);
            } else {
                StateMachine_Transition(ctx, STATE_IDLE);
            }
            break;
            
        case EVENT_START_STREAMING:
            // Start streaming directly
            StateMachine_Transition(ctx, STATE_STREAMING);
            break;
            
        case EVENT_BLE_DISCONNECTED:
            // BLE disconnected, return to idle
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        case EVENT_LOW_BATTERY:
            // Enter sleep mode for power saving
            StateMachine_Transition(ctx, STATE_SLEEP);
            break;
            
        default:
            // Recording continues, buffer audio data
            break;
    }
}

static void StateMachine_HandleStreaming(StateMachineContext_t* ctx, AppEvent_t event) {
    switch (event) {
        case EVENT_STOP_STREAMING:
            // Stop streaming, return to idle
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        case EVENT_START_RECORDING:
            // Start recording again
            StateMachine_Transition(ctx, STATE_RECORDING);
            break;
            
        case EVENT_BLE_DISCONNECTED:
            // BLE disconnected, return to idle
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        case EVENT_LOW_BATTERY:
            // Enter sleep mode for power saving
            StateMachine_Transition(ctx, STATE_SLEEP);
            break;
            
        default:
            // Streaming continues, send buffered audio data
            break;
    }
}

static void StateMachine_HandleSleep(StateMachineContext_t* ctx, AppEvent_t event) {
    switch (event) {
        case EVENT_WAKE_UP:
            // Wake up from sleep, return to idle
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        case EVENT_BUTTON_PRESSED:
            // Button press wakes up device
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        case EVENT_HIGH_BATTERY:
            // Battery charged, wake up
            StateMachine_Transition(ctx, STATE_IDLE);
            break;
            
        default:
            // Stay in sleep mode
            break;
    }
}
