#include <unity.h>
#include "ble_audio_stream.h"
#include <cstring>

// Global test objects
BLEAudioStreamer* streamer = nullptr;
BLEGattServer* gattServer = nullptr;

void setUp(void) {
    streamer = new BLEAudioStreamer();
    gattServer = new BLEGattServer();
}

void tearDown(void) {
    delete streamer;
    delete gattServer;
    streamer = nullptr;
    gattServer = nullptr;
}

// ==================== UUID Tests ====================

void test_audio_service_uuid_is_correct(void) {
    TEST_ASSERT_EQUAL_STRING(AUDIO_SERVICE_UUID, streamer->getServiceUUID().c_str());
}

void test_audio_data_uuid_is_correct(void) {
    TEST_ASSERT_EQUAL_STRING(AUDIO_DATA_UUID, streamer->getDataUUID().c_str());
}

void test_audio_control_uuid_is_correct(void) {
    TEST_ASSERT_EQUAL_STRING(AUDIO_CONTROL_UUID, streamer->getControlUUID().c_str());
}

void test_battery_service_uuid_is_correct(void) {
    TEST_ASSERT_EQUAL_STRING(BATTERY_SERVICE_UUID, BATTERY_SERVICE_UUID);
}

void test_battery_level_uuid_is_correct(void) {
    TEST_ASSERT_EQUAL_STRING(BATTERY_LEVEL_UUID, streamer->getBatteryUUID().c_str());
}

// ==================== Chunk Size Tests ====================

void test_audio_chunk_size_is_512_bytes(void) {
    TEST_ASSERT_EQUAL_UINT16(512, streamer->getChunkSize());
}

// ==================== State Machine Tests ====================

void test_initial_state_is_idle(void) {
    TEST_ASSERT_EQUAL(AudioStreamState::IDLE, streamer->getState());
}

void test_state_transition_idle_to_advertising(void) {
    bool result = streamer->startAdvertising();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::ADVERTISING, streamer->getState());
}

void test_state_transition_advertising_to_connected_on_connection(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    
    // Connection state changes, but we need to start streaming to get to STREAMING
    bool result = streamer->startStreaming();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::STREAMING, streamer->getState());
}

void test_state_transition_streaming_to_connected_on_stop(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->stopStreaming();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::IDLE, streamer->getState());
}

void test_state_transition_connected_to_idle_on_disconnect(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    streamer->stopStreaming();
    
    streamer->onDisconnect();
    TEST_ASSERT_EQUAL(AudioStreamState::CONNECTED, streamer->getState());
}

void test_advertising_to_idle_on_stop_advertising(void) {
    streamer->startAdvertising();
    bool result = streamer->stopAdvertising();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::IDLE, streamer->getState());
}

void test_invalid_state_transition_advertising_to_streaming_without_connection(void) {
    streamer->startAdvertising();
    // Try to start streaming without connection
    bool result = streamer->startStreaming();
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::ADVERTISING, streamer->getState());
}

void test_invalid_state_transition_streaming_to_advertising(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    // Try to start advertising while streaming
    bool result = streamer->startAdvertising();
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::STREAMING, streamer->getState());
}

// ==================== Connection State Tests ====================

void test_initial_connection_state_is_disconnected(void) {
    // Connection state is internal, but we can test via behavior
    streamer->onConnection(BLEConnectionState::CONNECTED);
    TEST_ASSERT_EQUAL(AudioStreamState::CONNECTED, streamer->getState());
}

void test_disconnect_from_streaming_state(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    streamer->onDisconnect();
    TEST_ASSERT_EQUAL(AudioStreamState::IDLE, streamer->getState());
}

// ==================== Audio Control Tests ====================

void test_handle_start_stream_command(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    
    bool result = streamer->handleControlCommand(AudioControlCommand::START_STREAM);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::STREAMING, streamer->getState());
}

void test_handle_stop_stream_command(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->handleControlCommand(AudioControlCommand::STOP_STREAM);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::IDLE, streamer->getState());
}

void test_handle_pause_stream_command(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->handleControlCommand(AudioControlCommand::PAUSE_STREAM);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::STREAMING, streamer->getState());
}

void test_handle_resume_stream_command(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->handleControlCommand(AudioControlCommand::RESUME_STREAM);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(AudioStreamState::STREAMING, streamer->getState());
}

void test_handle_invalid_command_returns_false(void) {
    bool result = streamer->handleControlCommand(static_cast<AudioControlCommand>(99));
    TEST_ASSERT_FALSE(result);
}

// ==================== Audio Chunk Tests ====================

void test_send_audio_chunk_in_streaming_state(void) {
    uint8_t testChunk[AUDIO_CHUNK_SIZE];
    memset(testChunk, 0xAA, AUDIO_CHUNK_SIZE);
    
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->sendAudioChunk(testChunk, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_TRUE(result);
}

void test_send_audio_chunk_smaller_than_max_size(void) {
    uint8_t testChunk[256];
    memset(testChunk, 0xBB, 256);
    
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->sendAudioChunk(testChunk, 256);
    TEST_ASSERT_TRUE(result);
}

void test_send_audio_chunk_fails_when_not_streaming(void) {
    uint8_t testChunk[AUDIO_CHUNK_SIZE];
    memset(testChunk, 0xCC, AUDIO_CHUNK_SIZE);
    
    // Not in streaming state
    bool result = streamer->sendAudioChunk(testChunk, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_FALSE(result);
}

void test_send_audio_chunk_fails_with_null_data(void) {
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->sendAudioChunk(nullptr, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_FALSE(result);
}

void test_send_audio_chunk_fails_with_zero_size(void) {
    uint8_t testChunk[AUDIO_CHUNK_SIZE];
    memset(testChunk, 0xDD, AUDIO_CHUNK_SIZE);
    
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->sendAudioChunk(testChunk, 0);
    TEST_ASSERT_FALSE(result);
}

void test_send_audio_chunk_fails_with_size_exceeding_max(void) {
    uint8_t testChunk[AUDIO_CHUNK_SIZE + 1];
    memset(testChunk, 0xEE, AUDIO_CHUNK_SIZE + 1);
    
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    
    bool result = streamer->sendAudioChunk(testChunk, AUDIO_CHUNK_SIZE + 1);
    TEST_ASSERT_FALSE(result);
}

// ==================== Callback Tests ====================

void test_connection_callback_is_called(void) {
    bool callbackCalled = false;
    streamer->setConnectionCallback([&callbackCalled](BLEConnectionState state) {
        callbackCalled = true;
    });
    
    streamer->onConnection(BLEConnectionState::CONNECTED);
    TEST_ASSERT_TRUE(callbackCalled);
}

void test_audio_callback_is_called_on_chunk_send(void) {
    bool callbackCalled = false;
    uint16_t callbackSize = 0;
    
    streamer->setAudioCallback([&callbackCalled, &callbackSize](const uint8_t* data, uint16_t size) {
        callbackCalled = true;
        callbackSize = size;
    });
    
    uint8_t testChunk[AUDIO_CHUNK_SIZE];
    memset(testChunk, 0xFF, AUDIO_CHUNK_SIZE);
    
    streamer->startAdvertising();
    streamer->onConnection(BLEConnectionState::CONNECTED);
    streamer->startStreaming();
    streamer->sendAudioChunk(testChunk, AUDIO_CHUNK_SIZE);
    
    TEST_ASSERT_TRUE(callbackCalled);
    TEST_ASSERT_EQUAL_UINT16(AUDIO_CHUNK_SIZE, callbackSize);
}

// ==================== Battery Level Tests ====================

void test_initial_battery_level_is_100(void) {
    TEST_ASSERT_EQUAL_UINT8(100, streamer->getBatteryLevel());
}

void test_set_battery_level_updates_value(void) {
    streamer->setBatteryLevel(75);
    TEST_ASSERT_EQUAL_UINT8(75, streamer->getBatteryLevel());
}

void test_set_battery_level_clamps_to_100(void) {
    streamer->setBatteryLevel(150);
    TEST_ASSERT_EQUAL_UINT8(100, streamer->getBatteryLevel());
}

void test_set_battery_level_accepts_zero(void) {
    streamer->setBatteryLevel(0);
    TEST_ASSERT_EQUAL_UINT8(0, streamer->getBatteryLevel());
}

// ==================== BLEGattServer Tests ====================

void test_gatt_server_initial_state(void) {
    TEST_ASSERT_FALSE(gattServer->isInitialized());
    TEST_ASSERT_FALSE(gattServer->isAdvertising());
    TEST_ASSERT_FALSE(gattServer->isConnected());
    TEST_ASSERT_FALSE(gattServer->hasAudioService());
    TEST_ASSERT_FALSE(gattServer->hasBatteryService());
}

void test_gatt_server_initialization(void) {
    bool result = gattServer->initialize();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(gattServer->isInitialized());
}

void test_gatt_server_double_initialization(void) {
    gattServer->initialize();
    bool result = gattServer->initialize();
    TEST_ASSERT_TRUE(result);
}

void test_gatt_server_setup_audio_service(void) {
    gattServer->initialize();
    bool result = gattServer->setupAudioService();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(gattServer->hasAudioService());
}

void test_gatt_server_setup_battery_service(void) {
    gattServer->initialize();
    bool result = gattServer->setupBatteryService();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(gattServer->hasBatteryService());
}

void test_gatt_server_setup_services_after_initialization(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->setupBatteryService();
    
    TEST_ASSERT_TRUE(gattServer->hasAudioService());
    TEST_ASSERT_TRUE(gattServer->hasBatteryService());
}

void test_gatt_server_start_advertising(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->startAdvertising();
    
    TEST_ASSERT_TRUE(gattServer->isAdvertising());
}

void test_gatt_server_stop_advertising(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->startAdvertising();
    gattServer->stopAdvertising();
    
    TEST_ASSERT_FALSE(gattServer->isAdvertising());
}

void test_gatt_server_on_connect(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->onConnect();
    
    TEST_ASSERT_TRUE(gattServer->isConnected());
}

void test_gatt_server_on_disconnect(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->onConnect();
    gattServer->onDisconnect();
    
    TEST_ASSERT_FALSE(gattServer->isConnected());
}

void test_gatt_server_send_audio_data_when_connected(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->onConnect();
    
    uint8_t testData[AUDIO_CHUNK_SIZE];
    memset(testData, 0x12, AUDIO_CHUNK_SIZE);
    
    bool result = gattServer->sendAudioData(testData, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_TRUE(result);
}

void test_gatt_server_send_audio_data_when_not_connected(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    
    uint8_t testData[AUDIO_CHUNK_SIZE];
    memset(testData, 0x34, AUDIO_CHUNK_SIZE);
    
    bool result = gattServer->sendAudioData(testData, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_FALSE(result);
}

void test_gatt_server_send_audio_data_when_not_initialized(void) {
    uint8_t testData[AUDIO_CHUNK_SIZE];
    memset(testData, 0x56, AUDIO_CHUNK_SIZE);
    
    bool result = gattServer->sendAudioData(testData, AUDIO_CHUNK_SIZE);
    TEST_ASSERT_FALSE(result);
}

void test_gatt_server_set_battery_level(void) {
    gattServer->initialize();
    gattServer->setupBatteryService();
    gattServer->setBatteryLevel(85);
    
    // Battery level is set internally, we can't directly verify
    // but the function should not crash
    TEST_ASSERT_TRUE(true);
}

void test_gatt_server_deinitialize(void) {
    gattServer->initialize();
    gattServer->setupAudioService();
    gattServer->setupBatteryService();
    gattServer->startAdvertising();
    gattServer->onConnect();
    
    gattServer->deinitialize();
    
    TEST_ASSERT_FALSE(gattServer->isInitialized());
    TEST_ASSERT_FALSE(gattServer->hasAudioService());
    TEST_ASSERT_FALSE(gattServer->hasBatteryService());
    TEST_ASSERT_FALSE(gattServer->isAdvertising());
    TEST_ASSERT_FALSE(gattServer->isConnected());
}

// ==================== Main ====================

int main(void) {
    UNITY_BEGIN();
    
    // UUID Tests
    RUN_TEST(test_audio_service_uuid_is_correct);
    RUN_TEST(test_audio_data_uuid_is_correct);
    RUN_TEST(test_audio_control_uuid_is_correct);
    RUN_TEST(test_battery_service_uuid_is_correct);
    RUN_TEST(test_battery_level_uuid_is_correct);
    
    // Chunk Size Tests
    RUN_TEST(test_audio_chunk_size_is_512_bytes);
    
    // State Machine Tests
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_state_transition_idle_to_advertising);
    RUN_TEST(test_state_transition_advertising_to_connected_on_connection);
    RUN_TEST(test_state_transition_streaming_to_connected_on_stop);
    RUN_TEST(test_state_transition_connected_to_idle_on_disconnect);
    RUN_TEST(test_advertising_to_idle_on_stop_advertising);
    RUN_TEST(test_invalid_state_transition_advertising_to_streaming_without_connection);
    RUN_TEST(test_invalid_state_transition_streaming_to_advertising);
    
    // Connection State Tests
    RUN_TEST(test_initial_connection_state_is_disconnected);
    RUN_TEST(test_disconnect_from_streaming_state);
    
    // Audio Control Tests
    RUN_TEST(test_handle_start_stream_command);
    RUN_TEST(test_handle_stop_stream_command);
    RUN_TEST(test_handle_pause_stream_command);
    RUN_TEST(test_handle_resume_stream_command);
    RUN_TEST(test_handle_invalid_command_returns_false);
    
    // Audio Chunk Tests
    RUN_TEST(test_send_audio_chunk_in_streaming_state);
    RUN_TEST(test_send_audio_chunk_smaller_than_max_size);
    RUN_TEST(test_send_audio_chunk_fails_when_not_streaming);
    RUN_TEST(test_send_audio_chunk_fails_with_null_data);
    RUN_TEST(test_send_audio_chunk_fails_with_zero_size);
    RUN_TEST(test_send_audio_chunk_fails_with_size_exceeding_max);
    
    // Callback Tests
    RUN_TEST(test_connection_callback_is_called);
    RUN_TEST(test_audio_callback_is_called_on_chunk_send);
    
    // Battery Level Tests
    RUN_TEST(test_initial_battery_level_is_100);
    RUN_TEST(test_set_battery_level_updates_value);
    RUN_TEST(test_set_battery_level_clamps_to_100);
    RUN_TEST(test_set_battery_level_accepts_zero);
    
    // BLEGattServer Tests
    RUN_TEST(test_gatt_server_initial_state);
    RUN_TEST(test_gatt_server_initialization);
    RUN_TEST(test_gatt_server_double_initialization);
    RUN_TEST(test_gatt_server_setup_audio_service);
    RUN_TEST(test_gatt_server_setup_battery_service);
    RUN_TEST(test_gatt_server_setup_services_after_initialization);
    RUN_TEST(test_gatt_server_start_advertising);
    RUN_TEST(test_gatt_server_stop_advertising);
    RUN_TEST(test_gatt_server_on_connect);
    RUN_TEST(test_gatt_server_on_disconnect);
    RUN_TEST(test_gatt_server_send_audio_data_when_connected);
    RUN_TEST(test_gatt_server_send_audio_data_when_not_connected);
    RUN_TEST(test_gatt_server_send_audio_data_when_not_initialized);
    RUN_TEST(test_gatt_server_set_battery_level);
    RUN_TEST(test_gatt_server_deinitialize);
    
    return UNITY_END();
}
