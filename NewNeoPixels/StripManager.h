#if ! defined (STRIP_MANAGER_H)
#define STRIP_MANAGER_H

#include "macros.h"
#include "colors.h" //  Includes Adafruit Neopixel library
#include "MicroTimer.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

class StripManager
{
private:
  // Basic fill command, fills all pixels in a strip with a single color and shows
  void fill (Adafruit_NeoPixel &strip, uint32_t color)
  {
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, color);
    }
    strip.show();
  }

  //  Color unpack function, returns an array of uint8_t
  //  from packed uint32_t color in format of [r,g,b]  
  uint8_t * unpackColor(uint32_t c)
  {
    static uint8_t output[3];
    
    output[0] = (uint8_t)(c >> 16);
    output[1] = (uint8_t)(c >> 8);
    output[2] = (uint8_t)c;
    
    //output[] = {r,g,b};
    return output;
  }

public: //  Public enums
  enum functions
  {
    OFF     = 0,
    SOLID   = 1,  //  Instantly fill entire strip with one color
    BLINK   = 2,  //  Alternate between 2 colors on a period
    SWEEP   = 3,  //  Sweep a cluster of pixels across the strip
    BOUNCE  = 4,  //  Sweep a cluster of pixels back and forth across the strip
    FLOOD   = 5,  //  Fill the strip one pixel at a time from an end
    FADE    = 6,  //  Smooth transition between two colors
    SNORE   = 7,  //  Fade back and forth between two colors
    CHASE   = 8,  //  Pattern of two colors chasing after each other
    SPARKLE = 9,  //  Random Colors
  };

  enum directions
  {
    UP      =  1, //  From strip start to end; determines to use period0 when blinking
    NO_DIR  =  0, //  No direction
    DOWN    = -1, //  From strip end to start; determines use of period1 when blinking
  };
   
protected:
  Adafruit_NeoPixel *pStrips_;
  const uint16_t NUM_STRIPS;
  MicroTimer timer_;

  //  --------------------------------------------------------------------
  //  Strip state structure
  //  --------------------------------------------------------------------
  struct StripState
  {
    functions   function;
    uint32_t    color0;
    uint32_t    color1;
    uint16_t    pSize0;         //  Size of clusters/paterns
    uint16_t    pSize1;
    uint8_t     brightness;
    int16_t     index;          //  Progress in current function's pattern
    directions  indexDirection; //  Direction of current function's pattern
    long long   period0_usec;
    long long   period1_usec;
    long long   nextUpdate_usec;

    StripStates()
    {
      function          = OFF;
      color0            = BLACK;
      color1            = WHITE;
      pSize0            = 0;
      pSize1            = 0;
      brightness        = 2;  //  Minimum visible
      index             = 0;
      indexDirection    = NO_DIR;
      period0_usec      = 0;  //  Forever
      period1_usec      = 0;
      nextUpdate_usec   = 0;
    }
  };
  StripState *pStripStates_;

  //  --------------------------------------------------------------------
  //  PROTECTED FUNCTIONS
  //  --------------------------------------------------------------------
  //  These are the functions that are run by runStrips() that update each
  //  strip based upon its function
  //  --------------------------------------------------------------------

  //  --------------------------------------------------------------------
  //  --------------------------------------------------------------------
  void nextBlink(uint16_t i)
  {
    if (pStripStates_[i].indexDirection == UP)
    {
      pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;
      fill(pStrips_[i], pStripStates_[i].color0);
      pStripStates_[i].indexDirection = DOWN;
    }
    else if (pStripStates_[i].indexDirection == DOWN)
    {
      pStripStates_[i].nextUpdate_usec += pStripStates_[i].period1_usec;  //  Overwrite next update based upon different period
      fill(pStrips_[i], pStripStates_[i].color1);
      pStripStates_[i].indexDirection = UP;
    }
  }
  
  //  --------------------------------------------------------------------
  //  --------------------------------------------------------------------
  void nextSweep(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;
    
    uint16_t n = pStrips_[i].numPixels();
    
    switch (pStripStates_[i].indexDirection)
    {
      case UP:
        Serial.print("Index Direction: ");
        Serial.println(pStripStates_[i].indexDirection);
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          //  If difference between index and current pixel is between 0 and
          //  and pattern size, set to secondary color
          Serial.print((pStripStates_[i].index - (int16_t)pixel));
          Serial.print(" ");
          Serial.print(((int16_t)pStripStates_[i].pSize0));
          if (((pStripStates_[i].index - (int16_t)pixel) <= (int16_t)pStripStates_[i].pSize0) &&
              ((pStripStates_[i].index - (int16_t)pixel) > 0))
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
            Serial.println(" =X");
          }
          else
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
            Serial.println(" =O");
          }
        }
        Serial.println("");
        //  If at end of sweep, exit to solid color
        Serial.println(pStripStates_[i].index - (int16_t)pStripStates_[i].pSize0);
        if (pStripStates_[i].index - (int16_t)pStripStates_[i].pSize0 >= (int16_t)n)
        {
          Serial.println("SOLID");
          pStripStates_[i].function = SOLID;
          pStripStates_[i].period0_usec = 0;
        }
        break;
      case DOWN:
        Serial.print("Index Direction: ");
        Serial.println(pStripStates_[i].indexDirection);
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          //  If difference between current pixed and index is between 0 and
          //  and pattern size, set to secondary color
          Serial.print((int16_t)pixel - pStripStates_[i].index);
          Serial.print(" ");
          Serial.print(((int16_t)pStripStates_[i].pSize0));
          if (((((int16_t)pixel - pStripStates_[i].index) + 1) <= (int16_t)pStripStates_[i].pSize0) &&
              ((((int16_t)pixel - pStripStates_[i].index) + 1) > 0))
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
            Serial.println(" =X");
          }
          else
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
            Serial.println(" =0");
          }
        }
        //  If at end of sweep, exit to solid color
        Serial.println((pStripStates_[i].index + (int16_t)pStripStates_[i].pSize0));
        if (pStripStates_[i].index + (int16_t)pStripStates_[i].pSize0 <= 0)
        {
          Serial.println("SOLID");
          pStripStates_[i].function = SOLID;
          pStripStates_[i].period0_usec = 0;
        }
        break;
      default:
        break;
    }
    
    pStripStates_[i].index += pStripStates_[i].indexDirection;
    pStrips_[i].show();
  }

  //  --------------------------------------------------------------------
  //  --------------------------------------------------------------------
  void nextBounce(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;
    
    uint16_t n = pStrips_[i].numPixels();

    for (uint16_t pixel = 0; pixel < n; ++pixel)
    {
      Serial.print((int16_t)pixel - pStripStates_[i].index);
      Serial.print(" ");
      Serial.print((int16_t)pStripStates_[i].pSize0);
      if ((((int16_t)pixel - pStripStates_[i].index) < (int16_t)pStripStates_[i].pSize0) &&
           ((int16_t)pixel - pStripStates_[i].index) >= 0)
      {
        Serial.println(" =X");
        pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
      }
      else
      {
        Serial.println(" =O");
        pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
      }
    }

    if ((pStripStates_[i].indexDirection == UP) &&
       ((pStripStates_[i].index + pStripStates_[i].pSize0) >= n))
    {
      Serial.println("Switching to DOWN");
      pStripStates_[i].indexDirection = DOWN;
    }
    else if ((pStripStates_[i].indexDirection == DOWN) &&
             (pStripStates_[i].index <= 0))
    {
      Serial.println("Switching to UP");
      pStripStates_[i].indexDirection = UP;
    }

    pStripStates_[i].index += pStripStates_[i].indexDirection;
    pStrips_[i].show();
  }

  //  --------------------------------------------------------------------
  //  --------------------------------------------------------------------
  void nextFlood(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;
    
    uint16_t n = pStrips_[i].numPixels();

    switch (pStripStates_[i].indexDirection)
    {
      case UP:
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          if (pixel < pStripStates_[i].index)
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
          }
          else
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
          }
        }
        //  If at end of flood, exit to solid color
        if (pStripStates_[i].index > n)
        {
          pStripStates_[i].function = SOLID;
          pStripStates_[i].color0 = pStripStates_[i].color1;
          pStripStates_[i].period0_usec = 0;
        }
        break;
      case DOWN:
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          if (pixel > pStripStates_[i].index)
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
          }
          else
          {
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
          }
        }
        //  If at end of flood, exit to solid color
        if (pStripStates_[i].index < 0)
        {
          pStripStates_[i].function = SOLID;
          pStripStates_[i].color0 = pStripStates_[i].color1;
          pStripStates_[i].period0_usec = 0;
        }
        break;
      default:
        break;
    }

    pStripStates_[i].index += pStripStates_[i].indexDirection;
    pStrips_[i].show();
  }

  //  -------------------------------------------------------------------
  //  -------------------------------------------------------------------
  void nextFade(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;

    uint8_t *pColor;
    
    pColor = unpackColor(pStripStates_[i].color0);
    uint8_t r0 = *(pColor + 0);
    uint8_t g0 = *(pColor + 1);
    uint8_t b0 = *(pColor + 2);
    
    pColor = unpackColor(pStripStates_[i].color1);
    uint8_t r1 = *(pColor + 0);
    uint8_t g1 = *(pColor + 1);
    uint8_t b1 = *(pColor + 2);
    
    float progress = (float)pStripStates_[i].index / 256.0;

    Serial.print(r0);
    Serial.print(",");
    Serial.print(g0);
    Serial.print(",");
    Serial.print(b0);
    
    Serial.print(" =(");
    Serial.print(progress);
    Serial.print(")> ");
    
    Serial.print(r1);
    Serial.print(",");
    Serial.print(g1);
    Serial.print(",");
    Serial.print(b1);
    Serial.print(" = ");

    uint8_t r = uint8_t(r1 * progress) + uint8_t(r0 * (1.0 - progress));
    uint8_t g = uint8_t(g1 * progress) + uint8_t(g0 * (1.0 - progress));
    uint8_t b = uint8_t(b1 * progress) + uint8_t(b0 * (1.0 - progress));
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.println(b);
    
    uint32_t progressColor = Adafruit_NeoPixel::Color(r,g,b);
    //  If at end of fade, exit to solid color
    if (progress >= 1)
    {
      pStripStates_[i].function = SOLID;
      pStripStates_[i].color0 = pStripStates_[i].color1;
      pStripStates_[i].period0_usec = 0;
    }
    
    fill(pStrips_[i], progressColor);
    pStripStates_[i].index++;
  }

  //  -------------------------------------------------------------------
  //  -------------------------------------------------------------------
  void nextSnore(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;

    uint8_t *pColor;
    
    pColor = unpackColor(pStripStates_[i].color0);
    uint8_t r0 = *(pColor + 0);
    uint8_t g0 = *(pColor + 1);
    uint8_t b0 = *(pColor + 2);
    
    pColor = unpackColor(pStripStates_[i].color1);
    uint8_t r1 = *(pColor + 0);
    uint8_t g1 = *(pColor + 1);
    uint8_t b1 = *(pColor + 2);
    
    float progress = (float)pStripStates_[i].index / 256.0;
//    uint32_t progressColor = uint32_t(pStripStates_[i].color1 * progress) + uint32_t(pStripStates_[i].color0 * (1.0 - progress));

    uint8_t r = uint8_t(r1 * progress) + uint8_t(r0 * (1.0 - progress));
    uint8_t g = uint8_t(g1 * progress) + uint8_t(g0 * (1.0 - progress));
    uint8_t b = uint8_t(b1 * progress) + uint8_t(b0 * (1.0 - progress));

    uint32_t progressColor = Adafruit_NeoPixel::Color(r,g,b);
    
    if (progress >= 1)
    {
      pStripStates_[i].indexDirection = DOWN;
    }
    else if (progress <= 0)
    {
      pStripStates_[i].indexDirection = UP;
    }
    
    fill(pStrips_[i], progressColor);
    pStripStates_[i].index += pStripStates_[i].indexDirection;
  }

  //  -------------------------------------------------------------------
  //  -------------------------------------------------------------------
  void nextChase(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;

    uint16_t n = pStrips_[i].numPixels();

    switch(pStripStates_[i].indexDirection)
    {
      case UP:
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          Serial.print(pStripStates_[i].index - (int16_t)pixel);
          Serial.print(" % ");
          Serial.print((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1);
          Serial.print(" = ");
          Serial.print((pStripStates_[i].index - (int16_t)pixel) % ((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1));
          Serial.print(" vs. ");
          Serial.print((int16_t)pStripStates_[i].pSize0);
          if ((pStripStates_[i].index - (int16_t)pixel) % ((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1) < (int16_t)pStripStates_[i].pSize0)
          {
            Serial.println(" = X");
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
          }
          else
          {
            Serial.println(" = O");
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
          }
        }
        Serial.println();
        break;
      case DOWN:
        for (uint16_t pixel = 0; pixel < n; ++pixel)
        {
          Serial.print(pStripStates_[i].index + (int16_t)pixel);
          Serial.print(" % ");
          Serial.print((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1);
          Serial.print(" = ");
          Serial.print((pStripStates_[i].index + (int16_t)pixel) % ((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1));
          Serial.print(" vs. ");
          Serial.print((int16_t)pStripStates_[i].pSize0);
          if ((pStripStates_[i].index + (int16_t)pixel) % ((int16_t)pStripStates_[i].pSize0 + (int16_t)pStripStates_[i].pSize1) < (int16_t)pStripStates_[i].pSize0)
          {
            Serial.println(" = X");
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color0);
          }
          else
          {
            Serial.println(" = O");
            pStrips_[i].setPixelColor(pixel, pStripStates_[i].color1);
          }
        }
        Serial.println();
        break;
      default:
        break;
    }

      pStripStates_[i].index += abs(pStripStates_[i].indexDirection);
      pStrips_[i].show();
  }
    
  //  -------------------------------------------------------------------
  //  -------------------------------------------------------------------
  void nextSparkle(uint16_t i)
  {
    pStripStates_[i].nextUpdate_usec += pStripStates_[i].period0_usec;
    
    uint16_t n = pStrips_[i].numPixels();
//    uint8_t bright = (random(1,16) * 16) - 16;
    
    for (uint16_t pixel = 0; pixel < n; ++pixel)
    {
      uint16_t cMax = random(255,512);

      uint8_t x = random (0,256);
      uint8_t y = random (0,256);

      uint8_t r;
      uint8_t g;
      uint8_t b;

      switch (random(1,4))
      {
        case 1:
          r = x;
          cMax -= x;
          if (y > cMax) {y = cMax;}
          switch (random(1,3))
          {
            case 1:
              g = y;
              cMax -= y;
              b = cMax;
              break;
            case 2:
              b = y;
              cMax -= y;
              g = cMax;
              break;
          }
          break;
        case 2:
          g = x;
          cMax -= x;
          if (y > cMax) {y = cMax;}
          switch (random(1,3))
          {
            case 1:
              r = y;
              cMax -= y;
              b = cMax;
              break;
            case 2:
              b = y;
              cMax -= y;
              r = cMax;
              break;
          }
          break;
        case 3:
          b = x;
          cMax -= x;
          if (y > cMax) {y = cMax;}
          switch (random(1,3))
          {
            case 1:
              r = y;
              cMax -= y;
              g = cMax;
              break;
            case 2:
              g = y;
              cMax -= y;
              r = cMax;
              break;
          }
          break;
      }
      
      //pStrips_[i].setBrightness(10 + (rand() % 15));
//      uint8_t r = (random(1,16) * random(1,16)) - 1;
//      uint8_t g = (random(1,16) * random(1,16)) - 1;
//      uint8_t b = (random(1,16) * random(1,16)) - 1;
//
      uint8_t bright = (random(1,16) * 16) - 16;
      
      r *= bright;
      g *= bright;
      b *= bright;
      
      uint32_t c = Adafruit_NeoPixel::Color(r,g,b);
            
      pStrips_[i].setPixelColor(pixel, c);
    }
    pStrips_[i].show();
  }    
  
public: //  Public functions + constructor
  //  -------------------------------------------------------------------
  //  CONSTRUCTOR
  //  -------------------------------------------------------------------
  StripManager(Adafruit_NeoPixel *pStrips, const uint16_t aNumStrips)
  : pStrips_(pStrips)
  , NUM_STRIPS(aNumStrips)
  {
    pStripStates_ = new StripState[NUM_STRIPS];  //  New array of strip states for each number of strips
  }

  //  --------------------------------------------------------------------
  //  begin() - initializes all strips for this object; maximum brightness
  //  defaults to 2, all pixels off
  //  --------------------------------------------------------------------
  void begin(void)
  {
    for (uint16_t i = 0; i < NUM_STRIPS; i++)
    {
      pStrips_[i].begin();
      pStrips_[i].setBrightness(pStripStates_[i].brightness);
      fill(pStrips_[i], BLACK);
    }
  }

  //  --------------------------------------------------------------------
  //  setBrightness(strip, brightness)
  //  - Set strip max brightness to brightness
  //  --------------------------------------------------------------------
  void setBrightness(uint16_t i, uint8_t brightness)
  {
    pStripStates_[i].brightness = brightness;
  }

  //  --------------------------------------------------------------------
  //  turnStripOff(strip) - turns strip off
  //  --------------------------------------------------------------------
  void turnStripOff(uint16_t i)
  {
    pStripStates_[i].function          = OFF;
//    pStripStates_[i].color0            = BLACK;
//    pStripStates_[i].color1            = WHITE;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
//    pStripStates_[i].index             = 0;
//    pStripStates_[i].indexDirection    = NO_DIR;
//    pStripStates_[i].period0_usec      = 0;
//    pStripStates_[i].period1_usec      = 0;
//    pStripStates_[i].nextUpdate_usec   = 0;
  }

  //  --------------------------------------------------------------------
  //  setStripSolid(strip, color) - fill strip with solid color
  //  --------------------------------------------------------------------
  void setStripSolid(uint16_t i, uint32_t color)
  {
    pStripStates_[i].function          = SOLID;
    pStripStates_[i].color0            = color;
//    pStripStates_[i].color1            = WHITE;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
//    pStripStates_[i].index             = 0;
//    pStripStates_[i].indexDirection    = NO_DIR;
//    pStripStates_[i].period0_usec      = 0;
//    pStripStates_[i].period1_usec      = 0;
//    pStripStates_[i].nextUpdate_usec   = 0;
  }

  //  --------------------------------------------------------------------
  //  startBlinking(strip, colorOn, periodOff, periodOn = periodOff, colorOff = BLACK)
  //  - fill strip blinking between a color and another (default black),
  //  color0 on for period0, color1 on for period1
  //  --------------------------------------------------------------------
  void startBlinking(uint16_t i, uint32_t colorOn, long long periodOff, long long periodOn = 0, uint32_t colorOff = BLACK)
  {
    if (periodOn == 0)
    {
      periodOn = periodOff;
    }
    
    pStripStates_[i].function          = BLINK;
    pStripStates_[i].color0            = colorOff;
    pStripStates_[i].color1            = colorOn;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
//    pStripStates_[i].index             = 0;
    pStripStates_[i].indexDirection    = UP;
    pStripStates_[i].period0_usec      = periodOff;
    pStripStates_[i].period1_usec      = periodOn;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  startSweep(strip, colorOn, uDelay, gSize = 1, sDirection = UP, colorOff = BLACK)
  //  - start sweep of gSize group of colorOn pixels sDirection across
  //  strip with uDelay between updates and colorOff backround
  //  --------------------------------------------------------------------
  void startSweep(uint16_t i, uint32_t colorOn, long long uDelay, uint16_t gSize = 1, directions sDirection = UP, uint32_t colorOff = BLACK)
  {
    uint16_t n = pStrips_[i].numPixels();
    
    pStripStates_[i].function          = SWEEP;
    pStripStates_[i].color0            = colorOff;
    pStripStates_[i].color1            = colorOn;
    pStripStates_[i].pSize0            = gSize;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
    if (sDirection == UP)
    {
      pStripStates_[i].index             = 0;
    }
    else
    {
      pStripStates_[i].index             = n;
    }
    pStripStates_[i].indexDirection    = sDirection;
    pStripStates_[i].period0_usec      = uDelay;
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  startBounce(strip, colorOn, uDelay, gSize = 1, colorOff = BLACK)
  //  - bounces a gSize group of colorOn pixels across the strip with
  //  uDelay between updates, colorOff background, and starting in 
  //  sDirection
  //  --------------------------------------------------------------------
  void startBounce(uint16_t i, uint32_t colorOn, long long uDelay, uint16_t gSize = 1, uint32_t colorOff = BLACK)
  {
    uint16_t n = pStrips_[i].numPixels();
    
    pStripStates_[i].function          = BOUNCE;
    pStripStates_[i].color0            = colorOff;
    pStripStates_[i].color1            = colorOn;
    pStripStates_[i].pSize0            = gSize;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
//    if (sDirection == UP)
//    {
    pStripStates_[i].index             = 0;
//    }
//    else
//    {
//      pStripStates_[i].index             = n;
//    }
    pStripStates_[i].indexDirection    = UP;
    pStripStates_[i].period0_usec      = uDelay;
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  fadeStrip(strip, colorNew, period, colorOld = BLACK)
  //  - fade strip from colorOld to colorNew over period
  //  --------------------------------------------------------------------
  void fadeStrip(uint16_t i, uint32_t colorNew, long long period, uint32_t colorOld = BLACK)
  {
    pStripStates_[i].function          = FADE;
    pStripStates_[i].color0            = colorOld;
    pStripStates_[i].color1            = colorNew;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
    pStripStates_[i].index             = 0;
//    pStripStates_[i].indexDirection    = NO_DIR;
    pStripStates_[i].period0_usec      = (long long)(period / 256);
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  startSnore(strip, colorOn, period, colorOff = BLACK)
  //  - fade strip from colorOld to colorNew and back over period
  //  --------------------------------------------------------------------
  void startSnore(uint16_t i, uint32_t colorOn, long long period, uint32_t colorOff = BLACK)
  {
    pStripStates_[i].function          = SNORE;
    pStripStates_[i].color0            = colorOff;
    pStripStates_[i].color1            = colorOn;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
    pStripStates_[i].index             = 0;
    pStripStates_[i].indexDirection    = UP;
    pStripStates_[i].period0_usec      = (long long)(period / 512);
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  startChase(strip, color_0, size1, uDelay, size0 = 1, color_1 = BLACK, sDirection = UP)
  //  - starts pattern chase of size0 color_0 pixels between size1 color_1
  //  pixels with uDelay between updates and direction of sDirection
  //  --------------------------------------------------------------------
  void startChase(uint16_t i, uint32_t color_0, uint16_t size1, long long uDelay, uint16_t size0 = 1, uint32_t color_1 = BLACK, directions sDirection = UP)
  {
    uint16_t n = pStrips_[i].numPixels();
    
    pStripStates_[i].function          = CHASE;
    pStripStates_[i].color0            = color_0;
    pStripStates_[i].color1            = color_1;
    pStripStates_[i].pSize0            = size0;
    pStripStates_[i].pSize1            = size1;
//    pStripStates_[i].brightness        = 2;
    pStripStates_[i].index             = n;
    pStripStates_[i].indexDirection    = sDirection;
    pStripStates_[i].period0_usec      = uDelay;
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  sparkleStrip(strip, uDelay)
  //  - fills strip with random colors every uDelay
  //  --------------------------------------------------------------------
  void sparkleStrip(uint16_t i, long long uDelay)
  {
    pStripStates_[i].function          = SPARKLE;
//    pStripStates_[i].color0            = BLACK;
//    pStripStates_[i].color1            = WHITE;
//    pStripStates_[i].pSize0            = 0;
//    pStripStates_[i].pSize1            = 0;
//    pStripStates_[i].brightness        = 2;
//    pStripStates_[i].index             = 0;
//    pStripStates_[i].indexDirection    = NO_DIR;
    pStripStates_[i].period0_usec      = uDelay;
//    pStripStates_[i].period1_usec      = 0;
    pStripStates_[i].nextUpdate_usec   = timer_.read();
  }

  //  --------------------------------------------------------------------
  //  getFunction(strip)
  //  - returns the strip's current function
  //  --------------------------------------------------------------------
  functions getFunction(uint16_t i) {return pStripStates_[i].function;}
  
  //  --------------------------------------------------------------------
  //  runStrips() - updates all strips
  //  --------------------------------------------------------------------
  void runStrips(void)
  {
    for (uint16_t i = 0; i < NUM_STRIPS; i++)
    {
      //  Update strip brightness immediately
      pStrips_[i].setBrightness(pStripStates_[i].brightness);

      //  Strips with period of 0 (aka no period) are run here
      if (pStripStates_[i].period0_usec == 0)
      {
        //  Switch case for strip function
        switch (pStripStates_[i].function)
        {
          case OFF:
            fill(pStrips_[i], BLACK);
            break;
          case SOLID:
            fill(pStrips_[i], pStripStates_[i].color0);
            break;
          default:
            //  Do nothing for all other functions
            break;
        }
      }
      //  Periodic strips (period != 0) run here
      else
      {
        //  Get current time in microseconds
        long long now_usec = timer_.read();

        if (now_usec >= pStripStates_[i].nextUpdate_usec)
        {
          //Serial.println("Function Switch Entered...");
          switch (pStripStates_[i].function)
          {
            case OFF:
              fill(pStrips_[i], BLACK);
              pStripStates_[i].period0_usec = 0;  //  Set period to 0 to prevent having to read timer for this strip again
              break;
            case SOLID:
              fill(pStrips_[i], pStripStates_[i].color0);
              pStripStates_[i].period0_usec = 0;  //  Set period to 0 to prevent having to read timer for this strip again
              break;
            case BLINK:
              nextBlink(i);
              break;
            case SWEEP:
              Serial.println("  Sweep");
              nextSweep(i);
              break;
            case BOUNCE:
            Serial.println("  Bounce");
              nextBounce(i);
              break;
            case FLOOD:
              nextFlood(i);
              break;
            case FADE:
              nextFade(i);
              break;
            case SNORE:
              nextSnore(i);
              break;
            case CHASE:
              nextChase(i);
              break;
            case SPARKLE:
              nextSparkle(i);
              break;
  
            default:
              break;  
          }
        }
      }
    }
  }
};

#endif
