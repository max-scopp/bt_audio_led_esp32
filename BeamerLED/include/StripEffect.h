#pragma once

#include "StripSection.h"

class StripEffect
{
public:
    virtual void draw(StripSection *p, int t);
};
