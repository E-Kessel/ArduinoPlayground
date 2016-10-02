/*
 * RoboServo1
 * Basic servo test for robot
 * Speed control is nonexistant, only small range of settings near
 * 90 deg produce speed variations, so we will treat these servos
 * as having only three states: 0 (full reverse), 90 (full stop),
 * and 180 (full ahead). RPM is still aplicable; we will still need
 * to calculate movement speed and rotation speed
 */
 
#include <Servo.h>

Servo ptServo;  // pt = port
Servo sbServo;  // sb = starboard

enum DirectionType
{
  STOP,
  FORWARD,
  BACKWARD
};

void connectServos (int ptPin, int sbPin)
{
  ptServo.attach(ptPin);
  sbServo.attach(sbPin);
}

void setServoDirection (DirectionType aDirection)
{
  switch (aDirection)
  {
    case FORWARD:
      // port servo backwards, starboard forwards
      ptServo.write(0);
      sbServo.write(180);
      break;
    case BACKWARD:
      // port servo forwards, starboard backwards
      ptServo.write(180);
      sbServo.write(0);
      break;
    case STOP:
      // Fall through
    default:
      // Stop if bad command, or fallen through
      ptServo.write(90);
      sbServo.write(90);
      break;
  }
}

const unsigned int ptServoPin = 9;      // Motor control board port 2
const unsigned int sbServoPin = 10;     // Motor control board port 1
const float rpm = 55.6;        // ~0.18 sec/60 deg
const float wheelDiaCm = 6.6;  //
const float wheelCircumCm = float(PI * wheelDiaCm);

void setup()
{
  connectServos(ptServoPin, sbServoPin);
  setServoDirection(STOP);
  Serial.begin(115200);
}

void loop()
{
  setServoDirection(STOP);
  Serial.println("STOP");
  delay(1000);
  setServoDirection(FORWARD);
  Serial.println("Forward");
  delay(1000);
  setServoDirection(STOP);
  Serial.println("STOP");
  delay(1000);
  setServoDirection(BACKWARD);
  Serial.println("Backward");
  delay(1000);
}
