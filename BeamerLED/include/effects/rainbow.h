
#include "base_effect.h"
#include "led_constants.h"

class RainbowFill : public StripEffect
{
public:
    void draw(int t)
    {
        static bool doRotate = true;
        static float rotationSpeed = 5;

        CRGB *leds = FastLED.leds();

        Serial.printf("rotate? %d ", doRotate);

        for (int iPos = 0; iPos < NUM_LEDS; iPos++)
        {
            float posFloat = (float)iPos / (float)NUM_LEDS;
            int hueRotation;

            if (doRotate)
            {
                int hueRotationInTime = (int)((posFloat * 255.0) + IN_SECONDS(t) + rotationSpeed);
                hueRotation = hueRotationInTime % 255;
            }
            else
            {
                hueRotation = posFloat * 255.0;
            }

            Serial.printf("%d[%d]; ", iPos, hueRotation);
            leds[iPos] = CHSV(hueRotation, 255, 255);
        }

        Serial.print("\n");
    }
};
