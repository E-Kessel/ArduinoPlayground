//  Turns the builtin LED in pin 13 off
//  if the button on pin 2 is pressed 

const unsigned int BUTTON = 2;
const unsigned int LED = 13;
bool buttonState = LOW;
bool ledState = !buttonState;

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledState);
}

void loop() {
  buttonState = digitalRead(BUTTON);
  ledState = !buttonState;
  digitalWrite(LED, ledState);
}
