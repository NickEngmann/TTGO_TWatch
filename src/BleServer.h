#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef NATIVE_BUILD
#include <functional>
#include <memory>
#else
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#endif

// Custom Service UUID for T-Watch Audio (128-bit UUID)
#define TWATCH_AUDIO_SERVICE_UUID "0000fff0-0000-1000-8000-00805f9b34fb"
#define TWATCH_AUDIO_DATA_UUID    "0000fff1-0000-1000-8000-00805f9b34fb"
#define TWATCH_STATUS_UUID        "0000fff2-0000-1000-8000-00805f9b34fb"
#define TWATCH_RECORD_UUID        "0000fff3-0000-1000-8000-00805f9b34fb"

// BLE MTU for audio streaming
#define BLE_MTU_SIZE 247
#define MAX_AUDIO_CHUNK_SIZE 200  // Leave room for headers

// Audio streaming states
enum AudioState {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_RECORDING = 1,
    AUDIO_STATE_STREAMING = 2,
    AUDIO_STATE_PAUSED = 3,
    AUDIO_STATE_ERROR = 4
};

// Connection status
enum ConnectionState {
    CONNECTION_DISCONNECTED = 0,
    CONNECTION_CONNECTING = 1,
    CONNECTION_CONNECTED = 2
};

// Callback function types
typedef std::function<void(bool connected)> ConnectionCallback;
typedef std::function<void(uint8_t state)> AudioStateCallback;
typedef std::function<void(uint16_t chunkSize)> ChunkSentCallback;

class BleServer {
public:
    BleServer();
    ~BleServer();

    // Initialize BLE server
    bool initialize(const char* deviceName = "TWatch Audio");
    
    // Start advertising
    bool advertise();
    
    // Stop advertising
    void stopAdvertising();
    
    // Send audio chunk to connected client
    bool sendChunk(const uint8_t* data, size_t length);
    
    // Check if connected
    bool isConnected() const;
    
    // Get current connection state
    ConnectionState getConnectionState() const;
    
    // Get current audio state
    AudioState getAudioState() const;
    
    // Set audio state
    void setAudioState(AudioState state);
    
    // Set recording state
    void setRecordingState(bool recording);
    
    // Register callbacks
    void onConnectionChange(ConnectionCallback callback);
    void onAudioStateChange(AudioStateCallback callback);
    void onChunkSent(ChunkSentCallback callback);
    
    // Get service UUID
    static const char* getServiceUUID();
    
    // Get MTU size
    static size_t getMTUSize();
    
    // Get max chunk size
    static size_t getMaxChunkSize();

private:
#ifdef NATIVE_BUILD
    // Native build simulation state
    ConnectionState _connectionState;
    AudioState _audioState;
    bool _advertising;
    size_t _currentMTU;
    ConnectionCallback _connCallback;
    AudioStateCallback _audioCallback;
    ChunkSentCallback _chunkCallback;
    void* _currentClient;  // Placeholder for client pointer
    
    void _simulateSendChunk(const uint8_t* data, size_t length);
#else
    // ESP32 BLE implementation
    BLEServer* _server;
    BLEService* _audioService;
    BLECharacteristic* _dataCharacteristic;
    BLECharacteristic* _statusCharacteristic;
    BLECharacteristic* _recordCharacteristic;
    
    BLECharacteristic* _notifyCharacteristic;
    
    // Callbacks
    ConnectionCallback _connCallback;
    AudioStateCallback _audioCallback;
    ChunkSentCallback _chunkCallback;
    
    // BLE callbacks
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(BleServer* parent) : _parent(parent) {}
        void onConnect(BLEClient* client);
        void onDisconnect(BLEClient* client);
        
    private:
        BleServer* _parent;
    };
    
    class NotifyCallbacks : public BLECharacteristicCallbacks {
    public:
        NotifyCallbacks(BleServer* parent) : _parent(parent) {}
        void onWrite(BLECharacteristic* pCharacteristic);
        
    private:
        BleServer* _parent;
    };
    
    ServerCallbacks _serverCallbacks;
    NotifyCallbacks _notifyCallbacks;
    
    // State
    ConnectionState _connectionState;
    AudioState _audioState;
    bool _advertising;
    size_t _currentMTU;
    BLEClient* _currentClient;
#endif
};
