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

StripSection *driver = new StripSection(
    Location::Front | Location::Left,
    "dashboard driver side",
    0,
    10);

StripSection *passenger = new StripSection(
    Location::Front | Location::Right,
    "dashboard passanger side",
    10,
    20);

class Strips
{
protected:
private:
    StripEffect *g_EffectPointer = nullptr;

    Strip<12> *dashboard = new Strip<12>(
        {driver,
         passenger});

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
        auto lCycle = digitalRead(LOOP_DEBUG_PIN);
        digitalWrite(LOOP_DEBUG_PIN, !lCycle);

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

#if DEBUG_TO_SERIAL
        Serial.printf("Looking for location %x... ", location);
#endif

        StripSection *ssfl = nullptr;

        // make sure to pass ssfl as ref down
        each([&ssfl, location](auto &sm) mutable
             {

#if DEBUG_TO_SERIAL
                 Serial.printf("each position (%i) on strip %d ...", sm.Positions.size(), sm.Pin);
#endif

                 for (int i = 0; i < sm.Positions.size(); i++)
                 {
                     auto Position = sm.Positions[i];

                     if (Position.Location == location)
                     {

#if DEBUG_TO_SERIAL
                         Serial.println("hit!");
#endif

                         ssfl = &Position;
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

        auto frontLeft = sectionForLocation(Location::Front | Location::Left);
        auto frontRight = sectionForLocation(Location::Front | Location::Right);

        fill_solid(frontLeft->Data, frontLeft->Size, CRGB::Red);
        fill_solid(frontRight->Data, frontRight->Size, CRGB::Blue);

        Serial.printf("driver %p    passanger %p\n", driver, passenger);
        Serial.printf("same? %i\n", driver == passenger);

        Serial.printf("%d %d %d %d",
                      frontLeft->_Start,
                      frontLeft->_End,
                      frontRight->_Start,
                      frontRight->_End);

        // fill_rainbow(frontLeft->Data, frontLeft->Size, hue, 20);
        // fill_rainbow(frontRight->Data, frontRight->Size, hue, 20);

        hue += 1;

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
