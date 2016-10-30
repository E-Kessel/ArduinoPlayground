#if ! defined (NEOPIXELDISPLAYCOMMANDS_H)
#define NEOPIXELDISPLAYCOMMANDS_H

#include "colors.h"

void fill (Adafruit_NeoPixel &strip, uint16_t color)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

#endif
