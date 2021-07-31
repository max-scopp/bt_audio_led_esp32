//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        marque.h
//
// Description:
//
//   Draws a theatre-style marquee
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#include <FastLED.h>

#include "StripEffect.h"

class MarqueeEffect : public StripEffect
{
protected:
    byte j;
    int scroll;

public:
    MarqueeEffect() : j(0), scroll(0) {}

    void DrawMarquee()
    {
        j += 4;
        byte k = j;

        // Roughly equivalent to fill_rainbow(g_LEDs, NUM_LEDS, j, 8);

        CRGB c;
        for (int i = 0; i < NUM_LEDS; i++)
            FastLED.leds()[i] = c.setHue(k += 8);

        scroll++;

        for (int i = scroll % 5; i < NUM_LEDS - 1; i += 5)
        {
            FastLED.leds()[i] = CRGB::Black;
            FastLED.leds()[i + 1] = CRGB::Black;
        }

        delay(50);
    }

    void DrawMarqueeMirrored()
    {
        j += 4;
        byte k = j;

        // Roughly equivalent to fill_rainbow(g_LEDs, NUM_LEDS, j, 8);

        CRGB c;
        for (int i = 0; i < (NUM_LEDS + 1) / 2; i++)
        {
            FastLED.leds()[i] = c.setHue(k);
            FastLED.leds()[NUM_LEDS - 1 - i] = c.setHue(k);
            k += 8;
        }

        scroll++;

        for (int i = scroll % 5; i < NUM_LEDS / 2; i += 5)
        {
            FastLED.leds()[i] = CRGB::Black;
            FastLED.leds()[NUM_LEDS - 1 - i] = CRGB::Black;
        }

        delay(50);
    }

    void draw(StripSection controller, int t)
    {
        static bool mirrored = false;

        if (mirrored)
        {
            DrawMarqueeMirrored();
        }
        else
        {
            DrawMarquee();
        }
    }
};
