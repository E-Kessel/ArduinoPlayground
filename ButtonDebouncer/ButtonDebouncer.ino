#include "ButtonManager.h"

#define BAUDRATE 250000

MicroTimer timer_us;

Button myButtons[]
{
  Button(2),
  Button(3, 250000),
  Button(4, 10000, HIGH)
};

ButtonManager<LENGTH(myButtons)> myKeypad(myButtons);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  timer_us.mark();
}

void loop()
{
  myKeypad.run();

  //DEBUG_PRINTLN(myButtons[0].getDTLastRelease() / 1000000.0);

//  Serial.print((myButtons[0].getLastPress() / 1000000.0));
//  Serial.print(" ");
//  Serial.print((myButtons[0].getLastRelease() / 1000000.0));
//  Serial.print(" ");
//  Serial.print((myButtons[0].getHoldTime() / 1000000.0));
//  Serial.println(" ");
  uint64_t x = 1;
  if (timer_us.read() >= 1000000)
  {
    timer_us.mark();
    //myButtons[0].getDTLastRelease();
    for EVERY(myKeypad.SIZE)
    {
      Serial.print("Button ");
      Serial.print(i);
      Serial.print(" was pressed for ");
      Serial.print((myButtons[i].getHoldTime() / 1000000.0));
      Serial.print(" seconds ");
      x = myButtons[i].getDTLastRelease();
//      timer_us.println(x);
      Serial.print(x / 1000000.0);
      Serial.println(" seconds ago...");
    }
    Serial.println("");
  }
  
//  Serial.println(myButtons[0].getReleaseCount());

//  if (timer_us.read() >= 1000000)
//  {
//    timer_us.mark();
//    Serial.print(myKeypad.detectChanges());
//    Serial.println(" button(s) have changed.");
//    Serial.print(myKeypad.detectPressed());
//    Serial.println(" button(s) have been pressed.");
//    Serial.print(myKeypad.detectReleased());
//    Serial.println(" button(s) have been released.");
//  }
}
