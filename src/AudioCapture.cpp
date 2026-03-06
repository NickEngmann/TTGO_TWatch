/*
 * AudioCapture - PDM Microphone Audio Capture for TTGO T-Watch V3
 * 
 * Implementation of audio capture using ESP32 I2S peripheral for PDM microphone.
 * Includes circular buffer management and HAL abstraction for native testing.
 */

#include "AudioCapture.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef NATIVE_BUILD
    // Native build - use mock implementations
    #include "AudioCapture_mock.h"
#else
    // ESP32 build - use real I2S driver
    #include "driver/i2s.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    #include "esp_log.h"
#endif

static const char *TAG = "AudioCapture";

// Default configuration for 16kHz mono PDM audio
static const AudioConfig_t defaultConfig = {
    .sampleRate = 16000,
    .bufferDuration = 3000,  // 3 seconds
    .channels = 1,
    .bitsPerSample = 16,
    .pdmMode = 2  // Dual channel PDM mode
};

// Circular buffer helper functions
static void circularBufferReset(AudioCapture_t *capture);
static size_t circularBufferWrite(AudioCapture_t *capture, const uint8_t *data, size_t len);
static size_t circularBufferRead(AudioCapture_t *capture, uint8_t *dest, size_t len);
static size_t circularBufferAvailable(const AudioCapture_t *capture);

// Default implementation of interface functions
static bool audioCapture_init(AudioCapture_t *capture, const AudioConfig_t *config);
static bool audioCapture_start(AudioCapture_t *capture);
static bool audioCapture_stop(AudioCapture_t *capture);
static bool audioCapture_pause(AudioCapture_t *capture);
static bool audioCapture_resume(AudioCapture_t *capture);
static AudioState_t audioCapture_getState(const AudioCapture_t *capture);
static size_t audioCapture_getBuffer(const AudioCapture_t *capture, uint8_t *dest, size_t destSize);
static size_t audioCapture_getBufferSize(const AudioCapture_t *capture);
static size_t audioCapture_getSampleCount(const AudioCapture_t *capture);
static size_t audioCapture_getBytesPerSample(const AudioCapture_t *capture);
static uint32_t audioCapture_getSampleRate(const AudioCapture_t *capture);
static void audioCapture_deinit(AudioCapture_t *capture);

// Interface implementation
AudioInterface_t audioCaptureInterface = {
    .init = audioCapture_init,
    .start = audioCapture_start,
    .stop = audioCapture_stop,
    .pause = audioCapture_pause,
    .resume = audioCapture_resume,
    .getState = audioCapture_getState,
    .getBuffer = audioCapture_getBuffer,
    .getBufferSize = audioCapture_getBufferSize,
    .getSampleCount = audioCapture_getSampleCount,
    .getBytesPerSample = audioCapture_getBytesPerSample,
    .getSampleRate = audioCapture_getSampleRate,
    .deinit = audioCapture_deinit
};

// Create audio capture instance
AudioCapture_t *audioCaptureCreate(void) {
    AudioCapture_t *capture = (AudioCapture_t *)calloc(1, sizeof(AudioCapture_t));
    if (!capture) {
        ESP_LOGE(TAG, "Failed to allocate AudioCapture structure");
        return NULL;
    }
    
    capture->iface = &audioCaptureInterface;
    capture->state = AUDIO_STATE_IDLE;
    capture->buffer = NULL;
    capture->bufferSize = 0;
    capture->writeIndex = 0;
    capture->readIndex = 0;
    capture->availableBytes = 0;
    capture->i2sPort = I2S_NUM_0;
    capture->pdmMode = defaultConfig.pdmMode;
    
#ifdef NATIVE_BUILD
    capture->mock_init = mock_audioCapture_init;
    capture->mock_start = mock_audioCapture_start;
    capture->mock_stop = mock_audioCapture_stop;
    capture->mock_data = NULL;
#endif
    
    return capture;
}

// Destroy audio capture instance
void audioCaptureDestroy(AudioCapture_t *capture) {
    if (!capture) {
        return;
    }
    
    // Stop if running
    if (capture->state == AUDIO_STATE_RUNNING) {
        capture->iface->stop(capture);
    }
    
    // Free buffer
    if (capture->buffer) {
        free(capture->buffer);
        capture->buffer = NULL;
    }
    
    capture->bufferSize = 0;
    capture->writeIndex = 0;
    capture->readIndex = 0;
    capture->availableBytes = 0;
    
#ifdef NATIVE_BUILD
    if (capture->mock_data) {
        free(capture->mock_data);
        capture->mock_data = NULL;
    }
#endif
    
    free(capture);
}

// Circular buffer reset
static void circularBufferReset(AudioCapture_t *capture) {
    capture->writeIndex = 0;
    capture->readIndex = 0;
    capture->availableBytes = 0;
}

// Circular buffer write
static size_t circularBufferWrite(AudioCapture_t *capture, const uint8_t *data, size_t len) {
    if (!capture || !capture->buffer || len == 0) {
        return 0;
    }
    
    size_t spaceAvailable = capture->bufferSize - capture->availableBytes;
    size_t bytesToWrite = (len < spaceAvailable) ? len : spaceAvailable;
    
    if (bytesToWrite == 0) {
        return 0;
    }
    
    // Calculate write position with wraparound
    size_t writePos = capture->writeIndex;
    size_t firstChunk = (writePos + bytesToWrite > capture->bufferSize) ? 
                         (capture->bufferSize - writePos) : bytesToWrite;
    
    // Copy first chunk
    memcpy(capture->buffer + writePos, data, firstChunk);
    
    // Copy remaining chunk if needed
    if (bytesToWrite > firstChunk) {
        memcpy(capture->buffer, data + firstChunk, bytesToWrite - firstChunk);
        capture->writeIndex = bytesToWrite - firstChunk;
    } else {
        capture->writeIndex = writePos + bytesToWrite;
    }
    
    capture->availableBytes += bytesToWrite;
    return bytesToWrite;
}

// Circular buffer read
static size_t circularBufferRead(AudioCapture_t *capture, uint8_t *dest, size_t len) {
    if (!capture || !capture->buffer || !dest || len == 0) {
        return 0;
    }
    
    size_t available = circularBufferAvailable(capture);
    size_t bytesToRead = (len < available) ? len : available;
    
    if (bytesToRead == 0) {
        return 0;
    }
    
    // Calculate read position with wraparound
    size_t readPos = capture->readIndex;
    size_t firstChunk = (readPos + bytesToRead > capture->bufferSize) ? 
                         (capture->bufferSize - readPos) : bytesToRead;
    
    // Copy first chunk
    memcpy(dest, capture->buffer + readPos, firstChunk);
    
    // Copy remaining chunk if needed
    if (bytesToRead > firstChunk) {
        memcpy(dest + firstChunk, capture->buffer, bytesToRead - firstChunk);
        capture->readIndex = bytesToRead - firstChunk;
    } else {
        capture->readIndex = readPos + bytesToRead;
    }
    
    capture->availableBytes -= bytesToRead;
    return bytesToRead;
}

// Get available bytes in circular buffer
static size_t circularBufferAvailable(const AudioCapture_t *capture) {
    if (!capture) {
        return 0;
    }
    return capture->availableBytes;
}

// Initialize with default configuration
bool audioCaptureInitDefault(AudioCapture_t *capture) {
    if (!capture) {
        return false;
    }
    return audioCaptureInit(capture, &defaultConfig);
}

// Initialize with custom configuration
static bool audioCapture_init(AudioCapture_t *capture, const AudioConfig_t *config) {
    if (!capture || !config) {
        return false;
    }
    
    // Stop if currently running
    if (capture->state == AUDIO_STATE_RUNNING) {
        capture->iface->stop(capture);
    }
    
    // Copy configuration
    capture->config = *config;
    capture->pdmMode = config->pdmMode;
    
    // Calculate buffer size: sampleRate * duration_ms / 1000 * channels * bitsPerSample / 8
    uint32_t sampleCount = (config->sampleRate * config->bufferDuration) / 1000;
    capture->bufferSize = sampleCount * config->channels * (config->bitsPerSample / 8);
    
    // Allocate buffer
    capture->buffer = (uint8_t *)calloc(1, capture->bufferSize);
    if (!capture->buffer) {
        ESP_LOGE(TAG, "Failed to allocate audio buffer (size: %lu)", (unsigned long)capture->bufferSize);
        return false;
    }
    
    // Reset circular buffer
    circularBufferReset(capture);
    capture->state = AUDIO_STATE_IDLE;
    
    ESP_LOGI(TAG, "Audio capture initialized: %lu samples, %lu bytes buffer", 
             (unsigned long)sampleCount, (unsigned long)capture->bufferSize);
    
    return true;
}

// Start audio capture
static bool audioCapture_start(AudioCapture_t *capture) {
    if (!capture) {
        return false;
    }
    
#ifdef NATIVE_BUILD
    // Use mock implementation for native build
    if (capture->mock_start && capture->mock_init(capture, &capture->config)) {
        capture->state = AUDIO_STATE_RUNNING;
        return true;
    }
    return false;
#else
    // ESP32 I2S PDM setup
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = capture->config.sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = GPIO_NUM_25,  // I2S BCK
        .ws_io_num = GPIO_NUM_26,  // I2S WS (L/R clock)
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = GPIO_NUM_22  // I2S DIN (PDM data)
    };
    
    // Configure for PDM mode
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    
    esp_err_t err = i2s_driver_install(capture->i2sPort, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %d", err);
        return false;
    }
    
    err = i2s_set_pin(capture->i2sPort, &pin_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pins: %d", err);
        i2s_driver_uninstall(capture->i2sPort);
        return false;
    }
    
    // Set PDM mode (dual channel for INMP441)
    i2s_set_pdm_rx_divider(capture->i2sPort, I2S_PDM_RX_DIVIDER_DISABLE);
    
    // Clear buffer before starting
    circularBufferReset(capture);
    capture->state = AUDIO_STATE_RUNNING;
    
    ESP_LOGI(TAG, "I2S audio capture started on port %d", capture->i2sPort);
    return true;
#endif
}

// Stop audio capture
static bool audioCapture_stop(AudioCapture_t *capture) {
    if (!capture) {
        return false;
    }
    
#ifdef NATIVE_BUILD
    if (capture->mock_stop) {
        capture->mock_stop(capture);
    }
    capture->state = AUDIO_STATE_IDLE;
    return true;
#else
    // Uninstall I2S driver
    i2s_stop(capture->i2sPort);
    i2s_driver_uninstall(capture->i2sPort);
    capture->state = AUDIO_STATE_IDLE;
    
    ESP_LOGI(TAG, "I2S audio capture stopped");
    return true;
#endif
}

// Pause audio capture
static bool audioCapture_pause(AudioCapture_t *capture) {
    if (!capture) {
        return false;
    }
    
#ifdef NATIVE_BUILD
    capture->state = AUDIO_STATE_PAUSED;
    return true;
#else
    i2s_stop(capture->i2sPort);
    capture->state = AUDIO_STATE_PAUSED;
    
    ESP_LOGI(TAG, "I2S audio capture paused");
    return true;
#endif
}

// Resume audio capture
static bool audioCapture_resume(AudioCapture_t *capture) {
    if (!capture) {
        return false;
    }
    
#ifdef NATIVE_BUILD
    capture->state = AUDIO_STATE_RUNNING;
    return true;
#else
    i2s_start(capture->i2sPort);
    capture->state = AUDIO_STATE_RUNNING;
    
    ESP_LOGI(TAG, "I2S audio capture resumed");
    return true;
#endif
}

// Get current state
static AudioState_t audioCapture_getState(const AudioCapture_t *capture) {
    if (!capture) {
        return AUDIO_STATE_ERROR;
    }
    return capture->state;
}

// Get buffer data
static size_t audioCapture_getBuffer(const AudioCapture_t *capture, uint8_t *dest, size_t destSize) {
    if (!capture || !dest) {
        return 0;
    }
    
    size_t available = circularBufferAvailable(capture);
    size_t bytesToRead = (destSize < available) ? destSize : available;
    
    return circularBufferRead((AudioCapture_t *)capture, dest, bytesToRead);
}

// Get buffer size
static size_t audioCapture_getBufferSize(const AudioCapture_t *capture) {
    if (!capture) {
        return 0;
    }
    return capture->bufferSize;
}

// Get sample count
static size_t audioCapture_getSampleCount(const AudioCapture_t *capture) {
    if (!capture) {
        return 0;
    }
    size_t bytesAvailable = circularBufferAvailable(capture);
    return bytesAvailable / (capture->config.channels * (capture->config.bitsPerSample / 8));
}

// Get bytes per sample
static size_t audioCapture_getBytesPerSample(const AudioCapture_t *capture) {
    if (!capture) {
        return 0;
    }
    return capture->config.channels * (capture->config.bitsPerSample / 8);
}

// Get sample rate
static uint32_t audioCapture_getSampleRate(const AudioCapture_t *capture) {
    if (!capture) {
        return 0;
    }
    return capture->config.sampleRate;
}

// Deinitialize
static void audioCapture_deinit(AudioCapture_t *capture) {
    if (!capture) {
        return;
    }
    
    audioCaptureStop(capture);
    
    if (capture->buffer) {
        free(capture->buffer);
        capture->buffer = NULL;
    }
    capture->bufferSize = 0;
    capture->state = AUDIO_STATE_IDLE;
    
    ESP_LOGI(TAG, "Audio capture deinitialized");
}

// Public wrapper functions
bool audioCaptureInit(AudioCapture_t *capture, const AudioConfig_t *config) {
    if (!capture || !capture->iface) {
        return false;
    }
    return capture->iface->init(capture, config);
}

bool audioCaptureStart(AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return false;
    }
    return capture->iface->start(capture);
}

bool audioCaptureStop(AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return false;
    }
    return capture->iface->stop(capture);
}

bool audioCapturePause(AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return false;
    }
    return capture->iface->pause(capture);
}

bool audioCaptureResume(AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return false;
    }
    return capture->iface->resume(capture);
}

AudioState_t audioCaptureGetState(const AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return AUDIO_STATE_ERROR;
    }
    return capture->iface->getState(capture);
}

size_t audioCaptureGetBuffer(const AudioCapture_t *capture, uint8_t *dest, size_t destSize) {
    if (!capture || !capture->iface) {
        return 0;
    }
    return capture->iface->getBuffer(capture, dest, destSize);
}

size_t audioCaptureGetBufferSize(const AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return 0;
    }
    return capture->iface->getBufferSize(capture);
}

size_t audioCaptureGetSampleCount(const AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return 0;
    }
    return capture->iface->getSampleCount(capture);
}

size_t audioCaptureGetBytesPerSample(const AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return 0;
    }
    return capture->iface->getBytesPerSample(capture);
}

uint32_t audioCaptureGetSampleRate(const AudioCapture_t *capture) {
    if (!capture || !capture->iface) {
        return 0;
    }
    return capture->iface->getSampleRate(capture);
}

void audioCaptureDeinit(AudioCapture_t *capture) {
    if (capture && capture->iface) {
        capture->iface->deinit(capture);
    }
}
