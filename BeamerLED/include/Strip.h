#pragma once

#include <FastLED.h>
#include <ArduinoJson.h>

#include "Utilities.h"
#include "StripSection.h"

using namespace std;

class IStrip
{
public:
    int Pin = 0;
    int LedCount = 3;
    CRGB *Data;
    std::vector<StripSection> Positions;
    virtual void compute() = 0;
};

template <int PIN>
class Strip : public IStrip
{
public:
    int Pin = PIN;

    int LedCount = 0; // how many leds over all sections there are

    CRGB *Data = new CRGB(); // the data array for all sections within

    std::vector<StripSection *> Positions; // the registered sections within the strip

    Strip(std::vector<StripSection *> positions) : Positions(positions)
    {
        for (int i = 0; i < Positions.size(); i++)
        {
            auto position = Positions[i];
            LedCount += position->Size;
        }

        FastLED.addLeds<WS2812B, PIN, GRB>(Data, LedCount);
    }

    void compute()
    {

#if DEBUG_TO_SERIAL
        Serial.println("compute position in strip...");
#endif

        // for each position in this strip...
        for (int i = 0; i < Positions.size(); i++)
        {
            StripSection position = *Positions[i]; // get the section of the strip
            CRGB *pData = position.Data;           // get the data within that section

            Serial.printf("for pos %d data is at %p}\n", position.Location, position.Data);

            // for each pixel in the section...
            for (int k = 0; k < position.Size; k++)
            {
                // compute the index of the strip based of the current pixel
                // within the current section
                int stripI = position.Offset + k;
                CRGB color = pData[k];

#if DEBUG_TO_SERIAL
                Serial.printf(
                    R"({"mappedFromTo": [%d, %d], "color":	)",
                    k,
                    stripI);

                PrintColor(color, true);
                Serial.println("}");
#endif

                // finally, assign the current section's pixel to the complete strip
                Data[stripI] = color;
            }
        }
    }
};
