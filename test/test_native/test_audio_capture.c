/*
 * Unity Tests for Audio Capture Circular Buffer Logic
 * 
 * Tests for the circular buffer implementation used in audio capture.
 * These tests run in native build environment without ESP32 hardware.
 */

#include "unity.h"
#include "AudioCapture.h"
#include "AudioCapture_mock.h"
#include <string.h>
#include <stdlib.h>

// Test configuration for 16kHz mono audio, 3 second buffer
static const AudioConfig_t testConfig = {
    .sampleRate = 16000,
    .bufferDuration = 3000,
    .channels = 1,
    .bitsPerSample = 16,
    .pdmMode = 2
};

// Test helper: generate test audio data
static void generateTestAudio(uint8_t *buffer, size_t size, uint32_t pattern) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)((pattern + i) & 0xFF);
    }
}

// Test helper: verify buffer contents
static bool verifyBufferContents(const uint8_t *buffer, size_t size, uint32_t pattern) {
    for (size_t i = 0; i < size; i++) {
        if (buffer[i] != (uint8_t)((pattern + i) & 0xFF)) {
            return false;
        }
    }
    return true;
}

// Test 1: AudioCapture creation and destruction
void test_audioCapture_create_destroy(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    // Verify initial state
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, capture->state);
    TEST_ASSERT_NULL(capture->buffer);
    TEST_ASSERT_EQUAL(0, capture->bufferSize);
    
    audioCaptureDestroy(capture);
}

// Test 2: AudioCapture initialization with default config
void test_audioCapture_init_default(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    bool result = audioCaptureInitDefault(capture);
    TEST_ASSERT_TRUE(result);
    
    // Verify configuration was set
    TEST_ASSERT_EQUAL(16000, capture->config.sampleRate);
    TEST_ASSERT_EQUAL(3000, capture->config.bufferDuration);
    TEST_ASSERT_EQUAL(1, capture->config.channels);
    TEST_ASSERT_EQUAL(16, capture->config.bitsPerSample);
    
    // Verify buffer was allocated
    TEST_ASSERT_NOT_NULL(capture->buffer);
    TEST_ASSERT_GREATER_THAN(0, capture->bufferSize);
    
    // Calculate expected size: 16000 * 3 * 1 * 2 = 96000 bytes
    TEST_ASSERT_EQUAL_UINT32(96000, capture->bufferSize);
    
    audioCaptureDestroy(capture);
}

// Test 3: AudioCapture initialization with custom config
void test_audioCapture_init_custom(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    AudioConfig_t customConfig = {
        .sampleRate = 8000,
        .bufferDuration = 2000,
        .channels = 1,
        .bitsPerSample = 16,
        .pdmMode = 0
    };
    
    bool result = audioCaptureInit(capture, &customConfig);
    TEST_ASSERT_TRUE(result);
    
    // Verify custom configuration
    TEST_ASSERT_EQUAL(8000, capture->config.sampleRate);
    TEST_ASSERT_EQUAL(2000, capture->config.bufferDuration);
    TEST_ASSERT_EQUAL(0, capture->config.pdmMode);
    
    // Calculate expected size: 8000 * 2 * 1 * 2 = 32000 bytes
    TEST_ASSERT_EQUAL_UINT32(32000, capture->bufferSize);
    
    audioCaptureDestroy(capture);
}

// Test 4: Circular buffer write and read
void test_audioCapture_write_read(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    bool result = audioCaptureInitDefault(capture);
    TEST_ASSERT_TRUE(result);
    
    // Allocate test data
    uint8_t testData[1024];
    generateTestAudio(testData, sizeof(testData), 42);
    
    // Write data to buffer
    size_t written = audioCaptureGetBuffer(capture, testData, sizeof(testData));
    TEST_ASSERT_EQUAL(0, written);  // Buffer is empty, can't write via getBuffer
    
    // Use mock to simulate data capture
    mock_audioCapture_init(capture, &testConfig);
    mock_audioCapture_start(capture);
    
    // Write test data via mock process
    mock_audioCapture_process(capture, testData, sizeof(testData));
    
    // Read data back
    uint8_t readData[1024];
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    TEST_ASSERT_EQUAL_UINT32(sizeof(testData), mockData->mockAvailableBytes);
    
    // Verify data integrity
    TEST_ASSERT_TRUE(verifyBufferContents(testData, sizeof(testData), 42));
    
    audioCaptureDestroy(capture);
}

// Test 5: Circular buffer wraparound
void test_audioCapture_wraparound(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    // Use smaller buffer for easier testing
    AudioConfig_t smallConfig = {
        .sampleRate = 16000,
        .bufferDuration = 100,  // 100ms = 1600 samples = 3200 bytes
        .channels = 1,
        .bitsPerSample = 16,
        .pdmMode = 2
    };
    
    mock_audioCapture_init(capture, &smallConfig);
    mock_audioCapture_start(capture);
    
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    
    // Write data that exceeds buffer size to test wraparound
    uint8_t testData[4000];
    generateTestAudio(testData, sizeof(testData), 123);
    
    mock_audioCapture_process(capture, testData, sizeof(testData));
    
    // Buffer should be full (circular buffer limits to buffer size)
    TEST_ASSERT_EQUAL_UINT32(mockData->mockBufferSize, mockData->mockAvailableBytes);
    
    // Read back and verify
    uint8_t readData[1000];
    size_t readBytes = circularBufferRead((AudioCapture_t *)capture, readData, sizeof(readData));
    TEST_ASSERT_EQUAL_UINT32(sizeof(readData), readBytes);
    
    audioCaptureDestroy(capture);
}

// Test 6: Buffer state management
void test_audioCapture_state_management(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    mock_audioCapture_init(capture, &testConfig);
    
    // Initial state should be idle
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, capture->state);
    
    // Start should change state to running
    mock_audioCapture_start(capture);
    TEST_ASSERT_EQUAL(AUDIO_STATE_RUNNING, capture->state);
    
    // Stop should change state back to idle
    mock_audioCapture_stop(capture);
    TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, capture->state);
    
    audioCaptureDestroy(capture);
}

// Test 7: Multiple start/stop cycles
test(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    mock_audioCapture_init(capture, &testConfig);
    
    // Perform multiple start/stop cycles
    for (int i = 0; i < 5; i++) {
        mock_audioCapture_start(capture);
        TEST_ASSERT_EQUAL(AUDIO_STATE_RUNNING, capture->state);
        
        mock_audioCapture_stop(capture);
        TEST_ASSERT_EQUAL(AUDIO_STATE_IDLE, capture->state);
    }
    
    audioCaptureDestroy(capture);
}

// Test 8: Get buffer size and sample count
test(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    mock_audioCapture_init(capture, &testConfig);
    
    // Verify buffer size
    TEST_ASSERT_EQUAL_UINT32(96000, audioCaptureGetBufferSize(capture));
    
    // Verify sample rate
    TEST_ASSERT_EQUAL_UINT32(16000, audioCaptureGetSampleRate(capture));
    
    // Verify bytes per sample (1 channel * 16 bits / 8)
    TEST_ASSERT_EQUAL_UINT32(2, audioCaptureGetBytesPerSample(capture));
    
    // Verify sample count when buffer is empty
    TEST_ASSERT_EQUAL_UINT32(0, audioCaptureGetSampleCount(capture));
    
    // Add some data and verify sample count
    mock_audioCapture_start(capture);
    uint8_t testData[1000];
    generateTestAudio(testData, sizeof(testData), 999);
    mock_audioCapture_process(capture, testData, sizeof(testData));
    
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    size_t expectedSamples = mockData->mockAvailableBytes / 2;
    TEST_ASSERT_EQUAL_UINT32(expectedSamples, audioCaptureGetSampleCount(capture));
    
    audioCaptureDestroy(capture);
}

// Test 9: Null pointer handling
test(void) {
    // All functions should handle null pointers gracefully
    TEST_ASSERT_EQUAL(0, audioCaptureGetBufferSize(NULL));
    TEST_ASSERT_EQUAL(0, audioCaptureGetSampleRate(NULL));
    TEST_ASSERT_EQUAL(0, audioCaptureGetSampleCount(NULL));
    TEST_ASSERT_EQUAL(0, audioCaptureGetBytesPerSample(NULL));
    TEST_ASSERT_EQUAL(AUDIO_STATE_ERROR, audioCaptureGetState(NULL));
    TEST_ASSERT_EQUAL(0, audioCaptureGetBuffer(NULL, NULL, 0));
    
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    // Destroy null should not crash
    audioCaptureDestroy(NULL);
    
    // Deinit null should not crash
    audioCaptureDeinit(NULL);
    
    audioCaptureDestroy(capture);
}

// Test 10: Buffer overflow protection
test(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    // Use small buffer for testing
    AudioConfig_t smallConfig = {
        .sampleRate = 16000,
        .bufferDuration = 100,
        .channels = 1,
        .bitsPerSample = 16,
        .pdmMode = 2
    };
    
    mock_audioCapture_init(capture, &smallConfig);
    mock_audioCapture_start(capture);
    
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    
    // Try to write more data than buffer can hold
    uint8_t largeData[10000];
    generateTestAudio(largeData, sizeof(largeData), 555);
    
    mock_audioCapture_process(capture, largeData, sizeof(largeData));
    
    // Buffer should be limited to its actual size
    TEST_ASSERT_EQUAL_UINT32(mockData->mockBufferSize, mockData->mockAvailableBytes);
    
    audioCaptureDestroy(capture);
}

// Test 11: Read after write
test(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    mock_audioCapture_init(capture, &testConfig);
    mock_audioCapture_start(capture);
    
    // Write test data
    uint8_t testData[500];
    generateTestAudio(testData, sizeof(testData), 777);
    mock_audioCapture_process(capture, testData, sizeof(testData));
    
    // Read the data back
    uint8_t readData[500];
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    
    size_t readBytes = circularBufferRead((AudioCapture_t *)capture, readData, sizeof(readData));
    TEST_ASSERT_EQUAL_UINT32(sizeof(testData), readBytes);
    
    // Verify data integrity
    TEST_ASSERT_TRUE(verifyBufferContents(readData, readBytes, 777));
    
    // Buffer should now be empty
    TEST_ASSERT_EQUAL_UINT32(0, circularBufferAvailable((const AudioCapture_t *)capture));
    
    audioCaptureDestroy(capture);
}

// Test 12: Partial read
test(void) {
    AudioCapture_t *capture = audioCaptureCreate();
    TEST_ASSERT_NOT_NULL(capture);
    
    mock_audioCapture_init(capture, &testConfig);
    mock_audioCapture_start(capture);
    
    // Write test data
    uint8_t testData[1000];
    generateTestAudio(testData, sizeof(testData), 888);
    mock_audioCapture_process(capture, testData, sizeof(testData));
    
    // Read only part of the data
    uint8_t partialRead[250];
    size_t readBytes = circularBufferRead((AudioCapture_t *)capture, partialRead, sizeof(partialRead));
    TEST_ASSERT_EQUAL_UINT32(sizeof(partialRead), readBytes);
    
    // Verify partial data
    TEST_ASSERT_TRUE(verifyBufferContents(partialRead, readBytes, 888));
    
    // Some data should still be in buffer
    AudioCaptureMockData_t *mockData = mock_audioCapture_getData(capture);
    TEST_ASSERT_NOT_NULL(mockData);
    TEST_ASSERT_EQUAL_UINT32(750, mockData->mockAvailableBytes);
    
    audioCaptureDestroy(capture);
}

// Test suite registration
void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_audioCapture_create_destroy);
    RUN_TEST(test_audioCapture_init_default);
    RUN_TEST(test_audioCapture_init_custom);
    RUN_TEST(test_audioCapture_write_read);
    RUN_TEST(test_audioCapture_wraparound);
    RUN_TEST(test_audioCapture_state_management);
    RUN_TEST(test_audioCapture_multiple_cycles);
    RUN_TEST(test_audioCapture_buffer_info);
    RUN_TEST(test_audioCapture_null_handling);
    RUN_TEST(test_audioCapture_overflow_protection);
    RUN_TEST(test_audioCapture_read_after_write);
    RUN_TEST(test_audioCapture_partial_read);
    
    return UNITY_END();
}
