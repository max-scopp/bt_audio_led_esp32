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

    StripBehaviour getBehaviour()
    {
        StripBehaviour b;

        return b;
    }

    void writeConfiguration(JsonObject o)
    {
    }

    void updateConfiguration(JsonObject updates)
    {
    }

    void DrawMarquee(vector<CRGB> leds)
    {
        int num_leds = leds.size();

        j += 4;
        byte k = j;

        // Roughly equivalent to fill_rainbow(g_LEDs, num_leds, j, 8);

        CRGB c;
        for (int i = 0; i < num_leds; i++)
            FastLED.leds()[i] = c.setHue(k += 8);

        scroll++;

        for (int i = scroll % 5; i < num_leds - 1; i += 5)
        {
            FastLED.leds()[i] = CRGB::Black;
            FastLED.leds()[i + 1] = CRGB::Black;
        }

        delay(50);
    }

    void DrawMarqueeMirrored(vector<CRGB> leds)
    {
        int num_leds = leds.size();

        j += 4;
        byte k = j;

        // Roughly equivalent to fill_rainbow(g_LEDs, num_leds, j, 8);

        CRGB c;
        for (int i = 0; i < (num_leds + 1) / 2; i++)
        {
            FastLED.leds()[i] = c.setHue(k);
            FastLED.leds()[num_leds - 1 - i] = c.setHue(k);
            k += 8;
        }

        scroll++;

        for (int i = scroll % 5; i < num_leds / 2; i += 5)
        {
            FastLED.leds()[i] = CRGB::Black;
            FastLED.leds()[num_leds - 1 - i] = CRGB::Black;
        }

        delay(50);
    }

    vector<CRGB> draw(int l, int t, int as)
    {
        static bool mirrored = false;
        vector<CRGB> leds(as);

        if (mirrored)
        {
            DrawMarqueeMirrored(leds);
        }
        else
        {
            DrawMarquee(leds);
        }

        return leds;
    }
};
