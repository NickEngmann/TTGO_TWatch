#include "BleServer.h"
#include "TTGO.h"
#include <Arduino.h>

using namespace TWatch;

// Global BLE server instance
static BleServer* g_bleServer = nullptr;

// Connection state display
static bool g_connectionState = false;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== T-Watch BLE Audio Stream ===");
    
    // Initialize BLE Server
    g_bleServer = new BleServer();
    
    if (!g_bleServer->init()) {
        Serial.println("Failed to initialize BLE Server");
        return;
    }
    
    Serial.println("BLE Server initialized");
    
    // Set up connection callback
    g_bleServer->setConnectionCallback([](CONNECTION_STATE state) {
        g_connectionState = (state == CONNECTION_STATE::CONNECTED);
        Serial.printf("Connection state: %s\n", g_connectionState ? "CONNECTED" : "DISCONNECTED");
    });
    
    // Start advertising
    g_bleServer->startAdvertising();
    Serial.println("BLE advertising started");
    Serial.println("Watch name: T-Watch Audio");
    Serial.println("Service UUID: 316C4655-2A53-5353-4548-242000110011");
    
    // Configure chunk size for audio streaming
    // 16kHz 16-bit mono = 32KB/s raw
    // BLE MTU 247 bytes, send in chunks of 160 bytes (10ms audio)
    g_bleServer->setChunkSize(160);
    g_bleServer->setMtu(247);
    
    Serial.printf("Chunk size: %d bytes\n", g_bleServer->getChunkSize());
    Serial.printf("MTU: %d bytes\n", g_bleServer->getMtu());
    
    Serial.println("\n=== BLE Audio Stream Ready ===");
    Serial.println("Pair with phone app to stream audio");
}

void loop() {
    // Check connection timeout
    g_bleServer->checkConnectionTimeout();
    
    // Simulate audio data streaming when connected
    if (g_bleServer->isConnected() && g_bleServer->getRecordingState() == RecordingState::RECORDING) {
        // Generate test audio data (sine wave simulation)
        static uint16_t audioSample = 0;
        audioSample += 100;  // Increment for frequency simulation
        
        uint8_t chunk[160];
        for (int i = 0; i < 160; i++) {
            chunk[i] = (audioSample + i * 10) & 0xFF;
        }
        
        if (g_bleServer->sendChunk(chunk, 160)) {
            Serial.printf("Sent audio chunk: %d bytes\n", 160);
        }
        
        // Update display with connection status
        Serial.printf("Connection: %s | Recording: %s\n", 
            g_bleServer->isConnected() ? "YES" : "NO",
            g_bleServer->getRecordingState() == RecordingState::RECORDING ? "ACTIVE" : "IDLE");
    }
    
    delay(100);  // 10Hz update rate
}

// BLE Callback handlers
void BleServer::onConnect(BLEServer* pServer) {
    Serial.println("[BLE] Device connected");
    BleServer::startConnection();
}

void BleServer::onDisconnect(BLEServer* pServer) {
    Serial.println("[BLE] Device disconnected");
    BleServer::endConnection();
}
