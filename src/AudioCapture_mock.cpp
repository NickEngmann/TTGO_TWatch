/*
 * AudioCapture Mock Implementation for Native Testing
 * 
 * Provides mock implementations for testing audio capture logic
 * without requiring ESP32 hardware.
 */

#include "AudioCapture_mock.h"
#include <stdlib.h>
#include <string.h>

// Mock implementation of init
bool mock_audioCapture_init(AudioCapture_t *capture, const AudioConfig_t *config) {
    if (!capture || !config) {
        return false;
    }
    
    // Allocate mock data
    AudioCaptureMockData_t *mockData = (AudioCaptureMockData_t *)calloc(1, sizeof(AudioCaptureMockData_t));
    if (!mockData) {
        return false;
    }
    
    // Calculate buffer size
    uint32_t sampleCount = (config->sampleRate * config->bufferDuration) / 1000;
    mockData->mockBufferSize = sampleCount * config->channels * (config->bitsPerSample / 8);
    
    // Allocate mock buffer
    mockData->mockBuffer = (uint8_t *)calloc(1, mockData->mockBufferSize);
    if (!mockData->mockBuffer) {
        free(mockData);
        return false;
    }
    
    // Initialize state
    mockData->mockWriteIndex = 0;
    mockData->mockReadIndex = 0;
    mockData->mockAvailableBytes = 0;
    mockData->mockStarted = false;
    mockData->mockInitialized = true;
    
    capture->mock_data = mockData;
    capture->config = *config;
    capture->bufferSize = mockData->mockBufferSize;
    capture->state = AUDIO_STATE_IDLE;
    
    return true;
}

// Mock implementation of start
bool mock_audioCapture_start(AudioCapture_t *capture) {
    if (!capture || !capture->mock_data) {
        return false;
    }
    
    AudioCaptureMockData_t *mockData = (AudioCaptureMockData_t *)capture->mock_data;
    mockData->mockStarted = true;
    capture->state = AUDIO_STATE_RUNNING;
    
    return true;
}

// Mock implementation of stop
bool mock_audioCapture_stop(AudioCapture_t *capture) {
    if (!capture || !capture->mock_data) {
        return false;
    }
    
    AudioCaptureMockData_t *mockData = (AudioCaptureMockData_t *)capture->mock_data;
    mockData->mockStarted = false;
    capture->state = AUDIO_STATE_IDLE;
    
    // Reset buffer
    mockData->mockWriteIndex = 0;
    mockData->mockReadIndex = 0;
    mockData->mockAvailableBytes = 0;
    
    return true;
}

// Process mock data (simulates audio capture)
void mock_audioCapture_process(AudioCapture_t *capture, const uint8_t *data, size_t len) {
    if (!capture || !capture->mock_data || !data || len == 0) {
        return;
    }
    
    AudioCaptureMockData_t *mockData = (AudioCaptureMockData_t *)capture->mock_data;
    
    if (!mockData->mockStarted) {
        return;  // Not started, ignore data
    }
    
    // Simulate circular buffer write
    size_t spaceAvailable = mockData->mockBufferSize - mockData->mockAvailableBytes;
    size_t bytesToWrite = (len < spaceAvailable) ? len : spaceAvailable;
    
    if (bytesToWrite == 0) {
        return;
    }
    
    // Calculate write position with wraparound
    size_t writePos = mockData->mockWriteIndex;
    size_t firstChunk = (writePos + bytesToWrite > mockData->mockBufferSize) ? 
                         (mockData->mockBufferSize - writePos) : bytesToWrite;
    
    // Copy first chunk
    memcpy(mockData->mockBuffer + writePos, data, firstChunk);
    
    // Copy remaining chunk if needed
    if (bytesToWrite > firstChunk) {
        memcpy(mockData->mockBuffer, data + firstChunk, bytesToWrite - firstChunk);
        mockData->mockWriteIndex = bytesToWrite - firstChunk;
    } else {
        mockData->mockWriteIndex = writePos + bytesToWrite;
    }
    
    mockData->mockAvailableBytes += bytesToWrite;
}

// Get mock data pointer
AudioCaptureMockData_t *mock_audioCapture_getData(const AudioCapture_t *capture) {
    if (!capture || !capture->mock_data) {
        return NULL;
    }
    return (AudioCaptureMockData_t *)capture->mock_data;
}
