# Luxopi-ROS Design Improvements for TTGO_TWatch

## Overview

This document describes design improvements for integrating TTGO_TWatch with Luxopi-ROS (Robot Operating System) ecosystem, focusing on real-time sensor data streaming, BLE communication, and state management optimizations.

---

## 1. Architecture Design

### 1.1 System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    TTGO_TWatch ESP32                        │
├─────────────────────────────────────────────────────────────┤
│  Application Layer                                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │   ROS Node  │  │  State Mgr  │  │  BLE Audio Server  │ │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘ │
│         │                │                     │            │
│  ┌──────▼────────────────▼─────────────────────▼──────────┐ │
│  │             Middleware Layer (MQTT/BLE)                │ │
│  └───────────────────────────┬────────────────────────────┘ │
│                              │                               │
│  ┌───────────────────────────▼────────────────────────────┐ │
│  │           Sensor Abstraction Layer                     │ │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐  │ │
│  │  │ BMA423   │ │ AXP20X   │ │ FT6236   │ │  PCM   │  │ │
│  │  │ Acceler  │ │  Power   │ │  Touch   │ │  Audio │  │ │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘  │ │
│  └───────────────────────────┬────────────────────────────┘ │
│                              │                               │
│  ┌───────────────────────────▼────────────────────────────┐ │
│  │           Hardware Abstraction Layer                   │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────▼─────────┐
                    │   ROS 2 Network   │
                    └───────────────────┘
```

### 1.2 Communication Protocol

#### BLE GATT Service for ROS Integration

```c
// Service UUID: 0xFFF0 (ROS2_TWATCH_SERVICE)
// Characteristics:
//   - 0xFFF1: Sensor Data (16 bytes)
//   - 0xFFF2: State Report (8 bytes)
//   - 0xFFF3: Control Commands (4 bytes)
//   - 0xFFF4: Battery Status (2 bytes)
```

---

## 2. State Machine Improvements

### 2.1 Enhanced State Transitions

**Current State Machine** (from `StateMachine.cpp`):
- IDLE
- RECORDING  
- STREAMING
- ERROR
- DEEP_SLEEP

**Proposed Enhancements**:

```cpp
enum ROSState {
    ROS_IDLE = 0,
    ROS_CONNECTING,
    ROS_CONNECTED,
    ROS_STREAMING,
    ROS_PAUSED,
    ROS_ERROR_RECOVERING,
    ROS_ERROR,
    ROS_DEEP_SLEEP
};

class ROSStateMachine {
private:
    ROSState currentState;
    ROSState previousState;
    unsigned long stateEnterTime;
    unsigned long watchdogTimeout;
    
    // Callbacks
    std::function<void(ROSState)> onStateChange;
    std::function<void(ROSState, ROSState)> onTransition;
    
public:
    bool transitionTo(ROSState newState);
    void checkWatchdog();
    void reset();
    ROSState getCurrentState() const;
    
    // Edge-triggered state change notifications
    bool hasStateChanged();
};
```

### 2.2 Transition Guards

```cpp
// Example: Prevent invalid transitions
bool ROSStateMachine::isValidTransition(ROSState from, ROSState to) {
    switch (from) {
        case ROS_IDLE:
            return to == ROS_CONNECTING || to == ROS_DEEP_SLEEP;
        case ROS_CONNECTING:
            return to == ROS_CONNECTED || to == ROS_ERROR_RECOVERING;
        case ROS_CONNECTED:
            return to == ROS_STREAMING || to == ROS_PAUSED || 
                   to == ROS_ERROR_RECOVERING;
        case ROS_STREAMING:
            return to == ROS_PAUSED || to == ROS_ERROR_RECOVERING;
        case ROS_PAUSED:
            return to == ROS_STREAMING || to == ROS_ERROR_RECOVERING;
        case ROS_ERROR_RECOVERING:
            return to == ROS_CONNECTED || to == ROS_ERROR ||
                   to == ROS_IDLE;
        case ROS_ERROR:
            return to == ROS_IDLE || to == ROS_DEEP_SLEEP;
        case ROS_DEEP_SLEEP:
            return to == ROS_IDLE;
        default:
            return false;
    }
}
```

---

## 3. Sensor Data Streaming

### 3.1 BMA423 Accelerometer Integration

```cpp
// Enhanced sensor data structure
struct ROS2SensorData {
    // IMU Data (12 bytes)
    int16_t accelerometer[3];  // X, Y, Z (mg)
    int16_t temperature;       // DegC * 100
    
    // Touch Data (4 bytes)
    uint8_t touchPoints;
    uint8_t gestureType;
    
    // Battery Status (2 bytes)
    uint8_t batteryLevel;
    uint8_t chargingStatus;
    
    // Timestamp (8 bytes)
    uint64_t timestamp;
};

// Optimized streaming with batching
class SensorDataStreamer {
private:
    BLEServer* server;
    BLEService* rosService;
    BLECharacteristic* sensorChar;
    BLECharacteristic* stateChar;
    
    std::vector<ROS2SensorData> dataBuffer;
    size_t maxBufferSize;
    uint32_t streamingInterval;
    
public:
    SensorDataStreamer(uint32_t interval = 100); // ms
    void startStreaming();
    void stopStreaming();
    void addData(const ROS2SensorData& data);
    void flushBuffer();
};
```

### 3.2 Data Compression

For BLE bandwidth optimization, implement simple run-length encoding:

```cpp
// Compressed sensor data (max 8 bytes)
struct CompressedSensorData {
    uint8_t flags;           // Bit flags for data validity
    int8_t accelDelta[3];    // Delta encoding for X, Y, Z
    int8_t tempDelta;        // Delta encoding
    
    // Compression ratio: ~40% reduction for stationary watch
};
```

---

## 4. Battery Monitor Enhancements

### 4.1 Edge-Triggered Alerts

**Current Implementation**:
- Voltage monitoring
- Percentage tracking  
- Charging detection

**Proposed Improvements**:

```cpp
class EnhancedBatteryMonitor {
private:
    float voltage;
    uint8_t batteryPercentage;
    bool isCharging;
    bool isUSBConnected;
    
    // Thresholds with hysteresis
    float lowVoltageThreshold;
    float criticalVoltageThreshold;
    float highVoltageThreshold;
    float fullChargeThreshold;
    
    // Edge-triggered flags (fire once per crossing)
    bool lowVoltageAlertFired;
    bool criticalVoltageAlertFired;
    bool chargingStartedFired;
    bool chargingCompleteFired;
    
    // Power saving mode
    bool powerSavingMode;
    
public:
    void update();
    void checkThresholds();
    bool isLowVoltageAlertPending();
    bool isCriticalVoltageAlertPending();
    bool resetAlerts();
    void setPowerSavingMode(bool enable);
};
```

### 4.2 Power Consumption Analysis

```cpp
struct PowerConsumptionReport {
    uint32_t totalConsumed_mWh;
    uint32_t chargingTime_s;
    uint32_t dischargingTime_s;
    float averageVoltage;
    float averageCurrent_uA;
    uint32_t deepSleepCycles;
    uint32_t wakeEvents;
};
```

---

## 5. BLE Audio Stream Improvements

### 5.1 State Machine Bug Fix

**Root Cause**: `onConnection()` doesn't transition to CONNECTED when in ADVERTISING state.

**Fixed Implementation**:

```cpp
void BLEAudioStream::onConnection(ble_gap_event_listener_t* listener, 
                                   ble_gap_conn_event_t* event, 
                                   void* arg) {
    if (event->connect.status != 0) {
        // Connection failed
        return;
    }
    
    // Fixed: Always check current state before transitioning
    switch (currentState) {
        case BLE_STATE_ADVERTISING:
        case BLE_STATE_IDLE:
            // Allow connection from these states
            currentState = BLE_STATE_CONNECTED;
            onConnectionEstablished();
            break;
        case BLE_STATE_STREAMING:
            // Already streaming, handle concurrent connection
            handleConcurrentConnection();
            break;
        default:
            // Reject connection in invalid states
            rejectConnection();
            break;
    }
}
```

### 5.2 Audio Chunk Management

```cpp
// Optimized audio chunk handling
struct AudioChunk {
    uint8_t data[AUDIO_CHUNK_SIZE];
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint8_t flags;  // LAST_CHUNK, ERROR_CORRECTION, etc.
    
    bool operator==(const AudioChunk& other) const {
        return sequenceNumber == other.sequenceNumber;
    }
};

class AudioStreamManager {
private:
    std::deque<AudioChunk> pendingChunks;
    uint16_t lastReceivedSeq;
    uint16_t expectedSeq;
    uint8_t errorCorrectionLevel;
    
public:
    void addChunk(const AudioChunk& chunk);
    bool processChunk();
    void handleOutOfOrder(uint16_t seq);
    void requestRetransmission(uint16_t seq);
};
```

---

## 6. Step Counter Integration with ROS

### 6.1 Step Data Publication

```cpp
struct ROS2StepData {
    uint32_t totalSteps;
    uint32_t dailyGoal;
    uint32_t stepsToday;
    uint32_t stepsThisWeek;
    uint32_t stepsThisMonth;
    float distance_meters;
    uint8_t activeMinutes;
    uint64_t timestamp;
};

// Publish to ROS2 topic: /ttgo_twatch/step_data
class StepCounterROS {
private:
    StepCounter& stepCounter;
    rclcpp::Publisher<ROS2StepData>::SharedPtr publisher;
    
public:
    StepCounterROS(StepCounter& sc, rclcpp::Node::SharedPtr node);
    void publishSteps();
    void resetDailySteps();
    void setDailyGoal(uint32_t goal);
};
```

### 6.2 Gesture Recognition for ROS Commands

```cpp
// Common gestures mapped to ROS commands
enum GestureType {
    GESTURE_NONE = 0,
    GESTURE_WAVE_LEFT,      // -> /cmd_vel twist (linear.x)
    GESTURE_WAVE_RIGHT,     // -> /cmd_vel twist (linear.x)
    GESTURE_RAISE_WRIST,    // -> Wake up / ROS connection
    GESTURE_LOWER_WRIST,    // -> Sleep / Disconnect
    GESTURE_SHAKE,          // -> Emergency stop / /cmd_vel zero
    GESTURE_TAP_DOUBLE,     // -> Toggle streaming
    GESTURE_TAP_TRIPLE      // -> Take snapshot
};

class GestureDetector {
public:
    GestureType detectGesture(const BMA423Data& accel);
    bool isGesturing();
    void resetGesture();
};
```

---

## 7. File Structure for ROS Integration

```
src/
├── ros/
│   ├── ROS2Node.h          # Main ROS2 node wrapper
│   ├── ROS2Node.cpp
│   ├── SensorPublisher.h   # Sensor data publishers
│   ├── SensorPublisher.cpp
│   ├── BLEROSAdapter.h     # BLE to ROS2 adapter
│   ├── BLEROSAdapter.cpp
│   └── ROS2Config.h        # ROS2 configuration
├── drive/
│   ├── bma423/
│   │   ├── ROS2BMA423.h    # ROS2 wrapper for BMA423
│   │   └── ROS2BMA423.cpp
│   └── axp/
│       ├── ROS2Battery.h   # ROS2 battery monitor
│       └── ROS2Battery.cpp
├── lib/
│   ├── ros2_common.h       # Common ROS2 includes
│   └── ros2_stub.h         # Non-ESP32 stubs for testing
```

---

## 8. Testing Strategy

### 8.1 Unit Tests

```cpp
// test/test_ros_integration/
// ├── test_ros_state_machine.cpp
// ├── test_sensor_publisher.cpp
// ├── test_ble_ros_adapter.cpp
// ├── test_battery_monitor_ros.cpp
// └── test_step_counter_ros.cpp
```

### 8.2 Integration Tests

- BLE audio streaming with ROS2 client simulation
- Sensor data accuracy validation
- State transition correctness verification
- Battery alert edge-triggered testing

---

## 9. Performance Considerations

### 9.1 Memory Optimization

- Use fixed-size buffers for BLE packets
- Implement circular buffer for sensor data
- Minimize dynamic allocations in interrupt context

### 9.2 Power Management

```cpp
// Aggressive power saving modes
enum PowerMode {
    POWER_MODE_HIGH_PERFORMANCE,  // 100Hz sensor polling
    POWER_MODE_BALANCED,          // 50Hz polling
    POWER_MODE_LOW_POWER,         // 10Hz polling
    POWER_MODE_ULTRA_LOW_POWER    // 1Hz polling, deep sleep between
};

class PowerManager {
public:
    void setMode(PowerMode mode);
    void enterDeepSleep(uint32_t duration_ms);
    void scheduleWakeEvent(uint32_t delay_ms);
};
```

---

## 10. Implementation Roadmap

### Phase 1: Core Infrastructure (Week 1-2)
- [ ] Implement ROS2 state machine
- [ ] Add BLE GATT service structure
- [ ] Create sensor data structures
- [ ] Implement edge-triggered battery alerts

### Phase 2: Data Streaming (Week 3-4)
- [ ] Integrate BMA423 with ROS2 publisher
- [ ] Implement step counter ROS2 integration
- [ ] Add BLE audio stream improvements
- [ ] Create gesture recognition system

### Phase 3: Optimization (Week 5-6)
- [ ] Implement data compression
- [ ] Add power management modes
- [ ] Performance profiling and tuning
- [ ] Integration testing

### Phase 4: Documentation & Examples (Week 7-8)
- [ ] ROS2 node examples
- [ ] Python client examples
- [ ] API documentation
- [ ] Tutorials and guides

---

## 11. References

- [ROS2 Documentation](https://docs.ros.org/)
- [ESP32 BLE Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/ble.html)
- [BMA423 Datasheet](https://github.com/Xinyuan-LilyGO/LilyGo-HAL/tree/master/BMA423)
- [TTGO_TWatch Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)

---

## 12. Appendix A: ROS2 Message Definitions

```idl
# msg/StepData.msg
uint32_t total_steps
uint32_t daily_goal
uint32_t steps_today
uint32_t steps_this_week
float32 distance_meters
uint8 active_minutes
uint64 timestamp

# msg/BatteryStatus.msg
float32 voltage
uint8 percentage
bool charging
bool usb_connected
bool low_voltage_alert
bool critical_voltage_alert

# msg/SensorData.msg
int16_t accel_x
int16_t accel_y
int16_t accel_z
int16_t temperature
uint8 touch_points
uint8 gesture_type
uint64 timestamp

# msg/ROSState.msg
uint8 state
uint8 previous_state
uint64 state_enter_time
string error_message
```

---

**Document Version**: 1.0  
**Last Updated**: 2026-03-25  
**Author**: TTGO_TWatch Development Team  
**License**: MIT
