#pragma once

#include <FastLED.h>

using namespace std;

#define MS_PER_SECOND 1000

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000 / x)

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
    int FPS = (int)fpsf;
    if (FPS > 999)
        FPS = 999;
    return FPS;
}

void PrintColor(CRGB c, bool json = false)
{
    if (json)
    {
        Serial.printf("{\"r\": %d, \"g\": %d, \"b\": %d}", c.r, c.g, c.b);
    }
    else
    {
        Serial.printf("rgb(%d, %d, %d) ", c.r, c.g, c.b);
    }
}

/// way to simple beep where pitch can not be higher than 1000.
void beep(int durationInMs, uint32_t pitch)
{
    bool onOff = 1;

    int start = millis();
    int end = start + durationInMs;

    do
    {
        digitalWrite(BUZZER_PIN, onOff);
        onOff = !onOff;

        vTaskDelay(configTICK_RATE_HZ / pitch);
    } while (millis() < end);

    digitalWrite(BUZZER_PIN, LOW);
}

template <typename T>
void reverse_in_place(vector<T> a)
{
    int size = a.size();
    Serial.printf("reverse_in_place.size %d\n", size);

    auto arr = &a;

    int i = size - 1;
    int j = 0;
    while (i > j)
    {
        Serial.printf("i j %d %d\n ", i, j);
        auto temp = arr[i];
        a[i] = a[j];
        a[j] = *temp;
        i--;
        j++;
    }
}

CRGB mix(CRGB a, CRGB b, float m)
{
    int bm = b * 255;
    return blend(a, b, bm);
}
