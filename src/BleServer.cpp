#include "BleServer.h"
#include <cstring>
#include <algorithm>

#ifdef NATIVE_BUILD
#include <iostream>
#endif

namespace TWatch {

// Custom UUID for T-Watch audio service
static const uint8_t AUDIO_SERVICE_UUID[] = {0x11, 0x00, 0x20, 0x24, 0x4A, 0x48, 0x45, 0x53, 0x53, 0x45, 0x20, 0x2A, 0x55, 0x46, 0x4C, 0x31};
static const uint8_t AUDIO_DATA_CHAR_UUID[] = {0x11, 0x01, 0x20, 0x24, 0x4A, 0x48, 0x45, 0x53, 0x53, 0x45, 0x20, 0x2A, 0x55, 0x46, 0x4C, 0x31};
static const uint8_t CONNECTION_STATUS_CHAR_UUID[] = {0x11, 0x02, 0x20, 0x24, 0x4A, 0x48, 0x53, 0x53, 0x45, 0x20, 0x2A, 0x55, 0x46, 0x4C, 0x31};
static const uint8_t RECORDING_STATE_CHAR_UUID[] = {0x11, 0x03, 0x20, 0x24, 0x4A, 0x48, 0x53, 0x53, 0x45, 0x20, 0x2A, 0x55, 0x46, 0x4C, 0x31};

BleServer::BleServer() 
    : _isAdvertising(false)
    , _isConnected(false)
    , _recordingState(RecordingState::IDLE)
    , _mtu(247)
    , _chunkSize(160)  // 160 bytes per chunk (10ms at 16kHz 16-bit mono)
    , _lastConnectionTime(0)
    , _connectionTimeout(5000)  // 5 second timeout
{
    memset(_buffer, 0, sizeof(_buffer));
    memset(_audioData, 0, sizeof(_audioData));
}

BleServer::~BleServer() {
    stopAdvertising();
}

bool BleServer::init() {
#ifdef NATIVE_BUILD
    _bleInitialized = true;
    return true;
#else
    // Initialize BLE on ESP32
    BLEDevice::init("T-Watch Audio");
    _bleInitialized = true;
    return true;
#endif
}

void BleServer::startAdvertising() {
#ifdef NATIVE_BUILD
    _isAdvertising = true;
    std::cout << "[NATIVE] BLE advertising started: T-Watch Audio" << std::endl;
    return;
#else
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService((uint8_t*)AUDIO_SERVICE_UUID, sizeof(AUDIO_SERVICE_UUID));
    
    // Create audio data characteristic with notify
    BLECharacteristic *pAudioChar = pService->createCharacteristic(
        (uint8_t*)AUDIO_DATA_CHAR_UUID, sizeof(AUDIO_DATA_CHAR_UUID),
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pAudioChar->addDescriptor(new BLE2902());
    
    // Create connection status characteristic
    BLECharacteristic *pConnChar = pService->createCharacteristic(
        (uint8_t*)CONNECTION_STATUS_CHAR_UUID, sizeof(CONNECTION_STATUS_CHAR_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    uint8_t status = 0;
    pConnChar->setValue(&status, 1);
    pConnChar->addDescriptor(new BLE2902());
    
    // Create recording state characteristic
    BLECharacteristic *pRecordChar = pService->createCharacteristic(
        (uint8_t*)RECORDING_STATE_CHAR_UUID, sizeof(RECORDING_STATE_CHAR_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    uint8_t state = static_cast<uint8_t>(_recordingState);
    pRecordChar->setValue(&state, 1);
    pRecordChar->addDescriptor(new BLE2902());
    
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06, 0x00, 0x10);
    BLEDevice::startAdvertising();
    _isAdvertising = true;
#endif
}

void BleServer::stopAdvertising() {
#ifdef NATIVE_BUILD
    if (_isAdvertising) {
        std::cout << "[NATIVE] BLE advertising stopped" << std::endl;
        _isAdvertising = false;
    }
    return;
#else
    BLEDevice::getAdvertising()->stop();
    _isAdvertising = false;
#endif
}

bool BleServer::isConnected() {
#ifdef NATIVE_BUILD
    return _isConnected;
#else
    return BLEDevice::getServer()->getConnectedCount() > 0;
#endif
}

void BleServer::setConnectionCallback(ConnectionCallback callback) {
    _connectionCallback = callback;
#ifdef NATIVE_BUILD
    if (callback) {
        std::cout << "[NATIVE] Connection callback registered" << std::endl;
    }
#endif
}

void BleServer::setRecordingState(RecordingState state) {
    _recordingState = state;
#ifdef NATIVE_BUILD
    std::cout << "[NATIVE] Recording state changed: " << static_cast<int>(state) << std::endl;
#else
    if (_isConnected && _recordingStateChar) {
        uint8_t stateVal = static_cast<uint8_t>(state);
        _recordingStateChar->setValue(&stateVal, 1);
        _recordingStateChar->notify();
    }
#endif
}

bool BleServer::sendChunk(const uint8_t* data, size_t length) {
#ifdef NATIVE_BUILD
    if (!_isConnected) {
        std::cout << "[NATIVE] Cannot send chunk: not connected" << std::endl;
        return false;
    }
    
    if (length > _chunkSize) {
        length = _chunkSize;
    }
    
    std::cout << "[NATIVE] Sending audio chunk: " << length << " bytes" << std::endl;
    return true;
#else
    if (!_isConnected || !_audioDataChar) {
        return false;
    }
    
    if (length > _chunkSize) {
        length = _chunkSize;
    }
    
    _audioDataChar->setValue(data, length);
    _audioDataChar->notify();
    return true;
#endif
}

bool BleServer::sendChunk(uint16_t sample) {
    uint8_t chunk[2];
    chunk[0] = sample & 0xFF;
    chunk[1] = (sample >> 8) & 0xFF;
    return sendChunk(chunk, 2);
}

void BleServer::startConnection() {
#ifdef NATIVE_BUILD
    _isConnected = true;
    _lastConnectionTime = millis();
    std::cout << "[NATIVE] BLE connection established" << std::endl;
    if (_connectionCallback) {
        _connectionCallback(CONNECTION_STATE::CONNECTED);
    }
#else
    _isConnected = true;
    _lastConnectionTime = millis();
    if (_connectionCallback) {
        _connectionCallback(CONNECTION_STATE::CONNECTED);
    }
#endif
}

void BleServer::endConnection() {
#ifdef NATIVE_BUILD
    _isConnected = false;
    std::cout << "[NATIVE] BLE connection ended" << std::endl;
    if (_connectionCallback) {
        _connectionCallback(CONNECTION_STATE::DISCONNECTED);
    }
#else
    _isConnected = false;
    if (_connectionCallback) {
        _connectionCallback(CONNECTION_STATE::DISCONNECTED);
    }
#endif
}

bool BleServer::checkConnectionTimeout() {
#ifdef NATIVE_BUILD
    if (_isConnected && (millis() - _lastConnectionTime) > _connectionTimeout) {
        endConnection();
        return true;
    }
    return false;
#else
    if (_isConnected && (millis() - _lastConnectionTime) > _connectionTimeout) {
        endConnection();
        return true;
    }
    return false;
#endif
}

void BleServer::setChunkSize(size_t size) {
    if (size > MAX_CHUNK_SIZE) {
        size = MAX_CHUNK_SIZE;
    }
    _chunkSize = size;
#ifdef NATIVE_BUILD
    std::cout << "[NATIVE] Chunk size set to: " << _chunkSize << " bytes" << std::endl;
#endif
}

size_t BleServer::getChunkSize() const {
    return _chunkSize;
}

void BleServer::setMtu(uint16_t mtu) {
    _mtu = std::min(mtu, static_cast<uint16_t>(247));
#ifdef NATIVE_BUILD
    std::cout << "[NATIVE] MTU set to: " << _mtu << " bytes" << std::endl;
#endif
}

uint16_t BleServer::getMtu() const {
    return _mtu;
}

void BleServer::getAudioBuffer(uint8_t* buffer, size_t length) {
#ifdef NATIVE_BUILD
    memset(buffer, 0, length);
    for (size_t i = 0; i < length && i < sizeof(_audioData); i++) {
        buffer[i] = _audioData[i] & 0xFF;
    }
#else
    memcpy(buffer, _audioData, std::min(length, sizeof(_audioData)));
#endif
}

void BleServer::setAudioData(const uint8_t* data, size_t length) {
#ifdef NATIVE_BUILD
    size_t copyLen = std::min(length, sizeof(_audioData));
    memcpy(_audioData, data, copyLen);
    std::cout << "[NATIVE] Audio data set: " << copyLen << " bytes" << std::endl;
#else
    size_t copyLen = std::min(length, sizeof(_audioData));
    memcpy(_audioData, data, copyLen);
#endif
}

void BleServer::resetAudioBuffer() {
#ifdef NATIVE_BUILD
    memset(_audioData, 0, sizeof(_audioData));
    std::cout << "[NATIVE] Audio buffer reset" << std::endl;
#else
    memset(_audioData, 0, sizeof(_audioData));
#endif
}

void BleServer::onConnect(BLEServer* pServer) {
#ifdef NATIVE_BUILD
    startConnection();
#else
    BLEDevice::getServer()->setCallbacks(new BleCallbacks());
    startConnection();
#endif
}

void BleServer::onDisconnect(BLEServer* pServer) {
#ifdef NATIVE_BUILD
    endConnection();
#else
    endConnection();
#endif
}

} // namespace TWatch
