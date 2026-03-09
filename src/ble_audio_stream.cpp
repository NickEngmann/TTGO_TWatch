#include "ble_audio_stream.h"
#include <cstring>

// BLEAudioStreamer implementation

BLEAudioStreamer::BLEAudioStreamer()
    : currentState(AudioStreamState::IDLE)
    , connectionState(BLEConnectionState::DISCONNECTED)
    , batteryLevel(100)
    , serviceUUID(AUDIO_SERVICE_UUID)
    , dataUUID(AUDIO_DATA_UUID)
    , controlUUID(AUDIO_CONTROL_UUID)
    , batteryUUID(BATTERY_LEVEL_UUID)
    , connectionCallback(nullptr)
    , audioCallback(nullptr)
{
}

BLEAudioStreamer::~BLEAudioStreamer() {
    // Cleanup if needed
}

AudioStreamState BLEAudioStreamer::getState() const {
    return currentState;
}

bool BLEAudioStreamer::startAdvertising() {
    if (!validateStateTransition(AudioStreamState::ADVERTISING)) {
        return false;
    }
    updateState(AudioStreamState::ADVERTISING);
    return true;
}

bool BLEAudioStreamer::stopAdvertising() {
    if (currentState != AudioStreamState::ADVERTISING) {
        return false;
    }
    // Advertising stopped, go back to IDLE
    updateState(AudioStreamState::IDLE);
    return true;
}

void BLEAudioStreamer::onConnection(BLEConnectionState state) {
    connectionState = state;
    // Transition to CONNECTED state when receiving connection while advertising
    if (state == BLEConnectionState::CONNECTED && currentState == AudioStreamState::ADVERTISING) {
        updateState(AudioStreamState::CONNECTED);
    }
    if (connectionCallback) {
        connectionCallback(state);
    }
}

void BLEAudioStreamer::onDisconnect() {
    connectionState = BLEConnectionState::DISCONNECTED;
    if (currentState == AudioStreamState::STREAMING || currentState == AudioStreamState::CONNECTED) {
        updateState(AudioStreamState::IDLE);
    }
}

bool BLEAudioStreamer::startStreaming() {
    if (!isStreamingReady()) {
        return false;
    }
    updateState(AudioStreamState::STREAMING);
    return true;
}

bool BLEAudioStreamer::stopStreaming() {
    if (currentState != AudioStreamState::STREAMING) {
        return false;
    }
    updateState(AudioStreamState::CONNECTED);
    return true;
}

bool BLEAudioStreamer::pauseStreaming() {
    if (currentState != AudioStreamState::STREAMING) {
        return false;
    }
    // Pause doesn't change state, just stops sending data
    return true;
}

bool BLEAudioStreamer::resumeStreaming() {
    if (currentState != AudioStreamState::STREAMING) {
        return false;
    }
    return true;
}

bool BLEAudioStreamer::sendAudioChunk(const uint8_t* data, uint16_t size) {
    if (currentState != AudioStreamState::STREAMING) {
        return false;
    }
    if (!data || size == 0) {
        return false;
    }
    
    // Validate chunk size (should be <= AUDIO_CHUNK_SIZE)
    if (size > AUDIO_CHUNK_SIZE) {
        return false;
    }
    
    // Call audio callback if set
    if (audioCallback) {
        audioCallback(data, size);
    }
    
    return true;
}

bool BLEAudioStreamer::handleControlCommand(AudioControlCommand cmd) {
    switch (cmd) {
        case AudioControlCommand::START_STREAM:
            return startStreaming();
        case AudioControlCommand::STOP_STREAM:
            return stopStreaming();
        case AudioControlCommand::PAUSE_STREAM:
            return pauseStreaming();
        case AudioControlCommand::RESUME_STREAM:
            return resumeStreaming();
        default:
            return false;
    }
}

void BLEAudioStreamer::setBatteryLevel(uint8_t level) {
    if (level > 100) {
        level = 100;
    }
    batteryLevel = level;
}

uint8_t BLEAudioStreamer::getBatteryLevel() const {
    return batteryLevel;
}

void BLEAudioStreamer::setConnectionCallback(ConnectionCallback cb) {
    connectionCallback = cb;
}

void BLEAudioStreamer::setAudioCallback(AudioCallback cb) {
    audioCallback = cb;
}

bool BLEAudioStreamer::validateStateTransition(AudioStreamState newState) {
    switch (newState) {
        case AudioStreamState::ADVERTISING:
            return currentState == AudioStreamState::IDLE;
        case AudioStreamState::CONNECTED:
            return currentState == AudioStreamState::ADVERTISING && 
                   connectionState == BLEConnectionState::CONNECTED;
        case AudioStreamState::STREAMING:
            return currentState == AudioStreamState::CONNECTED;
        default:
            return false;
    }
}

void BLEAudioStreamer::updateState(AudioStreamState newState) {
    currentState = newState;
}

bool BLEAudioStreamer::isStreamingReady() const {
    return currentState == AudioStreamState::CONNECTED && 
           connectionState == BLEConnectionState::CONNECTED;
}

// BLEGattServer implementation

BLEGattServer::BLEGattServer()
    : initialized(false)
    , audioServiceSetup(false)
    , batteryServiceSetup(false)
    , advertising(false)
    , connected(false)
    , server(nullptr)
    , audioService(nullptr)
    , batteryService(nullptr)
    , audioDataChar(nullptr)
    , audioControlChar(nullptr)
    , batteryLevelChar(nullptr)
{
}

BLEGattServer::~BLEGattServer() {
    deinitialize();
}

bool BLEGattServer::initialize() {
    if (initialized) {
        return true;
    }

#ifndef NATIVE_BUILD
    // Initialize BLE server on hardware
    server = new NimBLEServer();
    if (!server) {
        return false;
    }
#endif

    initialized = true;
    return true;
}

void BLEGattServer::deinitialize() {
    if (!initialized) {
        return;
    }

#ifndef NATIVE_BUILD
    if (audioService) {
        delete audioService;
    }
    if (batteryService) {
        delete batteryService;
    }
    if (server) {
        delete server;
    }
#endif

    initialized = false;
    audioServiceSetup = false;
    batteryServiceSetup = false;
    advertising = false;
    connected = false;
}

bool BLEGattServer::setupAudioService() {
    if (!initialized) {
        return false;
    }
    if (audioServiceSetup) {
        return true;
    }

#ifndef NATIVE_BUILD
    audioService = server->createService(AUDIO_SERVICE_UUID);
    if (!audioService) {
        return false;
    }

    audioDataChar = new NimBLECharacteristic(AUDIO_DATA_UUID,
        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ,
        AUDIO_CHUNK_SIZE);
    audioDataChar->addDescriptor(new NimBLEDescriptor(
        BLE2902_UUID, "Audio Data"));
    audioService->addCharacteristic(audioDataChar);

    audioControlChar = new NimBLECharacteristic(AUDIO_CONTROL_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ,
        1);
    audioControlChar->addDescriptor(new NimBLEDescriptor(
        BLE2902_UUID, "Audio Control"));
    audioService->addCharacteristic(audioControlChar);

    server->addService(audioService);
#endif

    audioServiceSetup = true;
    return true;
}

bool BLEGattServer::setupBatteryService() {
    if (!initialized) {
        return false;
    }
    if (batteryServiceSetup) {
        return true;
    }

#ifndef NATIVE_BUILD
    batteryService = server->createService(BATTERY_SERVICE_UUID);
    if (!batteryService) {
        return false;
    }

    batteryLevelChar = new NimBLECharacteristic(BATTERY_LEVEL_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY,
        1);
    batteryLevelChar->addDescriptor(new NimBLEDescriptor(
        BLE2902_UUID, "Battery Level"));
    batteryService->addCharacteristic(batteryLevelChar);

    server->addService(batteryService);
#endif

    batteryServiceSetup = true;
    return true;
}

void BLEGattServer::startAdvertising() {
    if (!initialized || !audioServiceSetup) {
        return;
    }

#ifndef NATIVE_BUILD
    NimBLEDevice::startAdvertising();
#endif

    advertising = true;
}

void BLEGattServer::stopAdvertising() {
    advertising = false;
}

void BLEGattServer::onConnect() {
    connected = true;
}

void BLEGattServer::onDisconnect() {
    connected = false;
}

bool BLEGattServer::sendAudioData(const uint8_t* data, uint16_t size) {
    if (!connected || !audioServiceSetup) {
        return false;
    }

#ifndef NATIVE_BUILD
    if (audioDataChar) {
        audioDataChar->setValue(data, size);
        audioDataChar->notify();
    }
#endif

    return true;
}

void BLEGattServer::setBatteryLevel(uint8_t level) {
    if (batteryServiceSetup) {
#ifndef NATIVE_BUILD
        if (batteryLevelChar) {
            uint8_t batteryData = level;
            batteryLevelChar->setValue(&batteryData, 1);
            batteryLevelChar->notify();
        }
#endif
    }
}
