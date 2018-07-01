#include "ButtonManager.h"
#include "StripManager.h"

#define BAUDRATE 250000
#define NUM_MODES 7

MicroTimer timer_us;

enum pins
{
  button1 = 2,
  button2 = 3,
  button3 = 4,
  aStrip = 6
};

Button myButtons[]
{
  Button(button1),
  Button(button2),
  Button(button3, 10000, HIGH)
};

Adafruit_NeoPixel myStrips[]
{
  Adafruit_NeoPixel(16, aStrip, NEO_GRB + NEO_KHZ800),
};

ButtonManager<LENGTH(myButtons)> myKeypad(myButtons);

StripManager myLights(myStrips, LENGTH(myStrips));

uint16_t i = 0;
bool power = true;
bool commandRecieved = true;

void setup()
{
  Serial.begin(BAUDRATE);
  timer_us.mark();

  myLights.begin();
  myLights.setBrightness(0,25);
}

void loop()
{
  myKeypad.run();
  myLights.runStrips();

  if (myButtons[0].beenPressed())
  {
    power = !power;
    i = 0;
    commandRecieved = true;
  }

  if (myButtons[1].beenPressed())
  {
    DEBUG_PRINTLN("goin down");
    if (i <= 0)
    {
      DEBUG_PRINTLN("to top");
      i = NUM_MODES - 1;
    }
    else {i--;}
    commandRecieved = true;
  }
  else if (myButtons[2].beenPressed())
  {
    DEBUG_PRINTLN("goin up");
    if (i >= NUM_MODES - 1)
    {
      DEBUG_PRINTLN("to 0");
      i = 0;
    }
    else {i++;}
    commandRecieved = true;
  }

  if (commandRecieved)
  {
    if (power)
    {
      switch(i)
      {
        case 0:
          myLights.startSnore(0, GREEN, 4000000);
          break;
        case 1:
          myLights.startBlinking(0, RED, 250000, 250000, BLUE);
          break;
        case 2:
          myLights.startBounce(0, BLUE_LIGHT, 50000, 8, ORANGE_DARK);
          break;
        case 3:
          myLights.startBounce(0, RED, 70000, 1);
          break;
        case 4:
          myLights.startChase(0, ORANGE, 6, 75000, 2, BLACK, StripManager::UP);
          break;
        case 5:
          myLights.startChase(0, MAGENTA, 3, 90000, 1, VIOLET_DARK, StripManager::DOWN);
          break;
        case 6:
          myLights.sparkleStrip(0, 100000);
          break;
        default:
          break;
      }
    }
    else
    {
      myLights.turnStripOff(0);
    }
    commandRecieved = false;
  }

}
