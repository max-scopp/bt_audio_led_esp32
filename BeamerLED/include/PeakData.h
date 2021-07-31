#include "Utilities.h"

#pragma once

// PeakData class
//
// Simple data class that holds the music peaks for up to 32 bands.  When the sound analyzer finishes a pass, its
// results are simplified down to this small class of band peaks.

class PeakData
{
public:
    float Peaks[BAND_COUNT];

    PeakData()
    {
        for (int i = 0; i < ARRAYSIZE(Peaks); i++)
            Peaks[i] = 0.0f;
    }
};