
#include "StripEffect.h"

// literally just prints debug informations about the effect
// and returns an debug pixels vector.
class SoundDebugEffect : public StripEffect
{
public:
    bool uniqueSections = false;
    bool fixAlignment = true;

    Interpolation interpolation = Interpolation::Linear;

    vector<CRGB> draw(int l, int t, int as)
    {
#if DEBUG_TO_SERIAL
        for (int i = 0; i < ARRAYSIZE(manager._peaks.Peaks); i++)
            Serial.print(manager._peaks.Peaks[i]);

        Serial.printf("\r");

        // Serial.println("Arguments:");

        // Serial.printf(
        //     R"([{"%d": []}, %d, %d])",
        //     l,
        //     t,
        //     as);

        // Serial.println();
#endif

        vector<CRGB> r(BAND_COUNT);
        int s = ARRAYSIZE(manager._peak2Decay);

        for (int i = 0; i < s; i++)
        {
            float hue = (manager._peak2Decay[i] + 0.15f) * 256.0f;
            float value = easeInSine(manager._peak2Decay[i]) * 255.0f;

            // Serial.print(manager._peaks.Peaks[i]);
            r[i] = CHSV(hue, 255, value);
        }

        // Serial.print("\r");

        return r;
    }
};
