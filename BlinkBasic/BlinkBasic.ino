
const unsigned int sleepTime = 500;
const unsigned int LED = 13;
bool state = LOW;

void setup() 
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, state);
}

void loop() 
{
  delay(sleepTime);
  state = !state;
  digitalWrite(LED, state);
}
