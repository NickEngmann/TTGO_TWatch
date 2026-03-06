/*
 * AudioCapture - PDM Microphone Audio Capture for TTGO T-Watch V3
 * 
 * This module provides audio capture functionality using the ESP32 I2S peripheral
 * to read PDM microphone data and store it in a circular buffer.
 */

#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Audio capture configuration
typedef struct {
    uint32_t sampleRate;      // Sample rate in Hz (16000 for 16kHz)
    uint32_t bufferDuration;  // Buffer duration in milliseconds (2000-4000)
    uint32_t channels;        // Number of channels (1 for mono)
    uint32_t bitsPerSample;   // Bits per sample (16)
    uint32_t pdmMode;         // PDM mode (0 = left channel, 1 = right channel, 2 = dual)
} AudioConfig_t;

// Audio capture state
typedef enum {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_RUNNING,
    AUDIO_STATE_PAUSED,
    AUDIO_STATE_ERROR
} AudioState_t;

// Audio capture handle
typedef struct AudioCapture AudioCapture_t;

// Audio capture interface (HAL)
typedef struct {
    // Initialize audio capture with given configuration
    bool (*init)(AudioCapture_t *capture, const AudioConfig_t *config);
    
    // Start audio capture
    bool (*start)(AudioCapture_t *capture);
    
    // Stop audio capture
    bool (*stop)(AudioCapture_t *capture);
    
    // Pause audio capture
    bool (*pause)(AudioCapture_t *capture);
    
    // Resume audio capture
    bool (*resume)(AudioCapture_t *capture);
    
    // Get current audio state
    AudioState_t (*getState)(const AudioCapture_t *capture);
    
    // Get circular buffer data (returns number of bytes available)
    size_t (*getBuffer)(const AudioCapture_t *capture, uint8_t *dest, size_t destSize);
    
    // Get circular buffer size
    size_t (*getBufferSize)(const AudioCapture_t *capture);
    
    // Get number of samples available
    size_t (*getSampleCount)(const AudioCapture_t *capture);
    
    // Get bytes per sample
    size_t (*getBytesPerSample)(const AudioCapture_t *capture);
    
    // Get sample rate
    uint32_t (*getSampleRate)(const AudioCapture_t *capture);
    
    // Deinitialize audio capture
    void (*deinit)(AudioCapture_t *capture);
} AudioInterface_t;

// Audio capture structure
typedef struct AudioCapture {
    AudioInterface_t *iface;      // Interface pointer for polymorphism
    AudioConfig_t config;         // Configuration
    AudioState_t state;           // Current state
    
    // Circular buffer implementation details
    uint8_t *buffer;              // Buffer data
    size_t bufferSize;            // Total buffer size in bytes
    size_t writeIndex;            // Current write position
    size_t readIndex;             // Current read position
    size_t availableBytes;        // Number of bytes available to read
    
    // I2S specific (ESP32 only)
    int i2sPort;
    int pdmMode;
    
    // For native build mocks
    bool (*mock_init)(struct AudioCapture *capture, const AudioConfig_t *config);
    bool (*mock_start)(struct AudioCapture *capture);
    bool (*mock_stop)(struct AudioCapture *capture);
    void *mock_data;              // Mock-specific data
} AudioCapture_t;

// Global interface for default implementation
extern AudioInterface_t audioCaptureInterface;

// Create audio capture instance
AudioCapture_t *audioCaptureCreate(void);

// Destroy audio capture instance
void audioCaptureDestroy(AudioCapture_t *capture);

// Initialize with default configuration (16kHz mono, 3 second buffer)
bool audioCaptureInitDefault(AudioCapture_t *capture);

// Initialize with custom configuration
bool audioCaptureInit(AudioCapture_t *capture, const AudioConfig_t *config);

// Start audio capture
bool audioCaptureStart(AudioCapture_t *capture);

// Stop audio capture
bool audioCaptureStop(AudioCapture_t *capture);

// Pause audio capture
bool audioCapturePause(AudioCapture_t *capture);

// Resume audio capture
bool audioCaptureResume(AudioCapture_t *capture);

// Get current state
AudioState_t audioCaptureGetState(const AudioCapture_t *capture);

// Get buffer data (copies data to dest, returns bytes copied)
size_t audioCaptureGetBuffer(const AudioCapture_t *capture, uint8_t *dest, size_t destSize);

// Get buffer size in bytes
size_t audioCaptureGetBufferSize(const AudioCapture_t *capture);

// Get number of samples available
size_t audioCaptureGetSampleCount(const AudioCapture_t *capture);

// Get bytes per sample
size_t audioCaptureGetBytesPerSample(const AudioCapture_t *capture);

// Get sample rate
uint32_t audioCaptureGetSampleRate(const AudioCapture_t *capture);

// Deinitialize
void audioCaptureDeinit(AudioCapture_t *capture);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_CAPTURE_H */
