#include "BleServer.h"
#include <cstring>
#include <cstdlib>

#ifdef NATIVE_BUILD
// Native build simulation - no hardware dependencies
#include <iostream>

BleServer::BleServer() 
    : _connectionState(CONNECTION_DISCONNECTED)
    , _audioState(AUDIO_STATE_IDLE)
    , _advertising(false)
    , _currentMTU(BLE_MTU_SIZE)
    , _connCallback(nullptr)
    , _audioCallback(nullptr)
    , _chunkCallback(nullptr)
    , _currentClient(nullptr)
{
}

BleServer::~BleServer() {
    stopAdvertising();
}

bool BleServer::initialize(const char* deviceName) {
    // In native build, just initialize state
    (void)deviceName;  // Unused in native build
    _connectionState = CONNECTION_DISCONNECTED;
    _audioState = AUDIO_STATE_IDLE;
    _advertising = false;
    _currentMTU = BLE_MTU_SIZE;
    return true;
}

bool BleServer::advertise() {
    if (_advertising) {
        return true;  // Already advertising
    }
    
    _advertising = true;
    #ifdef NATIVE_BUILD
    std::cout << "[BLE Native] Advertising device: TWatch Audio" << std::endl;
    std::cout << "[BLE Native] Service UUID: " << getServiceUUID() << std::endl;
    #endif
    return true;
}

void BleServer::stopAdvertising() {
    if (!_advertising) {
        return;
    }
    
    _advertising = false;
    #ifdef NATIVE_BUILD
    std::cout << "[BLE Native] Stopped advertising" << std::endl;
    #endif
}

bool BleServer::sendChunk(const uint8_t* data, size_t length) {
    if (!_advertising) {
        return false;
    }
    
    if (_connectionState != CONNECTION_CONNECTED) {
        return false;
    }
    
    if (data == nullptr || length == 0) {
        return false;
    }
    
    if (length > MAX_AUDIO_CHUNK_SIZE) {
        length = MAX_AUDIO_CHUNK_SIZE;
    }
    
    #ifdef NATIVE_BUILD
    _simulateSendChunk(data, length);
    #endif
    
    return true;
}

bool BleServer::isConnected() const {
    return _connectionState == CONNECTION_CONNECTED;
}

ConnectionState BleServer::getConnectionState() const {
    return _connectionState;
}

AudioState BleServer::getAudioState() const {
    return _audioState;
}

void BleServer::setAudioState(AudioState state) {
    AudioState oldState = _audioState;
    _audioState = state;
    
    if (_audioCallback && oldState != state) {
        _audioCallback(state);
    }
    
    #ifdef NATIVE_BUILD
    std::cout << "[BLE Native] Audio state changed to: " << state << std::endl;
    #endif
}

void BleServer::setRecordingState(bool recording) {
    if (recording) {
        setAudioState(AUDIO_STATE_RECORDING);
    } else {
        setAudioState(AUDIO_STATE_IDLE);
    }
}

void BleServer::onConnectionChange(ConnectionCallback callback) {
    _connCallback = callback;
}

void BleServer::onAudioStateChange(AudioStateCallback callback) {
    _audioCallback = callback;
}

void BleServer::onChunkSent(ChunkSentCallback callback) {
    _chunkCallback = callback;
}

const char* BleServer::getServiceUUID() {
    return TWATCH_AUDIO_SERVICE_UUID;
}

size_t BleServer::getMTUSize() {
    return BLE_MTU_SIZE;
}

size_t BleServer::getMaxChunkSize() {
    return MAX_AUDIO_CHUNK_SIZE;
}

#ifdef NATIVE_BUILD
void BleServer::_simulateSendChunk(const uint8_t* data, size_t length) {
    // Simulate sending chunk in native build
    #ifdef NATIVE_BUILD
    std::cout << "[BLE Native] Sending chunk: " << length << " bytes" << std::endl;
    #endif
    
    if (_chunkCallback) {
        _chunkCallback(static_cast<uint16_t>(length));
    }
}
#endif
#else
// ESP32 BLE implementation

BleServer::BleServer()
    : _server(nullptr)
    , _audioService(nullptr)
    , _dataCharacteristic(nullptr)
    , _statusCharacteristic(nullptr)
    , _recordCharacteristic(nullptr)
    , _notifyCharacteristic(nullptr)
    , _serverCallbacks(this)
    , _notifyCallbacks(this)
    , _connectionState(CONNECTION_DISCONNECTED)
    , _audioState(AUDIO_STATE_IDLE)
    , _advertising(false)
    , _currentMTU(BLE_MTU_SIZE)
    , _currentClient(nullptr)
{
}

BleServer::~BleServer() {
    stopAdvertising();
    
    if (_server != nullptr) {
        BLEDevice::destroyServer(_server);
    }
}

bool BleServer::initialize(const char* deviceName) {
    // Initialize BLE device
    BLEDevice::init(deviceName);
    
    // Create server
    _server = BLEDevice::createServer();
    _server->setCallbacks(&_serverCallbacks);
    
    // Create audio service
    _audioService = BLEDevice::createService(TWATCH_AUDIO_SERVICE_UUID);
    
    // Create data characteristic (notify)
    _dataCharacteristic = _audioService->createCharacteristic(
        TWATCH_AUDIO_DATA_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    _dataCharacteristic->addDescriptor(new BLE2902());
    _dataCharacteristic->setCallbacks(&_notifyCallbacks);
    
    // Create status characteristic (read/write)
    _statusCharacteristic = _audioService->createCharacteristic(
        TWATCH_STATUS_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    
    // Create record characteristic (read/write)
    _recordCharacteristic = _audioService->createCharacteristic(
        TWATCH_RECORD_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    
    // Start service
    _audioService->start();
    
    // Create advertising
    BLEAdvertising* pAdvertising = BLEDevice::createAdvertising();
    pAdvertising->addServiceUUID(_audioService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    
    _server->getGapServer()->setMTU(BLE_MTU_SIZE);
    
    return true;
}

bool BleServer::advertise() {
    if (_advertising) {
        return true;
    }
    
    BLEDevice::startAdvertising();
    _advertising = true;
    return true;
}

void BleServer::stopAdvertising() {
    if (!_advertising) {
        return;
    }
    
    BLEDevice::getGapServer()->stopAdvertising();
    _advertising = false;
}

bool BleServer::sendChunk(const uint8_t* data, size_t length) {
    if (!_advertising) {
        return false;
    }
    
    if (_connectionState != CONNECTION_CONNECTED) {
        return false;
    }
    
    if (data == nullptr || length == 0) {
        return false;
    }
    
    if (length > MAX_AUDIO_CHUNK_SIZE) {
        length = MAX_AUDIO_CHUNK_SIZE;
    }
    
    // Send notification
    _dataCharacteristic->setValue(data, length);
    _dataCharacteristic->notify();
    
    return true;
}

bool BleServer::isConnected() const {
    return _connectionState == CONNECTION_CONNECTED;
}

ConnectionState BleServer::getConnectionState() const {
    return _connectionState;
}

AudioState BleServer::getAudioState() const {
    return _audioState;
}

void BleServer::setAudioState(AudioState state) {
    AudioState oldState = _audioState;
    _audioState = state;
    
    // Update status characteristic
    uint8_t statusValue = static_cast<uint8_t>(state);
    _statusCharacteristic->setValue(&statusValue, 1);
    
    if (_audioCallback && oldState != state) {
        _audioCallback(state);
    }
}

void BleServer::setRecordingState(bool recording) {
    if (recording) {
        setAudioState(AUDIO_STATE_RECORDING);
    } else {
        setAudioState(AUDIO_STATE_IDLE);
    }
}

void BleServer::onConnectionChange(ConnectionCallback callback) {
    _connCallback = callback;
}

void BleServer::onAudioStateChange(AudioStateCallback callback) {
    _audioCallback = callback;
}

void BleServer::onChunkSent(ChunkSentCallback callback) {
    _chunkCallback = callback;
}

const char* BleServer::getServiceUUID() {
    return TWATCH_AUDIO_SERVICE_UUID;
}

size_t BleServer::getMTUSize() {
    return BLE_MTU_SIZE;
}

size_t BleServer::getMaxChunkSize() {
    return MAX_AUDIO_CHUNK_SIZE;
}

// BLE Server Callbacks implementation
void BleServer::ServerCallbacks::onConnect(BLEClient* client) {
    _parent->_connectionState = CONNECTION_CONNECTED;
    _parent->_currentClient = client;
    
    if (_parent->_connCallback) {
        _parent->_connCallback(true);
    }
    
    #ifdef DEBUG
    Serial.println("[BLE] Client connected");
    #endif
}

void BleServer::ServerCallbacks::onDisconnect(BLEClient* client) {
    _parent->_connectionState = CONNECTION_DISCONNECTED;
    _parent->_currentClient = nullptr;
    
    if (_parent->_connCallback) {
        _parent->_connCallback(false);
    }
    
    #ifdef DEBUG
    Serial.println("[BLE] Client disconnected");
    #endif
}

// Notify Callbacks implementation
void BleServer::NotifyCallbacks::onWrite(BLECharacteristic* pCharacteristic) {
    // Handle write requests to notify characteristic
    // This can be used for control commands
    
    #ifdef DEBUG
    Serial.println("[BLE] Notify characteristic written");
    #endif
}
#endif
