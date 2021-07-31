//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:
//
// Description:
//
//
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "StripEffect.h"
#include "ledgfx_dave.h"

static const CRGB TwinkleColors[] =
    {
        CRGB::Red,
        CRGB::Blue,
        CRGB::Purple,
        CRGB::Green,
        CRGB::Yellow};

class TwinkleEffect : public StripEffect
{
public:
    void DrawTwinkle()
    {
        static int passCount = 0;
        if (passCount++ == NUM_LEDS / 4)
        {
            passCount = 0;
            FastLED.clear(false);
        }
        FastLED.leds()[random(NUM_LEDS)] = TwinkleColors[random(0, ARRAYSIZE(TwinkleColors))];
        delay(200);
    }

    void draw(StripSection controller, int t)
    {
        DrawTwinkle();
    }
};
