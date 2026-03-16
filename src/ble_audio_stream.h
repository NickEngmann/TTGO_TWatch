#pragma once

#include <stdint.h>
#include <string>
#include <functional>

// BLE Audio Streaming Module
// This module handles BLE GATT server for audio streaming to phone

#ifndef NATIVE_BUILD
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLECharacteristic.h>
#endif

// Audio chunk size (BLE MTU limit)
constexpr uint16_t AUDIO_CHUNK_SIZE = 512;

// Custom service UUID for audio streaming
constexpr const char* AUDIO_SERVICE_UUID = "0000fff0-0000-1000-8000-00805f9b34fb";

// Custom characteristic UUIDs
constexpr const char* AUDIO_DATA_UUID = "0000fff1-0000-1000-8000-00805f9b34fb";
constexpr const char* AUDIO_CONTROL_UUID = "0000fff2-0000-1000-8000-00805f9b34fb";

// Standard BLE Battery Service UUID
constexpr const char* BATTERY_SERVICE_UUID = "0000180f-0000-1000-8000-00805f9b34fb";
constexpr const char* BATTERY_LEVEL_UUID = "00002a19-0000-1000-8000-00805f9b34fb";

// BLE Connection states
enum class BLEConnectionState {
    DISCONNECTED,
    CONNECTED
};

// Audio streaming states (state machine)
enum class AudioStreamState {
    IDLE,
    ADVERTISING,
    CONNECTED,
    STREAMING,
    ERROR
};

// Audio streaming control commands
enum class AudioControlCommand {
    START_STREAM,
    STOP_STREAM,
    PAUSE_STREAM,
    RESUME_STREAM
};

// Audio streamer class for BLE GATT server
class BLEAudioStreamer {
public:
    BLEAudioStreamer();
    ~BLEAudioStreamer();

    // State machine methods
    AudioStreamState getState() const;
    bool startAdvertising();
    bool stopAdvertising();
    void onConnection(BLEConnectionState state);
    void onDisconnect();

    // Audio streaming methods
    bool startStreaming();
    bool stopStreaming();
    bool pauseStreaming();
    bool resumeStreaming();
    bool sendAudioChunk(const uint8_t* data, uint16_t size);

    // Control methods
    bool handleControlCommand(AudioControlCommand cmd);

    // Battery level reporting
    void setBatteryLevel(uint8_t level);
    uint8_t getBatteryLevel() const;

    // Callbacks
    using ConnectionCallback = std::function<void(BLEConnectionState)>;
    using AudioCallback = std::function<void(const uint8_t*, uint16_t)>;

    void setConnectionCallback(ConnectionCallback cb);
    void setAudioCallback(AudioCallback cb);

    // Getters for testing
    uint16_t getChunkSize() const { return AUDIO_CHUNK_SIZE; }
    const std::string& getServiceUUID() const { return serviceUUID; }
    const std::string& getDataUUID() const { return dataUUID; }
    const std::string& getControlUUID() const { return controlUUID; }
    const std::string& getBatteryUUID() const { return batteryUUID; }

private:
    AudioStreamState currentState;
    BLEConnectionState connectionState;
    uint8_t batteryLevel;

    // UUIDs as strings for testing
    std::string serviceUUID;
    std::string dataUUID;
    std::string controlUUID;
    std::string batteryUUID;

    // Callbacks
    ConnectionCallback connectionCallback;
    AudioCallback audioCallback;

    // Internal methods
    bool validateStateTransition(AudioStreamState newState);
    void updateState(AudioStreamState newState);
    bool isStreamingReady() const;
};

// BLE GATT Server wrapper class (for hardware integration)
class BLEGattServer {
public:
    BLEGattServer();
    ~BLEGattServer();

    // Initialization
    bool initialize();
    void deinitialize();

    // Service setup
    bool setupAudioService();
    bool setupBatteryService();

    // Advertising
    void startAdvertising();
    void stopAdvertising();

    // Connection handling
    void onConnect();
    void onDisconnect();

    // Audio streaming
    bool sendAudioData(const uint8_t* data, uint16_t size);

    // Battery level
    void setBatteryLevel(uint8_t level);

    // State getters for testing
    bool isInitialized() const { return initialized; }
    bool hasAudioService() const { return audioServiceSetup; }
    bool hasBatteryService() const { return batteryServiceSetup; }
    bool isAdvertising() const { return advertising; }
    bool isConnected() const { return connected; }

private:
    bool initialized;
    bool audioServiceSetup;
    bool batteryServiceSetup;
    bool advertising;
    bool connected;

    // Mock BLE objects for native build
#ifndef NATIVE_BUILD
    NimBLEServer* server;
    NimBLEService* audioService;
    NimBLEService* batteryService;
    NimBLECharacteristic* audioDataChar;
    NimBLECharacteristic* audioControlChar;
    NimBLECharacteristic* batteryLevelChar;
#else
    // Mock objects for native build
    void* server;
    void* audioService;
    void* batteryService;
    void* audioDataChar;
    void* audioControlChar;
    void* batteryLevelChar;
#endif
};
