#include "LEDGFX.h"
#include "PeakData.h"
#include "SoundAnalyzer.h" // Measures and processes the incoming audio

#include "Constants.h"

// end-user debug features
TaskHandle_t startupSoundTask;

// core tasks
TaskHandle_t stripTask;
TaskHandle_t samplerTask;

// additional end user features
TaskHandle_t bluetoothTask;
TaskHandle_t webTask; // Not used (yet)

/// ==================================================================================

volatile size_t gFPS = 0; // FFT frames per second
volatile size_t mFPS = 0; // Matrix frames per second

// TODO: Move this somewhere else
SoundAnalyzer gAnalyzer(INPUT_PIN);

// LEDGFXLoop
//
// Main LED computation.
void LEDGFXLoop(void *)
{
    manager.setup(g_Brightness, g_PowerLimit);

    for (;;)
    {
        manager.loop();
    }
}

// SamplerLoop
//
// One CPU core spins in this loop, pulling completed buffers and running the FFT, etc.
void SamplerLoop(void *)
{
    unsigned long samplerLastFrame = 0;
    Serial.println("\n\nSampling...\n\n");

    for (;;)
    {
        gFPS = FPS(samplerLastFrame, millis());
        samplerLastFrame = millis();

        PeakData peaks = gAnalyzer.RunSamplerPass(BAND_COUNT);
        manager.SetPeaks(BAND_COUNT, peaks);
    }
}

// StartupSoundLoop
//
// Temporary Thread to make a startup sound for better debug purposes.
void StartupSoundLoop(void *)
{
    int freq = 2000;
    int channel = 0;
    int resolution = 8;

    ledcSetup(channel, freq, resolution);
    ledcAttachPin(BUZZER_PIN, channel);

    ledcWriteNote(channel, NOTE_C, 6);
    delay(100);
    ledcWriteNote(channel, NOTE_C, 7);
    delay(450);

    ledcDetachPin(BUZZER_PIN);
    vTaskDelete(startupSoundTask);
}

void BluetoothLoop(void *pvParameters)
{
    vTaskDelete(bluetoothTask);
    // Serial.println("Init BLE...");

    for (;;)
    {
        // WIP
    }
}