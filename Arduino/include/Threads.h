#include "SoundAnalyzer.h" // Measures and processes the incoming audio

#include "StripManager.h"
#include "Effects.h"

#include "BluetoothLE.h"
#include "Screen.h"

#include "Constants.h"

// end-user debug features
TaskHandle_t startupSoundTask;

// core tasks
TaskHandle_t stripTask;
TaskHandle_t samplerTask;

// additional end user features
TaskHandle_t bluetoothTask;
TaskHandle_t screenTask;
TaskHandle_t webTask; // Not used (yet)

/// ==================================================================================

// LEDGFXLoop
//
// Main LED computation.
void LEDGFXLoop(void *)
{
    manager.setup(g_Brightness, g_PowerLimit);

    for (;;)
    {
        manager.loop();
        Screen::show();
    }
}

// SamplerLoop
//
// One CPU core spins in this loop, pulling completed buffers and running the FFT, etc.
void SamplerLoop(void *)
{
    unsigned long samplerLastFrame = 0;

    Serial.println("Audio Sampler Launching...");
    Serial.printf("  FFT Size: %d bytes\n", MAX_SAMPLES);
    gAnalyzer.StartInterrupts();

    for (;;)
    {
        gFPS = FPS(samplerLastFrame, millis());
        samplerLastFrame = millis();

        PeakData peaks = gAnalyzer.RunSamplerPass(BAND_COUNT);
        manager.SetPeaks(BAND_COUNT, peaks);

        Screen::show();
    }
}

int channel = 0;
bool startupFinished = false;
bool checkingForUpdates = false;

// StartupSoundLoop
//
// Temporary Thread to make a startup sound for better debug purposes.
void StartupSoundLoop(void *)
{
    int freq = 2000;
    int resolution = 8;

    pinMode(BUZZER_PIN, OUTPUT);

    ledcSetup(channel, freq, resolution);
    ledcAttachPin(BUZZER_PIN, channel);

    ledcWriteNote(channel, NOTE_F, 6);
    delay(300);
    ledcWriteTone(channel, 0);

    for (;;)
    {
        if (startupFinished == true)
        {
            ledcWriteNote(channel, NOTE_C, 7);
            delay(150);

            ledcWriteTone(channel, 0);
            startupFinished = NULL;
        }
        else if (checkingForUpdates)
        {
            ledcWriteNote(channel, NOTE_F, 3);
            delay(300);

            ledcWriteTone(channel, 0);
            delay(1000);
        }
        else
        {
            delay(1);
        }
    }
}

void BluetoothLoop(void *pvParameters)
{
    Serial.println("Init BLE Service...");
    BluetoothLE::setup();

    for (;;)
    {
        BluetoothLE::loop();
    }
}
