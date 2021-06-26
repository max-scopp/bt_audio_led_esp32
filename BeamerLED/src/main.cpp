#include <Arduino.h>
#include <stdexcept>

#include <arduinoFFT.h> // FFT code for SoundAnalzyer

#define SHOW_FPS false

// from dave...

#define BAND_COUNT 16      // Choices are 8, 16, 24, or 32.  Only 16 is "pretty" and hand-tuned, but you could fix others
#define MATRIX_WIDTH 48    // Number of pixels wide
#define MATRIX_HEIGHT 16   // Number of pixels tall
#define GAIN_DAMPEN 2      // Higher values cause auto gain to react more slowly
#define INPUT_PIN 32       // Audio line input
#define COLOR_SPEED_PIN 33 // How fast palette rotates in spectrum bars
#define MAX_COLOR_SPEED 64 //    ...and the max allowed

#define BRIGHTNESS_PIN 25   // Pin for brightness pot read
#define PEAK_DECAY_PIN 26   // Pin for peak decay pot read
#define COLOR_SCHEME_PIN 27 // Pin for controlling color scheme

#define SUPERSAMPLES 2                                    // How many supersamples to take
#define SAMPLE_BITS 12                                    // Sample resolution (0-4095)
#define MAX_ANALOG_IN ((1 << SAMPLE_BITS) * SUPERSAMPLES) // What our max analog input value is on all analog pins (4096 is default 12 bit resolution)
#define MAX_VU 12000                                      // How high our VU could max out at.  Arbitarily tuned.
#define ONSCREEN_FPS 0                                    // Debugging display of FPS count on LED screen
#define MS_PER_SECOND 1000                                // 1000 milliseconds per second
#define STACK_SIZE 4096                                   // Stack size for each new thread

#define BLACK 0x0000 // Color definitions in 16-bit 5-6-5 space
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

volatile float gScaler = 0.0f;       // Instanteous read of LED display vertical scaling
volatile size_t gFPS = 0;            // FFT frames per second
volatile size_t mFPS = 0;            // Matrix frames per second
volatile float gLogScale = 2.0f;     // How exponential the peaks are made to be
volatile float gBrightness = 64;     // LED matrix brightness, 0-255
volatile float gPeakDecay = 0.0;     // Peak decay for white line on top of spectrum bars
volatile float gColorSpeed = 128.0f; // How fast the color palette rotates (smaller is faster, it's a time divisor)
volatile float gVU = 0;              // Instantaneous read of VU value
volatile int giColorScheme = 0;      // Global color scheme (index into table of palettes)

volatile unsigned long g_cSamples = 0;    // Total number of samples successfully collected
volatile unsigned long g_cInterrupts = 0; // Total number of interrupts that have occured
volatile unsigned long g_cIRQMisses = 0;  // Number of times buffer wasn't lockable by IRQ

#include "Constants.h"
#include "Utilities.h"

#include "LEDGFX.h"
//#include "LEDMatrixGFX.h"
// #include "SpectrumDisplay.h" // Draws the bars on the LEDs
#include "PeakData.h"
#include "SoundAnalyzer.h" // Measures and processes the incoming audio

#include "effects/debug.h"
#include "effects/rainbow.h"
// #include "effects/bounce.h"
// #include "effects/comet.h"
// #include "effects/fire.h"
// #include "effects/marquee.h"
// #include "effects/twinkle.h"

using namespace std;

TaskHandle_t stripTask;
TaskHandle_t samplerTask;

TaskHandle_t bluetoothTask;
TaskHandle_t webTask;

SoundAnalyzer gAnalyzer(INPUT_PIN);

// SpectrumDisplay gDisplay(&gAlignment, BAND_COUNT);

// void createBluetoothTask(void *pvParameters)
// {
//   // initBleCommunicationChannel();
//   initBluetooth("E46 ESP32 Sink", 13);

//   for (;;)
//   {
//     handleBluetooth();
//   }
// }

// SamplerLoop
//
// One CPU core spins in this loop, pulling completed buffers and running the FFT, etc.

void SamplerLoop(void *)
{
  unsigned long samplerLastFrame = 0;
  for (;;)
  {
    Serial.println("\n\nSampling...\n\n");
    gFPS = FPS(samplerLastFrame, millis());
    samplerLastFrame = millis();

    PeakData peaks = gAnalyzer.RunSamplerPass(BAND_COUNT);
    auto Peaks = peaks.Peaks;
    // gDisplay.SetPeaks(BAND_COUNT, peaks);

    Serial.println("\n\nDone...\n\n");

    for (int i = 0; i < ARRAYSIZE(Peaks); i++)
      Serial.print(Peaks[i]);
  }
}

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

void setup()
{
  delay(500); // half a second of silence is a virtue

  Serial.begin(115200);

  printProgramHeader();
  printBoardInfo();

  Serial.println("configuring pins...");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("setting up adc..."); // BUGBUG: Hardcoded output
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

  Serial.printf("initializing sampler task... ");

  const int sampler_pinned_to_core = 0;
  const int sampler_priority = 5;
  // xTaskCreatePinnedToCore(
  //     SamplerLoop,
  //     "Sampler Loop",
  //     STACK_SIZE, nullptr,
  //     sampler_priority,
  //     &samplerTask,
  //     sampler_pinned_to_core);

  Serial.printf("Sampler on core %d with priority %d. \n", sampler_pinned_to_core, sampler_priority);

  Serial.println("Audio Sampler Launching...");
  Serial.printf("  FFT Size: %d bytes\n", MAX_SAMPLES);
  // g_SoundAnalyzer.StartInterrupts();

  Serial.println("");

  Serial.printf("initializing led task... ");

  const int led_pinned_to_core = 1;
  const int led_priority = 5;
  xTaskCreatePinnedToCore(
      StripAnimationLoop,
      "Strip Animation Loop",
      STACK_SIZE, nullptr,
      led_priority,
      &stripTask,
      led_pinned_to_core);

  Serial.printf("leds on core %d with priority %d. ", led_pinned_to_core, led_priority);

  Serial.println("READY.");

  // audial ready information
  beep(100, 500);
  beep(100, 1000);

  manager.g_EffectPointer = new RainbowEffect();
}

void loop()
{
}
