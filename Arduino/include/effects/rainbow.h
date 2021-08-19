
#include "StripEffect.h"

class RainbowEffect : public StripEffect
{
public:
    bool doRotate = true;

    float hueDensity = 5;
    float hueDelta = 12;

    StripBehaviour getBehaviour()
    {
        StripBehaviour b;

        return b;
    }

    void writeConfiguration(JsonObject o)
    {
        o["r"] = doRotate;
        o["dens"] = hueDensity;
        o["delta"] = hueDelta;
    }

    void updateConfiguration(JsonObject updates)
    {
        doRotate = (bool)(updates.containsKey("r") ? updates["r"] : doRotate);
        hueDensity = (float)(updates.containsKey("dens") ? updates["dens"] : hueDensity);
        hueDelta = (float)(updates.containsKey("delta") ? updates["delta"] : hueDelta);

        Serial.printf("doRotate: %d \n", doRotate);
        Serial.printf("hueDensity: %d \n", hueDensity);
        Serial.printf("hueDelta: %d \n", hueDelta);
    }

    vector<CRGB> draw(int l, int t, int as)
    {
        vector<CRGB> r(as);

        for (int iPos = 0; iPos < as; iPos++)
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

            auto c = CRGB();

            r[iPos] = c.setHSV(hueRotation % 255, 255, 255);
        }

        return r;
    }
};
