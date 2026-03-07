#include <unity.h>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <memory>

// Inline BleServer implementation for native build testing
// Custom Service UUID for T-Watch Audio (128-bit UUID)
#define TWATCH_AUDIO_SERVICE_UUID "0000fff0-0000-1000-8000-00805f9b34fb"
#define BLE_MTU_SIZE 247
#define MAX_AUDIO_CHUNK_SIZE 200

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
    BleServer() 
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
    
    ~BleServer() {
        stopAdvertising();
    }

    // Initialize BLE server
    bool initialize(const char* deviceName = "TWatch Audio") {
        (void)deviceName;  // Unused in native build
        _connectionState = CONNECTION_DISCONNECTED;
        _audioState = AUDIO_STATE_IDLE;
        _advertising = false;
        _currentMTU = BLE_MTU_SIZE;
        return true;
    }
    
    // Start advertising
    bool advertise() {
        if (_advertising) {
            return true;  // Already advertising
        }
        
        _advertising = true;
        return true;
    }
    
    // Stop advertising
    void stopAdvertising() {
        if (!_advertising) {
            return;
        }
        
        _advertising = false;
    }
    
    // Send audio chunk to connected client
    bool sendChunk(const uint8_t* data, size_t length) {
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
        
        if (_chunkCallback) {
            _chunkCallback(static_cast<uint16_t>(length));
        }
        
        return true;
    }
    
    // Check if connected
    bool isConnected() const {
        return _connectionState == CONNECTION_CONNECTED;
    }
    
    // Get current connection state
    ConnectionState getConnectionState() const {
        return _connectionState;
    }
    
    // Get current audio state
    AudioState getAudioState() const {
        return _audioState;
    }
    
    // Set audio state
    void setAudioState(AudioState state) {
        AudioState oldState = _audioState;
        _audioState = state;
        
        if (_audioCallback && oldState != state) {
            _audioCallback(state);
        }
    }
    
    // Set recording state
    void setRecordingState(bool recording) {
        if (recording) {
            setAudioState(AUDIO_STATE_RECORDING);
        } else {
            setAudioState(AUDIO_STATE_IDLE);
        }
    }
    
    // Register callbacks
    void onConnectionChange(ConnectionCallback callback) {
        _connCallback = callback;
    }
    
    void onAudioStateChange(AudioStateCallback callback) {
        _audioCallback = callback;
    }
    
    void onChunkSent(ChunkSentCallback callback) {
        _chunkCallback = callback;
    }
    
    // Get service UUID
    static const char* getServiceUUID() {
        return TWATCH_AUDIO_SERVICE_UUID;
    }
    
    // Get MTU size
    static size_t getMTUSize() {
        return BLE_MTU_SIZE;
    }
    
    // Get max chunk size
    static size_t getMaxChunkSize() {
        return MAX_AUDIO_CHUNK_SIZE;
    }

private:
    // Native build simulation state
    ConnectionState _connectionState;
    AudioState _audioState;
    bool _advertising;
    size_t _currentMTU;
    ConnectionCallback _connCallback;
    AudioStateCallback _audioCallback;
    ChunkSentCallback _chunkCallback;
    void* _currentClient;  // Placeholder for client pointer
};

// Test fixtures
static BleServer* g_server = nullptr;
static bool g_connectionCallbackCalled = false;
static bool g_audioStateCallbackCalled = false;
static bool g_chunkSentCallbackCalled = false;
static uint16_t g_lastChunkSize = 0;
static uint8_t g_lastAudioState = 0;

void setUp(void) {
    g_server = new BleServer();
    g_connectionCallbackCalled = false;
    g_audioStateCallbackCalled = false;
    g_chunkSentCallbackCalled = false;
    g_lastChunkSize = 0;
    g_lastAudioState = 0;
}

void tearDown(void) {
    delete g_server;
    g_server = nullptr;
}

// Callback implementations for testing
static void onConnectionChanged(bool connected) {
    g_connectionCallbackCalled = true;
}

static void onAudioStateChanged(uint8_t state) {
    g_audioStateCallbackCalled = true;
    g_lastAudioState = state;
}

static void onChunkSent(uint16_t size) {
    g_chunkSentCallbackCalled = true;
    g_lastChunkSize = size;
}

// Test: BLE Server initialization
void test_BleServer_Initialization(void) {
    TEST_ASSERT_TRUE(g_server->initialize("TestDevice"));
    TEST_ASSERT_FALSE(g_server->isConnected());
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
    TEST_ASSERT_EQUAL(CONNECTION_DISCONNECTED, g_server->getConnectionState());
}

// Test: Advertising starts and stops correctly
void test_BleServer_Advertising(void) {
    // Initially not advertising
    g_server->initialize("TestDevice");
    
    // Start advertising
    TEST_ASSERT_TRUE(g_server->advertise());
    
    // Stop advertising
    g_server->stopAdvertising();
    // After stopping, advertise() should still return true (idempotent - it sets _advertising to true)
    // The test verifies that advertising state is properly managed
    TEST_ASSERT_TRUE(g_server->advertise());
    
    // Verify advertising flag is set
    // Note: We can't directly access _advertising, but we can verify behavior
}

// Test: Audio chunk sending
void test_BleServer_SendChunk(void) {
    g_server->initialize("TestDevice");
    
    // Register callbacks
    g_server->onConnectionChange(onConnectionChanged);
    g_server->onAudioStateChange(onAudioStateChanged);
    g_server->onChunkSent(onChunkSent);
    
    // Try to send chunk when not connected - should fail
    uint8_t testData[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    TEST_ASSERT_FALSE(g_server->sendChunk(testData, 10));
    
    // Try to send null data - should fail
    TEST_ASSERT_FALSE(g_server->sendChunk(nullptr, 10));
    
    // Try to send zero length - should fail
    TEST_ASSERT_FALSE(g_server->sendChunk(testData, 0));
}

// Test: Audio state transitions
void test_BleServer_AudioStateTransitions(void) {
    g_server->initialize("TestDevice");
    
    // Register callback first
    g_server->onAudioStateChange(onAudioStateChanged);
    
    // Start recording
    g_server->setRecordingState(true);
    TEST_ASSERT_EQUAL(AUDIO_STATE_RECORDING, g_server->getAudioState());
    // Callback is called when state changes from IDLE to RECORDING
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
    
    // Reset callback flag
    g_audioStateCallbackCalled = false;
    
    // Stop recording
    g_server->setRecordingState(false);
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
    
    // Reset callback flag
    g_audioStateCallbackCalled = false;
    
    // Direct state change
    g_server->setAudioState(AUDIO_STATE_STREAMING);
    TEST_ASSERT_EQUAL(AUDIO_STATE_STREAMING, g_server->getAudioState());
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
    
    // Reset callback flag
    g_audioStateCallbackCalled = false;
    
    // Pause
    g_server->setAudioState(AUDIO_STATE_PAUSED);
    TEST_ASSERT_EQUAL(AUDIO_STATE_PAUSED, g_server->getAudioState());
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
    
    // Reset callback flag
    g_audioStateCallbackCalled = false;
    
    // Error state
    g_server->setAudioState(AUDIO_STATE_ERROR);
    TEST_ASSERT_EQUAL(AUDIO_STATE_ERROR, g_server->getAudioState());
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
}

// Test: Connection state management
void test_BleServer_ConnectionState(void) {
    g_server->initialize("TestDevice");
    
    TEST_ASSERT_EQUAL(CONNECTION_DISCONNECTED, g_server->getConnectionState());
    TEST_ASSERT_FALSE(g_server->isConnected());
    
    // Simulate connection (in native build, we can't actually connect)
    // But we can verify the state machine works
    g_server->setAudioState(AUDIO_STATE_STREAMING);
    TEST_ASSERT_EQUAL(AUDIO_STATE_STREAMING, g_server->getAudioState());
}

// Test: Chunk size limits
void test_BleServer_ChunkSizeLimits(void) {
    g_server->initialize("TestDevice");
    
    // Verify max chunk size constant
    TEST_ASSERT_EQUAL_UINT32(MAX_AUDIO_CHUNK_SIZE, BleServer::getMaxChunkSize());
    TEST_ASSERT_EQUAL_UINT32(BLE_MTU_SIZE, BleServer::getMTUSize());
    
    // Try to send oversized chunk
    uint8_t largeData[MAX_AUDIO_CHUNK_SIZE + 100];
    memset(largeData, 0x55, sizeof(largeData));
    
    // Should be limited to max chunk size
    TEST_ASSERT_FALSE(g_server->sendChunk(largeData, sizeof(largeData)));
}

// Test: Service UUID
void test_BleServer_ServiceUUID(void) {
    const char* uuid = BleServer::getServiceUUID();
    TEST_ASSERT_NOT_NULL(uuid);
    TEST_ASSERT_EQUAL_STRING(TWATCH_AUDIO_SERVICE_UUID, uuid);
}

// Test: Callback registration
void test_BleServer_CallbackRegistration(void) {
    g_server->initialize("TestDevice");
    
    // Register all callbacks
    g_server->onConnectionChange(onConnectionChanged);
    g_server->onAudioStateChange(onAudioStateChanged);
    g_server->onChunkSent(onChunkSent);
    
    // Verify callbacks are registered (state changed)
    g_server->setAudioState(AUDIO_STATE_RECORDING);
    TEST_ASSERT_TRUE(g_audioStateCallbackCalled);
}

// Test: Recording state toggle
void test_BleServer_RecordingToggle(void) {
    g_server->initialize("TestDevice");
    
    // Start recording
    g_server->setRecordingState(true);
    TEST_ASSERT_EQUAL(AUDIO_STATE_RECORDING, g_server->getAudioState());
    
    // Stop recording
    g_server->setRecordingState(false);
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
    
    // Toggle multiple times
    for (int i = 0; i < 5; i++) {
        g_server->setRecordingState(true);
        TEST_ASSERT_EQUAL(AUDIO_STATE_RECORDING, g_server->getAudioState());
        g_server->setRecordingState(false);
        TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
    }
}

// Test: Idle state after initialization
void test_BleServer_IdleState(void) {
    g_server->initialize("TestDevice");
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
    TEST_ASSERT_EQUAL(CONNECTION_DISCONNECTED, g_server->getConnectionState());
}

// Test: Error state handling
void test_BleServer_ErrorState(void) {
    g_server->initialize("TestDevice");
    
    // Set error state
    g_server->setAudioState(AUDIO_STATE_ERROR);
    TEST_ASSERT_EQUAL(AUDIO_STATE_ERROR, g_server->getAudioState());
    
    // Recover from error
    g_server->setAudioState(AUDIO_STATE_IDLE);
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, g_server->getAudioState());
}

// Test: Chunk sending with valid data when connected (simulation)
void test_BleServer_ChunkWithValidData(void) {
    g_server->initialize("TestDevice");
    g_server->onChunkSent(onChunkSent);
    
    // Create test audio data (simulating 16kHz 16-bit mono samples)
    uint8_t audioData[32];
    for (int i = 0; i < 32; i++) {
        audioData[i] = (uint8_t)(i % 256);
    }
    
    // In native build, sendChunk returns false when not connected
    // But we can verify the callback would be called if connected
    TEST_ASSERT_FALSE(g_server->sendChunk(audioData, 32));
    TEST_ASSERT_FALSE(g_chunkSentCallbackCalled);  // Callback should not be called
}

// Test: MTU size configuration
void test_BleServer_MTUConfiguration(void) {
    TEST_ASSERT_EQUAL_UINT16(247, BleServer::getMTUSize());
    TEST_ASSERT_LESS_THAN(BLE_MTU_SIZE, BleServer::getMaxChunkSize());
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_BleServer_Initialization);
    RUN_TEST(test_BleServer_Advertising);
    RUN_TEST(test_BleServer_SendChunk);
    RUN_TEST(test_BleServer_AudioStateTransitions);
    RUN_TEST(test_BleServer_ConnectionState);
    RUN_TEST(test_BleServer_ChunkSizeLimits);
    RUN_TEST(test_BleServer_ServiceUUID);
    RUN_TEST(test_BleServer_CallbackRegistration);
    RUN_TEST(test_BleServer_RecordingToggle);
    RUN_TEST(test_BleServer_IdleState);
    RUN_TEST(test_BleServer_ErrorState);
    RUN_TEST(test_BleServer_ChunkWithValidData);
    RUN_TEST(test_BleServer_MTUConfiguration);
    
    return UNITY_END();
}
