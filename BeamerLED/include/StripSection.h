
#pragma once

#include <FastLED.h>

class StripSection
{
public:
    CRGB *Data = nullptr; // the led data to render within this section

    int Index; // the index of the section of all strips

    int Offset; // where the section starts
    int Size;   // how many leds are within this section

    int Location; // binary representation of the physical location

    int _Start; // prefer to not use
    int _End;   // prefer to not use

    StripSection(int location, std::string sectionName, int start, int end)
        : Location(location),
          _Start(start),
          _End(end)
    {
        static int SectionCounter = 0;

        Index = SectionCounter++;

        Size = end - start;
        Offset = start;

        Data = new CRGB[Size]{0};
    }
};
