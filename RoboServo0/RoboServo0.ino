/*
 * RoboServo0
 * Basic servo test for robot
 */
#include <Servo.h>

Servo ptServo;  // pt = port
Servo sbServo;  // sb = starboard

const int ptServoPin = 9;   // Motor control board port 2
const int sbServoPin = 10;  // Motor control board port 1
const float rpm = 55.6;    // ~0.18 sec/60 deg

void setup()
{
  ptServo.attach(ptServoPin);
  sbServo.attach(sbServoPin);

  Serial.begin(115200);
}

int i;

void loop()
{
  for (i = 0; i <= 180; ++i)
  {
    Serial.println(i);
    ptServo.write(i);
    sbServo.write(i);
    delay(50);
  }
  for (i = 180; i >= 0; --i)
  {
    Serial.println(i);
    ptServo.write(i);
    sbServo.write(i);
    delay(50);
  }
}
