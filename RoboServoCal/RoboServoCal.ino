/*
 * Calibrate servos by writing them to stopped (90 deg) and
 * turning the calibration screw (above the cables)
 */
#include <Servo.h>

Servo ptServo;  // pt = port
Servo sbServo;  // sb = starboard

const int ptServoPin = 9;   // Motor control board port 2
const int sbServoPin = 10;  // Motor control board port 1

void setup()
{
  ptServo.attach(ptServoPin);
  sbServo.attach(sbServoPin);
}

void loop()
{
  ptServo.write(90);
  sbServo.write(90);
  delay(100);
}
