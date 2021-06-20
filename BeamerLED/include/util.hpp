#pragma once

#define MS_PER_SECOND 1000

#define PERIOD_FROM_FREQ(f) (round(1000000 * (1.0 / f))) // Calculate period in microseconds (us) from frequency in Hz
#define FREQ_FROM_PERIOD(p) (1.0 / p * 1000000)          // Calculate frequency in Hz given the priod in microseconds (us)
#define IN_SECONDS(inMs) inMs / (float)MS_PER_SECOND

// mapFloat
//
// Given an input value x that can range from in_min to in_max, maps return output value between out_min and out_max
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// FPS
//
// Given a millisecond value for when the last frame took place and the current timestamp returns the number of
// frames per second, as low as 0.  Never exceeds 999 so you can make some width assumptions.
int FPS(unsigned long start, unsigned long end)
{
    unsigned long msDuration = end - start;
    float fpsf = 1.0f / (msDuration / (float)MS_PER_SECOND);
    return (int)fpsf;
}
