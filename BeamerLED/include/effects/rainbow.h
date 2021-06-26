
#include "StripEffect.h"

class RainbowEffect : public StripEffect
{
public:
    bool uniqueSections = false;
    bool fixAlignment = true;

    Interpolation interpolation = Interpolation::Linear;

    vector<CRGB> draw(int l, int t, int as)
    {
        vector<CRGB> r(as);

        static bool doRotate = true;

        static float hueDensity = 5;
        static float hueDelta = 12;

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
