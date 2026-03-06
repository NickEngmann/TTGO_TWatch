# T-Watch BLE Audio Stream

This example demonstrates BLE GATT server functionality for audio streaming from the T-Watch to an Android phone.

## Features

- **BLE GATT Server**: Custom service for audio streaming
- **Audio Data Characteristic**: Notifies audio chunks to connected device
- **Connection Status Characteristic**: Real-time connection state updates
- **Recording State Characteristic**: Tracks recording state (IDLE, RECORDING, PAUSED, STOPPED)
- **Configurable Chunk Size**: Optimized for 16kHz 16-bit mono audio
- **Connection Timeout**: Automatic disconnect after 5 seconds of inactivity

## Technical Details

### Service UUID
```
11002024-4A48-4553-5345-202A55464C31
```

### Characteristics

| Characteristic | UUID | Properties |
|----------------|------|------------|
| Audio Data | 11012024-4A48-4553-5345-202A55464C31 | Notify |
| Connection Status | 11022024-4A48-5353-5345-202A55464C31 | Read, Notify |
| Recording State | 11032024-4A48-5353-5345-202A55464C31 | Read, Notify |

### Audio Streaming

- **Sample Rate**: 16kHz
- **Bit Depth**: 16-bit mono
- **Chunk Size**: 160 bytes (10ms audio per chunk)
- **Data Rate**: ~32KB/s raw (requires compression for real audio)
- **MTU**: 247 bytes (BLE standard max)

### Recording States

```cpp
enum class RecordingState : uint8_t {
    IDLE = 0,      // No recording
    RECORDING = 1, // Recording in progress
    PAUSED = 2,    // Recording paused
    STOPPED = 3    // Recording stopped
};
```

## Usage

### 1. Initialize BLE Server

```cpp
#include "BleServer.h"

using namespace TWatch;

BleServer* bleServer = new BleServer();
bleServer->init();
bleServer->setConnectionCallback([](CONNECTION_STATE state) {
    if (state == CONNECTION_STATE::CONNECTED) {
        Serial.println("Device connected");
    } else {
        Serial.println("Device disconnected");
    }
});
bleServer->startAdvertising();
```

### 2. Set Recording State

```cpp
bleServer->setRecordingState(RecordingState::RECORDING);
```

### 3. Send Audio Chunks

```cpp
uint8_t audioChunk[160];
// Fill audioChunk with audio data...
bleServer->sendChunk(audioChunk, 160);
```

### 4. Check Connection Status

```cpp
if (bleServer->isConnected()) {
    // Stream audio data
}
```

## Testing

Run native tests:
```bash
pio test -e native
```

## Phone App Integration

To receive audio data, create an Android app that:
1. Scans for "T-Watch Audio" BLE device
2. Connects to the custom service
3. Subscribes to the audio_data characteristic notifications
4. Processes received audio chunks

## Limitations

- Raw audio at 16kHz 16-bit mono = 32KB/s exceeds typical BLE bandwidth (~20KB/s)
- Consider implementing:
  - Voice Activity Detection (VAD) to reduce data rate
  - Audio compression (e.g., Opus codec)
  - Lower sample rates for voice-only applications

## Dependencies

- ESP32 BLE Stack (NimBLE or Arduino BLE)
- TFT_eSPI (for display)
- ArduinoJson (for configuration)

## License

MIT License - See LICENSE file in project root
