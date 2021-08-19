#include <Arduino.h>

#include <arduinoFFT.h> // FFT code for SoundAnalzyer

#include "Constants.h"
#include "Globals.h"

#include "Utilities.h"

#include "Threads.h"
//#include "LEDMatrixGFX.h"
// #include "SpectrumDisplay.h" // Draws the bars on the LEDs

using namespace std;

void printBoardInfo()
{
  Serial.print("ESP SDK Version: ");
  Serial.println(ESP.getSdkVersion());

  Serial.print("ESP Chip Revision: ");
  Serial.println(ESP.getChipRevision());
}

void printProgramHeader()
{
  Serial.println("\rBALE - BMW Ambient Light & Effect's - (c) 2020 Max Scopp");
  Serial.println("------------------------------------------------------------");
  Serial.println("");
}

void setupLedStrip()
{
  pinMode(13, OUTPUT);

  Serial.printf("Initializing led task... ");

  const int pinned_to_core = 1;
  const int priority = 10;

  xTaskCreatePinnedToCore(
      LEDGFXLoop,
      "Strip Animation Loop",
      STACK_SIZE, nullptr,
      priority,
      &stripTask,
      pinned_to_core);

  Serial.printf("leds on core %d with priority %d. ", pinned_to_core, priority);
}

void setupAudioSampler()
{
  Serial.println("Setting up adc..."); // BUGBUG: Hardcoded output
  Serial.println("  Resolution  : 12 bit");
  Serial.println("  Cycles      : 32");
  Serial.println("  Supersamples:  2");
  Serial.println("  Attenuation :  2.5dB");
  Serial.println("");

  // Set up the ESP32 DAC

  adcAttachPin(INPUT_PIN);

  analogReadResolution(12); // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
  analogSetWidth(12);       // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
  // analogSetCycles(32);                           // Set number of cycles per sample, default is 8 and provides an optimal result, range is 1 - 255
  // analogSetSamples(SUPERSAMPLES);                // Set number of samples in the range, default is 1, it has an effect on sensitivity has been multiplied
  analogSetClockDiv(1);                          // Set the divider for the ADC clock, default is 1, range is 1 - 255
  analogSetAttenuation(ADC_11db);                // For all pins
  analogSetPinAttenuation(INPUT_PIN, ADC_2_5db); // Sets the input attenuation for the audio pin ONLY, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db

  // End of ESP32 DAC setup

  Serial.printf("Initializing sampler task... ");

  const int pinned_to_core = 0;
  const int priority = 10;

  xTaskCreatePinnedToCore(
      SamplerLoop,
      "Sampler Loop",
      STACK_SIZE, nullptr,
      priority,
      &samplerTask,
      pinned_to_core);

  Serial.printf("running on core %d with priority %d. \n", pinned_to_core, priority);
}

void setupBluetoothConnectivity()
{
  Serial.println("Setting up bluetooth...");

  const int pinned_to_core = 1;
  const int priority = 5;

  xTaskCreatePinnedToCore(
      BluetoothLoop,
      "Bluetooth Loop",
      STACK_SIZE, nullptr,
      priority,
      &bluetoothTask,
      pinned_to_core);

  Serial.printf("running on core %d with priority %d. \n", pinned_to_core, priority);
}

void setupLazyStartupSound()
{
  xTaskCreatePinnedToCore(
      StartupSoundLoop,
      "Startup Sound",
      STACK_SIZE, nullptr,
      0,
      &startupSoundTask,
      0);
}

void setup()
{
  // delay(2000); // half a second of silence is a virtue
  Serial.begin(115200);

  printProgramHeader();
  printBoardInfo();

  setupLazyStartupSound();
  Serial.println("");

  setupLedStrip();
  Serial.println("");

  setupAudioSampler();
  Serial.println("");

  setupBluetoothConnectivity();
  Serial.println("");

  Serial.println("READY");

  // manager.g_EffectPointer = new RainbowEffect();
  manager.g_EffectPointer = new SoundDebugEffect();

  startupFinished = true;
}

void loop()
{
}
