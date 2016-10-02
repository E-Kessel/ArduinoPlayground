const int baudRate = 115200;
const int irPin = 0;
int rawIR;
float vIn;
float range;

void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  rawIR = analogRead(irPin);
  Serial.print("Raw = ");
  Serial.print( rawIR);
  Serial.print(". ");
  vIn = (float(rawIR)/1023.0)*5.0;
  Serial.print("vIn = ");
  Serial.print(vIn);
  Serial.print(" V. ");
  // V ~~ 1/d * 22.5
  // Vd ~~ 22.5
  // d ~~ 22.5/V
  range = 22.5 / vIn;
  Serial.print("Range ~~ ");
  Serial.print(range);
  Serial.println(" cm.");
  delay(100);
}
