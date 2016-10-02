const unsigned int LED = 13;
const unsigned int BUTTON = 2;
const unsigned long DEBOUNCE = 10;

bool buttonState = LOW;
bool lastButtonState = LOW;
bool buttonOutput = LOW;
bool lastButtonOutput = LOW;
bool ledState = LOW;

bool stateChecking = false;

unsigned long changeStart = 0;
unsigned long currentTime = 0;
unsigned long i = 0;

void setup() 
{
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledState);
  Serial.begin(250000); //  LUDICROUS SPEED, GO!
  Serial.print("The button must be pressed for ");
  Serial.print(DEBOUNCE);
  Serial.println(" ms to register.");
}

void loop() 
{
  lastButtonState = buttonState;
  buttonState = digitalRead(BUTTON);
  currentTime = millis();
  
  if (!stateChecking && buttonState != lastButtonState)  //  Not checking and button just changed states
  {
    changeStart = currentTime;
    stateChecking = true;
  }
  else if (stateChecking && buttonState != lastButtonState) //  Were checking and button returned to previous state
  {
    stateChecking= false;
  }

  lastButtonOutput = buttonOutput;
  if (stateChecking && currentTime >= changeStart + DEBOUNCE)  //  Were still checking and the timer ran out
  {
    buttonOutput = buttonState;
    stateChecking = false;
  }
  
  //**********************************************************************************************************************
  //**********************************************************************************************************************
  
  if (buttonOutput != lastButtonOutput)
  {
    ledState = buttonOutput;
    digitalWrite(LED, ledState);
    Serial.print("The button state is now ");
    Serial.print(buttonOutput);
    Serial.print(" at ");
    Serial.print(double(currentTime) / 1000.00);
    Serial.print(" seconds from start after ");
    Serial.print(double(i) / 1000000.00);
    Serial.println(" million loop cycles.");  //  "The button state is now <value> at <value> seconds from start after <value> million loop cycles"
  }
  
  i++;
}
