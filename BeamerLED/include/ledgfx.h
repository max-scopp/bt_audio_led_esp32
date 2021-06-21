//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        ledgfx.h
//
// Description:
//
//   LED Drawing Routines for Dave's Garage Tutorial series
//
// History:     OCt-18-2020     davepl      Created from main.cpp code
//---------------------------------------------------------------------------

#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include <sys/time.h> // For time-of-day

// Utility Macros

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000 / x)

// Simple definitions of what direction we're talking about

enum PixelOrder
{
  Sequential = 0,
  Reverse = 1,
  BottomUp = 2,
  TopDown = 4,
  LeftRight = 8,
  RightLeft = 16
};

DEFINE_GRADIENT_PALETTE(vu_gpGreen){
    0, 0, 4, 0,       // near black green
    64, 0, 255, 0,    // green
    128, 255, 255, 0, // yellow
    192, 255, 0, 0,   // red
    255, 255, 0, 0    // red
};

DEFINE_GRADIENT_PALETTE(gpSeahawks){
    0,
    0,
    0,
    4,
    64,
    3,
    38,
    58,
    128,
    0,
    21,
    50,
    192,
    78,
    167,
    1,
    255,
    54,
    87,
    140,
};

inline float RandomFloat()
{
  float r = random(1000000L) / 1000000.0f;
  return r;
}

inline double UnixTime()
{
  timeval tv = {0};
  gettimeofday(&tv, nullptr);
  return (double)(tv.tv_usec / 1000000.0 + (double)tv.tv_sec);
}

// FractionalColor
//
// Returns a fraction of a color; abstracts the fadeToBlack out to this function in case we
// want to improve the color math or do color correction all in one location at a later date.

CRGB ColorFraction(CRGB colorIn, float fraction)
{
  fraction = min(1.0f, fraction);
  return CRGB(colorIn).fadeToBlackBy(255 * (1.0f - fraction));
}

// DrawPixels
//
// Uses floating point math to draw a floating point number of pixels starting at a
// floating point offset into the strip

void DrawPixels(float fPos, float count, CRGB color)
{
  // Calculate how much the first pixel will hold
  float availFirstPixel = 1.0f - (fPos - (long)(fPos));
  float amtFirstPixel = min(availFirstPixel, count);
  float remaining = min(count, FastLED.size() - fPos);
  int iPos = fPos;

  // Blend (add) in the color of the first partial pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos++] += ColorFraction(color, amtFirstPixel);
    remaining -= amtFirstPixel;
  }

  // Now draw any full pixels in the middle

  while (remaining > 1.0f)
  {
    FastLED.leds()[iPos++] += color;
    remaining--;
  }

  // Draw tail pixel, up to a single full pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos] += ColorFraction(color, remaining);
  }
}
