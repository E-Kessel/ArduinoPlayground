// ETHAN: "IT'S ALIVE! IT'S ALLLIIIVVVEEEEEEE!!!"

// Whiteboards are cool

#define DIM(x) (sizeof(x)/sizeof(x[0])) // Dad's favorite macro
#define LAST(x) (DIM(x)-1)

struct SensorMapType
{
  float volts;
  float distance;
  float dvslope;
};
SensorMapType table[] = 
{
  // V, D
  {2.30, 10.0},
  {1.30, 20.0},
  {0.90, 30.0},
  {0.75, 40.0},
  {0.60, 50.0},
  {0.40, 80.0}
};
const long baudRate = 115200;
const int irPin = 0;
int rawIR;
float vIn;
float range;

void setup() 
{
  for (uint16_t i = 0; i < DIM(table) - 1; ++i)
  {
    table[i].dvslope = ((table[i+1].distance - table[i].distance) /
                        (table[i+1].volts - table[i].volts));
  }
  table[LAST(table)].dvslope = float(table[LAST(table)-1].dvslope);
  Serial.begin(baudRate);
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
  
  bool tooClose = (vIn > table[0].volts);
  bool tooFar   = (vIn < table[LAST(table)].volts);

  // If neither too close or far, interpolate
  if ( ! tooClose && ! tooFar)
  {
    // Search the table to find the indexes that
    // the input voltage is between high (inclusive) and low (exclusive)
    uint16_t index;
    for (index = 0; index < DIM(table); ++index)
    {
      if (vIn > table[index].volts)
      {
        --index;
        break;
      }
    }
    // Compute delta-V
    float deltaV = table[index].volts - vIn;
    // Compute delta-D = delta-V / dvslope
    ///float deltaD = table[index].distance - 
    // Compute D = Dindex - (Delta-V / dvslope)
    range = table[index].distance - (deltaV * table[index].dvslope);
    
    Serial.print("Range ~~ ");
    Serial.print(range);
    Serial.println(" cm.");
  }
  else
  {
    Serial.println("OUT OF RANGE");
  }
  delay(100);
}
