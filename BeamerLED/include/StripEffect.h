#pragma once

#include "StripSection.h"

enum Interpolation
{
    None = 0,
    Linear = 1,
    Cubic = 2
};

class StripEffect
{
public:
    bool uniqueSections = false; // signifies whether or not each section shall run draw() seperately.
    bool fixAlignment = true;    // weather or not certain location should apply orientational changes to the result of their designated draw().
    Interpolation interpolation = Interpolation::Linear;

    virtual vector<CRGB> draw(int l, int t, int as); // the stuff you want to draw to an specific section of the strip(s). You get a location which is a bit shifted value of the enum Location and the current millis() if you want to depend on those variables. You must return any size of array to actually draw. You get a preffered size as third argument.
};
