
#include "StripEffect.h"
#include "Constants.h"
#include "Utilities.h"

// literally just prints debug informations about the effect
// and returns an debug pixels vector.
class SoundVUDebugEffect : public StripEffect
{
public:
    bool uniqueSections = false;
    bool fixAlignment = true;

    Interpolation interpolation = Interpolation::Linear;

    vector<CRGB> draw(int l, int t, int as)
    {
        vector<CRGB> r(16);

        static int yVU = 0;

        static int iPeakVUy = 0;           // size (in LED pixels) of the VU peak
        static unsigned long msPeakVU = 0; // timestamp in ms when that peak happened so we know how old it is

        const int MAX_FADE = 256;

        // if (iPeakVUy > 1)
        // {
        //     int fade = MAX_FADE * (millis() - msPeakVU) / (float)MS_PER_SECOND;
        //     DrawVUPixels(iPeakVUy, yVU, fade);
        //     DrawVUPixels(iPeakVUy - 1, yVU, fade);
        // }

        int bars = mapFloat(gVU, 0, MAX_VU, 1, 16);
        bars = min(bars, 16);

        if (bars > iPeakVUy)
        {
            msPeakVU = millis();
            iPeakVUy = bars;
        }
        else if (millis() - msPeakVU > MS_PER_SECOND)
        {
            iPeakVUy = 0;
        }

        for (int i = 0; i < bars; i++)
            r[i] = i < iPeakVUy ? CRGB::Red : CRGB::Black;

        return r;
    }
};
