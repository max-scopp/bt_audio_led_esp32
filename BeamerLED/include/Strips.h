#pragma once

#include <FastLED.h>
#include <cstdint>
#include <functional>
#include <algorithm>

#include "Utilities.h"
#include "Constants.h"

#include "Strip.h"
#include "StripSection.h"
#include "Locations.h"

#include "StripEffect.h"
#include "ledgfx_dave.h"

#define LOOP_DEBUG_PIN 13

class Strips
{
protected:
private:
    StripEffect *g_EffectPointer = nullptr;

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

    void computeData()
    {
        each([](auto &sm)
             { sm.compute(); });
    }

public:
    void setup(uint8_t brightness, uint32_t mwPowerLimit)
    {
        pinMode(LOOP_DEBUG_PIN, OUTPUT);

        FastLED.setBrightness(brightness);
        FastLED.setMaxPowerInMilliWatts(mwPowerLimit);
    }

    void show()
    {
        Serial.println("show");
        auto lCycle = digitalRead(LOOP_DEBUG_PIN);
        digitalWrite(LOOP_DEBUG_PIN, !lCycle);

        Serial.println("compute");
        computeData();

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
            drawFrame();
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

    void drawFrame()
    {
        // auto dashboardLeft = sectionForLocation(Location::Front | Location::Left);

        static int hue = 0;

        Serial.println();

        auto frontLeft = sectionForLocation(Location::Front | Location::Left);
        fill_rainbow(frontLeft->Data, frontLeft->Size, hue, 5);

        auto frontRight = sectionForLocation(Location::Front | Location::Right);
        fill_rainbow(frontRight->Data, frontRight->Size, hue, 5);
        flipArray(frontRight->Data, frontRight->Size);

        // fill_rainbow(frontLeft->Data, frontLeft->Size, hue, 20);
        // fill_rainbow(frontRight->Data, frontRight->Size, hue, 20);

        hue += 3;

        if (hue > 255)
        {
            hue = 0;
        }

        // if (dashboardLeft)
        // {
        //     DrawPixels(dashboardLeft, 20, dashboardLeft->Size, CRGB::Violet);
        // }

        show();
    }
};