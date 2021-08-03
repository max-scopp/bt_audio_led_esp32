#pragma once

#define FASTLED_INTERNAL

#include <Arduino.h>
#include <FastLED.h>
#include <cstdint>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

#include "Utilities.h"
#include "Constants.h"
#include "Easing.h"

#include "Strip.h"
#include "StripSection.h"
#include "Locations.h"

#include "PeakData.h"
#include "StripEffect.h"

// #include "ledgfx_dave.h"

using namespace std;

class StripManager
{
protected:
private:
    bool bootupCycle = true;
    float Brightness = 0.0;

    Strip<32> *dashboard = new Strip<32>(
        {new StripSection(
             Location::Front | Location::Left,
             "dashboard driver side",
             0,
             15),
         new StripSection(
             Location::Front | Location::Right,
             "dashboard driver side",
             15,
             30)});

    // YOU MUST USE `auto` AS STRIP ARGUMENT OR ELSE OBJECT SLICING OCCOURS
    template <typename Func>
    void each(Func f)
    {
        f(*dashboard);
    }

public:
    StripEffect *g_EffectPointer = nullptr;

    PeakData _peaks;

    float _peak1Decay[BAND_COUNT] = {0};
    float _peak2Decay[BAND_COUNT] = {0};

    unsigned long _lastPeak1Time[BAND_COUNT] = {0};

    void setup(uint32_t _brightness, uint32_t mwPowerLimit)
    {
        pinMode(LOOP_DEBUG_PIN, OUTPUT);

        Serial.printf("setup target brightness as %d ", _brightness);

        Brightness = _brightness;           // this is the internal intended brightness (can change!)
        FastLED.setBrightness(_brightness); // this is the default brightness for the strips before any render happened. (boot render may overwrite this!)
        // FastLED.setMaxPowerInMilliWatts(mwPowerLimit);

        each([](auto sm)
             {
                 for (int i = 0; i < sm.Positions.size(); i++)
                 {
                     auto Position = *sm.Positions[i];
                     Serial.printf("@%d [%d] %dL %dO %dS ", sm.Pin, Position.Index, Position.Location, Position.Offset, Position.Size);
                 }
             });
    }

    void bootupRender()
    {
        static int fadeInMs = 1e+3;
        static int end = millis() + fadeInMs;

        int now = millis();

        int d = end - now;
        double p = (double)(fadeInMs - d) / fadeInMs;
        double l = easeInOutQuint(p);

        if (d > 0)
        {
            FastLED.setBrightness((double)Brightness * l);
        }
        else
        {
            bootupCycle = false;
            FastLED.setBrightness(Brightness);
        }

#if DEBUG_TO_SERIAL
        Serial.printf("setBrightness  %f \t (L %f) \n", (double)Brightness * l, l);
#endif
    }

    void loop()
    {
        static const int RUN_EVERY = TARGET_FPS > 0 ? MS_PER_SECOND / TARGET_FPS : 0;
        static unsigned long ledsLastFrame = 0;
        volatile size_t gLedsFPS = 0;

        EVERY_N_MILLISECONDS(RUN_EVERY)
        {
            gLedsFPS = FPS(ledsLastFrame, millis());
            ledsLastFrame = millis();

#if SHOW_FPS
            // TODO: Make fancy Serial.printf
            Serial.printf("FPS: %d\r", gLedsFPS);
#endif

            FastLED.clear();

            DecayPeaks();

            render();
        }
    }

    StripSection *sectionForLocation(int location)
    {
        StripSection *ssfl = nullptr;

#if DEBUG_TO_SERIAL
        Serial.printf("Looking for location %x... ", location);
#endif

        // make sure to pass ssfl as ref down
        each([&](auto &sm) mutable
             {

#if DEBUG_TO_SERIAL
                 Serial.printf("each position (%i) on strip %d ...", sm.Positions.size(), sm.Pin);
#endif

                 for (int i = 0; i < sm.Positions.size(); i++)
                 {
                     StripSection Position = *sm.Positions[i];

                     Serial.printf("\nCheck %d === %d\n", Position.Location, location);

                     if (Position.Location == location)
                     {
                         ssfl = &Position;

#if DEBUG_TO_SERIAL
                         Serial.printf("hit! Data at %p for %d\n", Position.Data, Position.Location);
#endif

                         break;
                     }
                 }

#if DEBUG_TO_SERIAL
                 if (!ssfl)
                     Serial.println("not found.");
#endif
             });

        return ssfl;
    }

    int computePrefferedArraySizeForEffectDraw()
    {
        int m = 0;

        each([&](auto &sm)
             { m = max(m, sm.LedCount); });

        return m;
    }

    template <typename T>
    void applyAlignment(int location, vector<T> *arr)
    {
        bool doMirrorRight = location & Location::Right;
        bool doMirrorBack = location & Location::Back;

        if (doMirrorRight || doMirrorBack)
        {
            reverse(arr->begin(), arr->end());
        }
    }

    void render()
    {
        if (!g_EffectPointer)
        {
#if DEBUG_TO_SERIAL
            Serial.println("No effect to render");
#endif
        }
        else
        {
            if (bootupCycle)
            {
                bootupRender();
            }

            int prefferedArraySize = computePrefferedArraySizeForEffectDraw();
            StripBehaviour behaviour = g_EffectPointer->getBehaviour();

#if DEBUG_TO_SERIAL
            if (!behaviour.uniqueSections)
            {

                Serial.println("g_EffectPointer->createsUniqueSections() OPTION IS IGNORED; NOT IMPLEMENTED");
            }
#endif

            each([&](auto &sm)
                 {
                     for (int i = 0; i < sm.Positions.size(); i++)
                     {
                         StripSection Position = *sm.Positions[i];
                         vector<CRGB> r = g_EffectPointer->draw(Position.Location, millis(), prefferedArraySize);

                         if (behaviour.fixAlignment)
                         {
                             applyAlignment(Position.Location, &r);
                         }

                         int rS = r.size();
                         int s = Position.Size;

#if DEBUG_TO_SERIAL
                         Serial.printf("resize result from %d to %d\n", rS, s);
#endif

                         for (int i = 0; i < s; i++)
                         {
                             int iNext = i + 1 >= s ? i : i + 1;
                             int iPrev = i - 1 < 0 ? 0 : i - 1;

                             float perc = (float)i / (float)s;
                             float percNext = (float)iNext / (float)s;
                             float percPrev = (float)iPrev / (float)s;

                             int rI = rS * perc;
                             float overlay = perc - (float)rI / (float)rS;

                             int rINext = rI + 1 >= rS ? rI : rI + 1;
                             int rIPrev = rI - 1 < 0 ? 0 : rI - 1;

                             Interpolation interpolation = behaviour.interpolation;

#if DEBUG_TO_SERIAL
                             Serial.printf("map %d to %d (%f / %f) ", i, rI, overlay, perc);
#endif

                             if (!interpolation)
                             {
                                 Position.Data[i] = r[rI];
                             }
                             else
                             {
                                 CRGB rC = r[rI];

                                 // the quirky ternaries are for index sticking.
                                 CRGB rCNext = r[rINext];
                                 CRGB rCPrev = r[rIPrev];

                                 float amount;

                                 switch (interpolation)
                                 {
                                 case Interpolation::Linear:
                                 default:
                                     amount = perc;
                                     break;
                                 case Interpolation::Cubic:
                                     amount = easeInOutCubic(perc);
                                     break;
                                 }

                                 Position.Data[i] = r[rI];
                            //  CRGB computedColor = blend(
                            //      rC,
                            //      blend(
                            //          rCPrev,
                            //          rCNext,
                            //          ((float)i + 1 / (float)s) * 255),
                            //      ((float)i - 1 / (float)s) * 255);

#if DEBUG_TO_SERIAL

                                 Serial.print("blend colors (prev, now, next) ");
                                 PrintColor(rCPrev);
                                 PrintColor(rC);
                                 PrintColor(rCNext);

                                 Serial.printf(" with blend amount %f resulted in ", amount);
                                 // PrintColor(computedColor);

                                 Serial.println();
#endif
                             }

#if DEBUG_TO_SERIAL
                             Serial.println();
#endif
                         }
                     }
                 });
        }

        show();
    }

    void show()
    {
        // debug leds when we render
        auto lCycle = digitalRead(LOOP_DEBUG_PIN);
        digitalWrite(LOOP_DEBUG_PIN, !lCycle);

        // map each section to each strip
        each([](auto &sm)
             { sm.compute(); });

#if DEBUG_TO_SERIAL
        Serial.println("Final strip pixels are as follows:");

        Serial.print("[");
        each([](auto &sm)
             {
                 Serial.printf(R"({"pin": %d, "pixels": [)", sm.Pin);
                 for (int i = 0; i < sm.LedCount; i++)
                 {
                     auto pixel = sm.Data[i];

                     PrintColor(pixel, true);

                     Serial.print(",");
                 }
                 Serial.print("]},");
             });
        Serial.println("]");

#endif

        FastLED.show();
    }

    void setBrightness(float brightness, bool smooth = false)
    {
        Brightness = brightness;
        FastLED.setBrightness(Brightness);

        if (smooth)
        {
            throw runtime_error("Smooth is not supported yet");
        }
    }

    float getBrightness()
    {
        return Brightness;
    }

    // Display::SetPeaks
    //
    // Allows the analyzer to call and set the peak data for all of the bands at once.

    void SetPeaks(byte bands, PeakData peakData)
    {
        //Serial.print("SetPeaks: ");
        for (int i = 0; i < bands; i++)
        {
            _peaks = peakData;
            // Serial.printf("%f, ", _peaks.Peaks[i]);

            if (peakData.Peaks[i] > _peak1Decay[i])
            {
                _peak1Decay[i] = peakData.Peaks[i];
                _lastPeak1Time[i] = millis(); // For the white line top peak we track when it was set so we can age it out visually
            }
            if (peakData.Peaks[i] > _peak2Decay[i])
            {
                _peak2Decay[i] = peakData.Peaks[i];
            }
        }
        //Serial.println("");
    }

    // SpectrumDisplay::DecayPeaks
    //
    // Every so many ms we decay the peaks by a given amount

    void DecayPeaks()
    {
        static unsigned long lastDecay = 0;
        float seconds = (millis() - lastDecay) / (float)MS_PER_SECOND;
        lastDecay = millis();

        float decayAmount1 = max(0.0f, seconds * gPeakDecay);
        float decayAmount2 = seconds * PEAK2_DECAY_PER_SECOND;

        for (int iBand = 0; iBand < BAND_COUNT; iBand++)
        {
            _peak1Decay[iBand] -= min(decayAmount1, _peak1Decay[iBand]);
            _peak2Decay[iBand] -= min(decayAmount2, _peak2Decay[iBand]);
        }
    }
};

StripManager manager;
