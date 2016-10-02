// ----------------------------------------------------------------------------------------------------
// Useful macros
// ----------------------------------------------------------------------------------------------------

#define EVER (;;)
#define DIM(x)  (sizeof(x)/sizeof(x[0]))
#define LAST(x) (DIM(x)-1)

struct Button
{
  // A struct is just like a class but with all members public by default
  // A class has all members private by default
  // keyword public:, protected:, and private: can be used to change access scope
  
  const unsigned int PIN;
  const unsigned long DEBOUNCE;
  
  bool buttonState;
  bool lastButtonState = buttonState;
  bool buttonOutput = buttonState;
  bool lastButtonOutput = buttonState;

  bool stateChecking;           
  unsigned long changeStart;

  // Constructors
  Button() {}
  
  Button(const unsigned int &aPin,                 //  Default Settings
         const unsigned long &aDebounceTime_ms = 10,
         const bool &initialButtonState = LOW)     //  Default start state, only change if button is pulled-up
  : PIN(aPin)
  , DEBOUNCE(aDebounceTime_ms)
  , buttonState(initialButtonState)
  , lastButtonState(buttonState)
  , buttonOutput(buttonState)
  , lastButtonOutput(buttonState)
  , stateChecking(false)                            //  I don't recomend changing these
  , changeStart(0)
  {
       // Nothing more to do here     
  }
};

Button buttons[] =
{
  // Pin, Debounce Period (Default 10 ms), Default Start State (Only change if button is pulled-up)
  Button(2),
  Button(3,250),
  Button(4,1000)
};

unsigned long currentTime = 0;
unsigned long i = 0;

void setup() 
{
  Serial.begin(250000); //  LUDICROUS SPEED, GO!
  Serial.print("Serial connection established...\n");
  Serial.print("Initalizing buttons...\n\n");
  for (int n = 0; n < DIM(buttons); n++)
  {
    Serial.print("Initalizing button on pin ");
    Serial.print(buttons[n].PIN);
    Serial.print(" to input...\n");
    pinMode(buttons[n].PIN, INPUT);
    Serial.print("Done.\n\n");
  }
  Serial.print("Button initialization complete.\n\n");
  Serial.print("Each button must be heald in a state for the following times to register:\n");
  for (int n = 0; n < DIM(buttons); n++)
  {
    Serial.print("Button on pin ");
    Serial.print(buttons[n].PIN);
    Serial.print(": ");
    Serial.print(buttons[n].DEBOUNCE);
    Serial.print(" ms.\n");
  }
  Serial.print("\n");
}

void loop() 
{
  for(int n = 0; n < DIM(buttons); n++)
  {
    buttons[n].lastButtonState = buttons[n].buttonState;    //  There has got to be a better way than to buttons[n]. everything
    buttons[n].buttonState = digitalRead(buttons[n].PIN);
    currentTime = millis();
    
    if (!buttons[n].stateChecking && buttons[n].buttonState != buttons[n].lastButtonState)  //  Not checking and button just changed states
    {
      buttons[n].changeStart = currentTime;
      buttons[n].stateChecking = true;
    }
    else if (buttons[n].stateChecking && buttons[n].buttonState != buttons[n].lastButtonState) //  Were checking and button returned to previous state
    {
      buttons[n].stateChecking= false;
    }
  
    buttons[n].lastButtonOutput = buttons[n].buttonOutput;
    if (buttons[n].stateChecking && currentTime >= buttons[n].changeStart + buttons[n].DEBOUNCE)  //  Were still checking and the timer ran out
    {
      buttons[n].buttonOutput = buttons[n].buttonState;
      buttons[n].stateChecking = false;
    }
    
    if (buttons[n].buttonOutput != buttons[n].lastButtonOutput)
    {
      Serial.print("The button on pin ");
      Serial.print(buttons[n].PIN);
      Serial.print(" now has the state of ");
      Serial.print(buttons[n].buttonOutput);
      Serial.print(" at ");
      Serial.print(double(currentTime) / 1000.00);
      Serial.print(" seconds from start after ");
      Serial.print(double(i) / 1000000.00);
      Serial.print(" million loop cycles. (");  //  "The button on pin <value> now has the state of <value> at <value> seconds from start after <value> million loop cycles"
      Serial.print(double(i) / double(currentTime));
      Serial.print(" thousand loop cycles per second)\n");
    }
  }
  
  i++;
}
