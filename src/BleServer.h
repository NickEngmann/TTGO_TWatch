#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <BLE2902.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#ifdef NATIVE_BUILD
#include <functional>
#include <cstdint>
#include <cstring>
#endif

namespace TWatch {

enum class RecordingState : uint8_t {
    IDLE = 0,
    RECORDING = 1,
    PAUSED = 2,
    STOPPED = 3
};

enum class CONNECTION_STATE : uint8_t {
    CONNECTED = 1,
    DISCONNECTED = 0
};

typedef std::function<void(CONNECTION_STATE state)> ConnectionCallback;

class BleServer {
public:
    BleServer();
    ~BleServer();
    
    // Initialization
    bool init();
    void startAdvertising();
    void stopAdvertising();
    
    // Connection management
    bool isConnected();
    void setConnectionCallback(ConnectionCallback callback);
    void startConnection();
    void endConnection();
    bool checkConnectionTimeout();
    
    // Audio streaming
    bool sendChunk(const uint8_t* data, size_t length);
    bool sendChunk(uint16_t sample);
    void getAudioBuffer(uint8_t* buffer, size_t length);
    void setAudioData(const uint8_t* data, size_t length);
    void resetAudioBuffer();
    
    // Configuration
    void setChunkSize(size_t size);
    size_t getChunkSize() const;
    void setMtu(uint16_t mtu);
    uint16_t getMtu() const;
    
    // State management
    void setRecordingState(RecordingState state);
    RecordingState getRecordingState() const { return _recordingState; }
    
    // Callback handlers (for BLE device)
    static void onConnect(BLEServer* pServer);
    static void onDisconnect(BLEServer* pServer);
    
private:
    bool _isAdvertising;
    bool _isConnected;
    RecordingState _recordingState;
    uint16_t _mtu;
    size_t _chunkSize;
    unsigned long _lastConnectionTime;
    const uint16_t _connectionTimeout;
    
    uint8_t _buffer[MAX_CHUNK_SIZE];
    uint8_t _audioData[MAX_AUDIO_DATA_SIZE];
    
    ConnectionCallback _connectionCallback;
    
#ifdef NATIVE_BUILD
    bool _bleInitialized;
#else
    BLEServer* _server;
    BLEService* _audioService;
    BLECharacteristic* _audioDataChar;
    BLECharacteristic* _connectionStatusChar;
    BLECharacteristic* _recordingStateChar;
#endif
};

} // namespace TWatch

#endif // BLE_SERVER_H
