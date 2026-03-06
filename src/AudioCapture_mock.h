/*
 * AudioCapture Mock Header for Native Testing
 * 
 * Provides mock implementations for testing audio capture logic
 * without requiring ESP32 hardware.
 */

#ifndef AUDIO_CAPTURE_MOCK_H
#define AUDIO_CAPTURE_MOCK_H

#include "AudioCapture.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mock data structure for native testing
typedef struct {
    uint8_t *mockBuffer;
    size_t mockBufferSize;
    size_t mockWriteIndex;
    size_t mockReadIndex;
    size_t mockAvailableBytes;
    bool mockStarted;
    bool mockInitialized;
} AudioCaptureMockData_t;

// Mock function declarations
bool mock_audioCapture_init(AudioCapture_t *capture, const AudioConfig_t *config);
bool mock_audioCapture_start(AudioCapture_t *capture);
bool mock_audioCapture_stop(AudioCapture_t *capture);
void mock_audioCapture_process(AudioCapture_t *capture, const uint8_t *data, size_t len);
AudioCaptureMockData_t *mock_audioCapture_getData(const AudioCapture_t *capture);

#ifdef __cplusplus
}
#endif

// Circular buffer helper functions for testing
size_t circularBufferRead(AudioCapture_t *capture, uint8_t *dest, size_t destSize);
size_t circularBufferAvailable(const AudioCapture_t *capture);

#endif /* AUDIO_CAPTURE_MOCK_H */
