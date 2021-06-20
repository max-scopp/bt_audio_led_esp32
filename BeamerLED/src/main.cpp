#include <Arduino.h>

// #include "ble_app_comms.hpp"
// #include "bt_audio_sink.hpp"
#include "led_strip.h"

#define PIN_BLUETOOTH_TO_CORE 1
#define PIN_LEDS_TO_CORE 0

TaskHandle_t Core0TaskHnd;
TaskHandle_t Core1TaskHnd;

// void createBluetoothTask(void *pvParameters)
// {
//   // initBleCommunicationChannel();
//   initBluetooth("E46 ESP32 Sink", 13);

//   for (;;)
//   {
//     //handleBluetooth();
//   }
// }

void createLedAnimationTask(void *pvParameters)
{
  initLEDs();

  for (;;)
  {
    drawLEDs();
  }
}

void setup()
{
  Serial.begin(115200);

  while (!Serial)
  {
  }

  Serial.println("");
  Serial.printf("initializing... ");

  xTaskCreatePinnedToCore(
      createLedAnimationTask,
      "LEDs",
      3000,
      NULL,
      1,
      &Core1TaskHnd,
      PIN_LEDS_TO_CORE);
  Serial.printf("LEDs on core %d... ", PIN_LEDS_TO_CORE);

  // xTaskCreatePinnedToCore(
  //     createBluetoothTask,
  //     "Bluetooth",
  //     3000,
  //     NULL,
  //     1,
  //     &Core0TaskHnd,
  //     PIN_BLUETOOTH_TO_CORE);

  // Serial.printf("Bluetooth on core %d... ", PIN_BLUETOOTH_TO_CORE);

  Serial.println("Initialized!");
}

void loop()
{
}
