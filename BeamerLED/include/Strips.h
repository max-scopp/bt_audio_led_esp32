#pragma once

#include <FastLED.h>
#include <cstdint>
#include <functional>
#include <algorithm>
#include <vector>

#include "Utilities.h"
#include "Constants.h"

#include "Strip.h"
#include "StripSection.h"
#include "Locations.h"
#include "Easing.h"

#include "StripEffect.h"
#include "ledgfx_dave.h"

#define LOOP_DEBUG_PIN 13

class Strips
{
protected:
private:
    bool bootupCycle = true;
    float Brightness = 0.0;

    Strip<12> *dashboard = new Strip<12>(
        {new StripSection(
             Location::Front | Location::Left,
             "dashboard driver side",
             0,
             10),
         new StripSection(
             Location::Front | Location::Right,
             "dashboard passanger side",
             10,
             20)});

    // YOU MUST USE `auto` AS STRIP ARGUMENT OR ELSE OBJECT SLICING OCCOURS
    template <typename Func>
    void each(Func f)
    {
        f(*dashboard);
    }

public:
    StripEffect *g_EffectPointer = nullptr;

    void setup(uint32_t _brightness, uint32_t mwPowerLimit)
    {
        pinMode(LOOP_DEBUG_PIN, OUTPUT);

        Brightness = _brightness;
        Serial.printf("setup target brightness as %d\n", Brightness);

        FastLED.setBrightness(_brightness);
        FastLED.setMaxPowerInMilliWatts(mwPowerLimit);
    }

    void bootupRender()
    {
        static int fadeInMs = 800;
        static int end = millis() + fadeInMs;

        int now = millis();

        int d = end - now;
        double p = (double)(fadeInMs - d) / fadeInMs;
        double l = easeOutQuad(p);

        if (d > 0)
        {
            FastLED.setBrightness(Brightness * l);
        }
        else
        {
            bootupCycle = false;
            FastLED.setBrightness(Brightness);
        }

        // static int end = ((int)millis()) + 3000;

        // int now = millis();
        // int p = now / end;

        // if (p >= 1.0f)
        // {
        //     bootupCycle = false;
        //     FastLED.setBrightness(Brightness);
        // }
        // else
        // {
        //     auto bootupBrightness = (double)Brightness * p;
        //     Serial.printf("brightness bootup is %f (%d / %d) \n", p, now, end);
        //     FastLED.setBrightness(bootupBrightness);
        // }
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
        if (g_EffectPointer)
        {
            if (bootupCycle)
            {
                bootupRender();
            }

            int prefferedArraySize = computePrefferedArraySizeForEffectDraw();

            if (!g_EffectPointer->uniqueSections)
            {
                Serial.println("g_EffectPointer->createsUniqueSections() OPTION IS IGNORED; NOT IMPLEMENTED");
            }

            each([&](auto &sm)
                 {
                     for (int i = 0; i < sm.Positions.size(); i++)
                     {
                         StripSection Position = *sm.Positions[i];
                         vector<CRGB> r = g_EffectPointer->draw(Position.Location, millis(), prefferedArraySize);

                         PrintColor(r[0]);

                         if (g_EffectPointer->fixAlignment)
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

                             Interpolation interpolation = g_EffectPointer->interpolation;

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

        // FIXME: DEBUG CODE, REMOVE!
        // static int hue = 0;

        // auto frontLeft = sectionForLocation(Location::Front | Location::Left);
        // fill_rainbow(frontLeft->Data, frontLeft->Size, hue, 5);

        // auto frontRight = sectionForLocation(Location::Front | Location::Right);
        // fill_rainbow(frontRight->Data, frontRight->Size, hue, 5);
        // reverse_in_place(frontRight->Data, frontRight->Size);

        // // fill_rainbow(frontLeft->Data, frontLeft->Size, hue, 20);
        // // fill_rainbow(frontRight->Data, frontRight->Size, hue, 20);

        // hue += 3;

        // if (hue > 255)
        // {
        //     hue = 0;
        // }

        // // if (dashboardLeft)
        // // {
        // //     DrawPixels(dashboardLeft, 20, dashboardLeft->Size, CRGB::Violet);
        // // }

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
};
