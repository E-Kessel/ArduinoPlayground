#include "StripManager.h"

enum stripPins
{
  aStrip  = 6
};

Adafruit_NeoPixel myStrips[]
{
  Adafruit_NeoPixel(16, aStrip, NEO_GRB + NEO_KHZ800),
};

StripManager myLights(myStrips, LENGTH(myStrips));

uint16_t i;
unsigned long nextTime = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Lightmanager starting");
  
  myLights.begin();

  Serial.println(RED);
  Serial.println(BLACK);
  
  myLights.setBrightness(0, 25);
  //myLights.startBlinking(0, BLUE, 120000, 280000, ORANGE);
  //myLights.startSweep(0, BLUE_LIGHT, 100000, 20, StripManager::UP, BLACK);
  //myLights.startBounce(0, BLUE, 50000, 8, ORANGE_DARK);
  //myLights.startBounce(0, RED, 200, 1);
  //myLights.fadeStrip(0, VIOLET, 1000000, GREEN);
  //myLights.startSnore(0, Adafruit_NeoPixel::Color(0x14,0xD0, 0x14), 2000000, Adafruit_NeoPixel::Color(0x66,0x66,0x66));
  //myLights.startSnore(0, VIOLET, 3000000);
  //myLights.startChase(0, GREEN, 5, 75000, 3, BLACK, StripManager::UP);
  //myLights.sparkleStrip(0, 100000);
  Serial.println("Function Set...");
}

void loop()
{
  myLights.runStrips();
  
  if (millis() >= nextTime)
  {
    nextTime += 5000;
    switch(i++ % 6)
    {
      case 0:
        myLights.startBlinking(0, BLUE, 120000, 120000, RED);
        break;
      case 1:
        myLights.startBounce(0, BLUE_LIGHT, 50000, 8, ORANGE_DARK);
        break;
      case 2:
        myLights.startBounce(0, RED, 70000, 1);
        break;
      case 3:
        myLights.startSnore(0, VIOLET, 3000000);
        break;
      case 4:
        myLights.startChase(0, ORANGE, 6, 75000, 2, BLACK, StripManager::UP);
        break;
      case 5:
        myLights.sparkleStrip(0, 100000);
        break;
      default:
        break;
    }
  }
}
