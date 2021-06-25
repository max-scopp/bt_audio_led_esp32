
#include "StripEffect.h"

class RainbowEffect : public StripEffect
{
public:
    void draw(StripSection *p, int t)
    {
        static bool doRotate = true;

        static float hueDensity = 5;
        static float hueDelta = 12;

        CRGB *leds = FastLED.leds();

        for (int iPos = 0; iPos < NUM_LEDS; iPos++)
        {
            float progress = hueDensity * iPos;
            int hueRotation;

            if (doRotate)
            {
                hueRotation = t / hueDelta - progress;
            }
            else
            {
                hueRotation = progress;
            }

            leds[iPos] = CHSV(hueRotation % 255, 255, 255);
        }
    }
};
