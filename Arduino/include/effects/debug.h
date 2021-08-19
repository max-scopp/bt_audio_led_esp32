
#include "StripEffect.h"

// literally just prints debug informations about the effect
// and returns an debug pixels vector.
class DebugEffect : public StripEffect
{
public:
    StripBehaviour getBehaviour()
    {
        StripBehaviour b;

        b.uniqueSections = true;
        b.interpolation = Interpolation::None;

        return b;
    }

    void writeConfiguration(JsonObject o)
    {
    }

    void updateConfiguration(JsonObject updates)
    {
    }

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

        vector<CRGB> r(as);

        for (size_t i = 0; i < as; i++)
        {
            r[i] = i % 2 ? CRGB::Magenta : CRGB::Lime;
        }

        r[0] = CRGB::Red;
        r[as - 1] = CRGB::Blue;

        return r;
    }
};
