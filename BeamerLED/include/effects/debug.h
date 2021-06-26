
#include "StripEffect.h"

// literally just prints debug informations about the effect
// and returns an debug pixels vector.
class DebugEffect : public StripEffect
{
public:
    bool uniqueSections = false;
    bool fixAlignment = true;

    Interpolation interpolation = Interpolation::Linear;

    vector<CRGB> draw(int l, int t, int as)
    {

#if DEBUG_TO_SERIAL
        Serial.println("debug effect active.");

        Serial.println("Arguments:");

        Serial.printf(
            R"([{"%d": []}, %d, %d])",
            l,
            t,
            as);

        Serial.println();
#endif

        vector<CRGB> r(2);
        r[0] = CRGB::Magenta;
        r[1] = CRGB::Lime;
        return r;
    }
};
