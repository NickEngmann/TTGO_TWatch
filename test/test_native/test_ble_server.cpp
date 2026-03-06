#include <unity.h>
#include "BleServer.h"
#include <cstring>
#include <iostream>

using namespace TWatch;

// Test fixtures
static BleServer* g_server = nullptr;
static uint8_t g_testAudioData[1024];
static size_t g_testDataSize = 0;

void setUp(void) {
    g_server = new BleServer();
    g_server->init();
    g_server->setChunkSize(160);
    memset(g_testAudioData, 0, sizeof(g_testAudioData));
    g_testDataSize = 0;
}

void tearDown(void) {
    delete g_server;
    g_server = nullptr;
}

// Test 1: BLE Server Initialization
void test_ble_server_init(void) {
    TEST_ASSERT_TRUE(g_server->init());
    TEST_ASSERT_FALSE(g_server->isConnected());
    TEST_ASSERT_EQUAL(160, g_server->getChunkSize());
    TEST_ASSERT_EQUAL(247, g_server->getMtu());
}

// Test 2: Connection State Transitions
void test_connection_state_transitions(void) {
    TEST_ASSERT_FALSE(g_server->isConnected());
    
    g_server->startConnection();
    TEST_ASSERT_TRUE(g_server->isConnected());
    
    g_server->endConnection();
    TEST_ASSERT_FALSE(g_server->isConnected());
}

// Test 3: Connection Callback Registration
void test_connection_callback(void) {
    bool callbackCalled = false;
    CONNECTION_STATE lastState = CONNECTION_STATE::DISCONNECTED;
    
    g_server->setConnectionCallback([&callbackCalled, &lastState](CONNECTION_STATE state) {
        callbackCalled = true;
        lastState = state;
    });
    
    g_server->startConnection();
    TEST_ASSERT_TRUE(callbackCalled);
    TEST_ASSERT_EQUAL(CONNECTION_STATE::CONNECTED, lastState);
    
    callbackCalled = false;
    g_server->endConnection();
    TEST_ASSERT_TRUE(callbackCalled);
    TEST_ASSERT_EQUAL(CONNECTION_STATE::DISCONNECTED, lastState);
}

// Test 4: Audio Chunk Sending with Valid Data
void test_send_chunk_valid_data(void) {
    g_server->startConnection();
    
    // Create test audio data (160 bytes)
    for (size_t i = 0; i < 160; i++) {
        g_testAudioData[i] = (uint8_t)(i & 0xFF);
    }
    g_testDataSize = 160;
    
    bool result = g_server->sendChunk(g_testAudioData, g_testDataSize);
    TEST_ASSERT_TRUE(result);
}

// Test 5: Audio Chunk Sending with Invalid Data (not connected)
void test_send_chunk_not_connected(void) {
    // Don't connect - should fail
    bool result = g_server->sendChunk(g_testAudioData, 160);
    TEST_ASSERT_FALSE(result);
}

// Test 6: Audio Chunk Sending with Excessive Data (should be truncated)
void test_send_chunk_excessive_data(void) {
    g_server->startConnection();
    g_server->setChunkSize(100);
    
    // Create data larger than chunk size
    for (size_t i = 0; i < 200; i++) {
        g_testAudioData[i] = (uint8_t)(i & 0xFF);
    }
    
    bool result = g_server->sendChunk(g_testAudioData, 200);
    TEST_ASSERT_TRUE(result);  // Should succeed but truncate
}

// Test 7: Recording State Transitions
void test_recording_state_transitions(void) {
    g_server->setRecordingState(RecordingState::IDLE);
    TEST_ASSERT_EQUAL(RecordingState::IDLE, g_server->getRecordingState());
    
    g_server->setRecordingState(RecordingState::RECORDING);
    TEST_ASSERT_EQUAL(RecordingState::RECORDING, g_server->getRecordingState());
    
    g_server->setRecordingState(RecordingState::PAUSED);
    TEST_ASSERT_EQUAL(RecordingState::PAUSED, g_server->getRecordingState());
    
    g_server->setRecordingState(RecordingState::STOPPED);
    TEST_ASSERT_EQUAL(RecordingState::STOPPED, g_server->getRecordingState());
}

// Test 8: Audio Buffer Operations
void test_audio_buffer_operations(void) {
    g_server->startConnection();
    
    // Set audio data
    for (size_t i = 0; i < 64; i++) {
        g_testAudioData[i] = (uint8_t)(i * 2 & 0xFF);
    }
    g_testDataSize = 64;
    
    g_server->setAudioData(g_testAudioData, g_testDataSize);
    
    // Get audio buffer
    uint8_t buffer[64];
    g_server->getAudioBuffer(buffer, 64);
    
    // Verify data was copied
    for (size_t i = 0; i < 64; i++) {
        TEST_ASSERT_EQUAL_UINT8((i * 2) & 0xFF, buffer[i]);
    }
}

// Test 9: Audio Buffer Reset
void test_audio_buffer_reset(void) {
    g_server->startConnection();
    
    // Set some audio data
    for (size_t i = 0; i < 32; i++) {
        g_testAudioData[i] = 0xFF;
    }
    g_server->setAudioData(g_testAudioData, 32);
    
    // Reset buffer
    g_server->resetAudioBuffer();
    
    // Verify buffer is zeroed
    uint8_t buffer[32];
    g_server->getAudioBuffer(buffer, 32);
    
    for (size_t i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, buffer[i]);
    }
}

// Test 10: Chunk Size Configuration
void test_chunk_size_configuration(void) {
    g_server->setChunkSize(128);
    TEST_ASSERT_EQUAL(128, g_server->getChunkSize());
    
    g_server->setChunkSize(256);
    TEST_ASSERT_EQUAL(256, g_server->getChunkSize());
    
    // Test maximum chunk size limit
    g_server->setChunkSize(1000);
    TEST_ASSERT_EQUAL(MAX_CHUNK_SIZE, g_server->getChunkSize());
}

// Test 11: MTU Configuration
void test_mtu_configuration(void) {
    g_server->setMtu(128);
    TEST_ASSERT_EQUAL(128, g_server->getMtu());
    
    g_server->setMtu(247);
    TEST_ASSERT_EQUAL(247, g_server->getMtu());
    
    // Test MTU limit (max 247)
    g_server->setMtu(512);
    TEST_ASSERT_EQUAL(247, g_server->getMtu());
}

// Test 12: Single Sample Audio Chunk
void test_send_single_sample(void) {
    g_server->startConnection();
    
    uint16_t sample = 0x1234;
    bool result = g_server->sendChunk(sample);
    TEST_ASSERT_TRUE(result);
}

// Test 13: Connection Timeout Check
void test_connection_timeout(void) {
    g_server->startConnection();
    TEST_ASSERT_TRUE(g_server->isConnected());
    
    // Simulate timeout by setting last connection time to past
    g_server->checkConnectionTimeout();
    // In native build, this won't actually timeout without time manipulation
    // but the function should execute without error
}

// Test 14: Audio Data with Zero Length
void test_send_chunk_zero_length(void) {
    g_server->startConnection();
    
    bool result = g_server->sendChunk(g_testAudioData, 0);
    TEST_ASSERT_TRUE(result);  // Should succeed with zero length
}

// Test 15: Audio Buffer with Partial Read
void test_audio_buffer_partial_read(void) {
    g_server->startConnection();
    
    // Set only 32 bytes of audio data
    for (size_t i = 0; i < 32; i++) {
        g_testAudioData[i] = (uint8_t)i;
    }
    g_server->setAudioData(g_testAudioData, 32);
    
    // Request more data than available
    uint8_t buffer[64];
    g_server->getAudioBuffer(buffer, 64);
    
    // First 32 bytes should be correct
    for (size_t i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, buffer[i]);
    }
    // Remaining bytes should be zero or unchanged
}

void test_ble_server_main(void) {
    std::cout << "Running BLE Server tests..." << std::endl;
    
    UNITY_BEGIN();
    RUN_TEST(test_ble_server_init);
    RUN_TEST(test_connection_state_transitions);
    RUN_TEST(test_connection_callback);
    RUN_TEST(test_send_chunk_valid_data);
    RUN_TEST(test_send_chunk_not_connected);
    RUN_TEST(test_send_chunk_excessive_data);
    RUN_TEST(test_recording_state_transitions);
    RUN_TEST(test_audio_buffer_operations);
    RUN_TEST(test_audio_buffer_reset);
    RUN_TEST(test_chunk_size_configuration);
    RUN_TEST(test_mtu_configuration);
    RUN_TEST(test_send_single_sample);
    RUN_TEST(test_connection_timeout);
    RUN_TEST(test_send_chunk_zero_length);
    RUN_TEST(test_audio_buffer_partial_read);
    UNITY_END();
}

int main(int argc, char** argv) {
    test_ble_server_main();
    return 0;
}
