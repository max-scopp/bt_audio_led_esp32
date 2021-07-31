#pragma once

#define FASTLED_INTERNAL

#include <FastLED.h>
#include <pixeltypes.h> // Handy color and hue stuff

#include "ledgfx_dave.h"
#include "StripEffect.h"
#include "Utilities.h"
#include "StripSection.h"
#include "Strips.h"

using namespace std;

int g_Brightness = 30;  // 0-255 LED brightness scale
int g_PowerLimit = 3000; // 900mW Power Limit

// BouncingBallEffect bounce(2);
// CometEffect comet;
// FireEffect fire(NUM_LEDS);
// MarqueeEffect marquee;
// RainbowEffect rainbow;
// TwinkleEffect twinkle;

LEDGFX manager;

void UNUSED()
{

  // DrawPixels(dashboardRight, 0, dashboardRight.size, CRGB::BlueViolet);
  // Sequential Rainbows
  // byte hue = basehue;
  // for (int i = 0; i < NUM_LEDS; i++)
  //   DrawPixels(0, i, 1, CHSV(hue += 16, 255, 255));
  // basehue += 4;
  // delay(100);

  // Serial.printf("FPS: %d\r", lastFrame);

  // if (g_EffectPointer)
  // {
  //   g_EffectPointer->draw(dashboardLeft, millis());
  // }

  /*
    // RGB Spinners
    float b = beat16(60) / 65535.0f * FAN_SIZE;
    DrawPixels(b, 1, CRGB::Red, Sequential, 0);
    DrawPixels(b, 1, CRGB::Green, Sequential, 1);
    DrawPixels(b, 1, CRGB::Blue, Sequential, 2);
    */

  /*
    // Left to Right Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
        DrawPixels(0, b, CRGB::Cyan, LeftRight, iFan);
    */

  /*
    // Left to Right Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
      DrawPixels(0, b, CRGB::Cyan);
      */

  /*
    // Bottom up Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
        DrawPixels(0, b, CRGB::Green, BottomUp);
    */

  /*
    // Bottom up Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
        DrawPixels(0, b, CRGB::Green, TopDown);
    */

  /*
    // Simple Color Cycle
    static byte hue = 0;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawPixels(i, 1, CHSV(hue, 255, 255));
    hue += 4;
    */

  /*
    // Vertical Rainbow Wipe
    static byte basehue = 0;
    byte hue = basehue;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawPixels(i, 1, CHSV(hue+=8, 255, 255), BottomUp);
    basehue += 4;
    */

  /*
    // Horizontal Rainbow Stripe
    static byte basehue = 0;
    byte hue = basehue;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawPixels(i, 1, CHSV(hue+=16, 255, 255), LeftRight);
    basehue += 8;
    */

  /*
    // Rainbow Stripe Palette Effect
    static CRGBPalette256 pal(RainbowStripeColors_p);
    static byte baseColor = 0;
    byte hue = baseColor;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawPixels(i, 1, ColorFromPalette(pal, hue += 4), BottomUp);
    baseColor += 1;
    */

  /*
    // vu-Style Meter
    int b = beatsin16(30) * NUM_LEDS / 65535L;
    static const CRGBPalette256 vuPaletteGreen = vu_gpGreen;
    for (int i = 0; i < b; i++)
      DrawPixels(i, 1, ColorFromPalette(vuPaletteGreen, (int)(255 * i / NUM_LEDS)));
      */

  /*
    // Sequential Fire Fans
    static FireEffect fire(NUM_LEDS, 20, 100, 3, NUM_LEDS, true, false);
    fire.DrawFire();
    */

  /*
    // Bottom Up Fire Effect with extra sparking on first fan only
    static FireEffect fire(NUM_LEDS, 20, 140, 3, FAN_SIZE, true, false);
    fire.DrawFire(BottomUp);
    */

  /*
    // LeftRight (Wide) Fire Effect with extra sparking on first fan only
    static FireEffect fire(NUM_LEDS, 20, 140, 3, FAN_SIZE, true, false);
    fire.DrawFire(LeftRight);
    for (int i = 0; i < FAN_SIZE; i++)  // Copy end fan down onto others
    {
      g_LEDs[i] = g_LEDs[i + 2 * FAN_SIZE];             
      g_LEDs[i + FAN_SIZE] = g_LEDs[i + 2 * FAN_SIZE];
    }
    */

  // int b = beatsin16(30) * NUM_LEDS / 65535L;
  // static const CRGBPalette256 seawhawksPalette = vu_gpSeahawks;
  // for (int i = 0; i < NUM_LEDS; i++)
  //     DrawPixels(i, 1, ColorFromPalette(seawhawksPalette, beat8(64) + (int)(255 * i / NUM_LEDS)), BottomUp);

  // EVERY_N_MILLISECONDS(250)
  // {
  //     g_OLED.clearBuffer();
  //     g_OLED.setCursor(0, g_lineHeight);
  //     g_OLED.printf("FPS  : %u", FastLED.getFPS());
  //     g_OLED.setCursor(0, g_lineHeight * 2);
  //     g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));
  //     g_OLED.setCursor(0, g_lineHeight * 3);
  //     g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
  //     g_OLED.sendBuffer();
  // }
}
