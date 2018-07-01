#if ! defined (BUTTON_MANAGER_H)
#define BUTTON_MANAGER_H

#include "macros.h"
#include "MicroTimer.h"

#define BUTTONLOG
#if defined (BUTTONLOG)
  #define BUTTONLOG_PRINT(...) Serial.print(__VA_ARGS__)
  #define BUTTONLOG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
  #define BUTTONLOG_PRINT(...)
  #define BUTTONLOG_PRINTLN(...)
#endif

class Button
{
private:
protected:
  MicroTimer timer_;
  
  struct ButtonData
  {
    bool defaultState;
    
    bool input;
    bool lastInput;
    bool output;
    bool lastOutput;
    
    bool checking;
    bool hasChanged;
    bool beenPressed;
    bool beenReleased;
    
    uint64_t timeChange;
    uint64_t timeNow;
    uint64_t timePress;
    uint64_t timeRelease;

    uint16_t pressCount;
    uint16_t releaseCount;

    //  -------------------------------------------------------------------
    //  CONSTRUCTOR
    //  -------------------------------------------------------------------
    ButtonData(bool initState, const uint64_t &now)
    : defaultState(initState)
    , input(initState)
    , lastInput(initState)
    , output(initState)
    , lastOutput(initState)
    , checking(false)
    , hasChanged(false)
    , beenPressed(false)
    , beenReleased(false)
    , timeChange(now)
    , timeNow(now)
    , timePress(now)
    , timeRelease(now)
    , pressCount(0)
    , releaseCount(0)
    {}
  }buttonData_;
  //ButtonData buttonData_;
  
public:
  const uint8_t PIN;
  uint64_t      debounce_us;

  //  -------------------------------------------------------------------
  //  CONSTRUCTOR
  //  -------------------------------------------------------------------
  Button(uint8_t aPin, uint64_t aDebounce_us = 10000, bool initState = LOW)
  : buttonData_(initState, timer_.read())
  , PIN(aPin)
  , debounce_us(aDebounce_us)
  {
    pinMode(PIN, INPUT);
  }

  void run(void)
  {
    buttonData_.lastInput = buttonData_.input;
    buttonData_.input = digitalRead(PIN);
    buttonData_.timeNow = timer_.read();

    //  If we are not checking for changes and state just changed
    if (!buttonData_.checking && (buttonData_.input != buttonData_.lastInput))
    { //  Record change time and begin looking for changes
      buttonData_.timeChange = buttonData_.timeNow;
      buttonData_.checking = true;
    } //  If we are checking and state just changed
    else if (buttonData_.checking && (buttonData_.input != buttonData_.lastInput))
    { //  Stop checking
      buttonData_.checking = false;
    }

    //  If we are still checking and the timer has exceeded the debounce
    if (buttonData_.checking && (buttonData_.timeNow >= (buttonData_.timeChange + debounce_us)))
    { //  Set the output to the read value and stop checking
      buttonData_.lastOutput = buttonData_.output;
      buttonData_.output = buttonData_.input;
      buttonData_.checking = false;
      
      buttonData_.hasChanged = true;
      if (buttonData_.output != buttonData_.defaultState)
      {
        buttonData_.beenPressed = true;
        buttonData_.timePress = buttonData_.timeNow;
        buttonData_.pressCount++;
      }
      else
      {
        buttonData_.beenReleased = true;
        buttonData_.timeRelease = buttonData_.timeNow;
        buttonData_.releaseCount++;
      }
    }
  }

  bool getState(void)
  {
    return buttonData_.output;
  }

  bool getLastState(void)
  {
    return buttonData_.lastOutput;
  }

  bool getChange(void)
  {
    return (buttonData_.output != buttonData_.lastOutput);
  }

  bool hasChanged(void)
  {
    if(buttonData_.hasChanged)
    {
      buttonData_.hasChanged = false;
      return true;
    } else {return false;}
  }

  bool beenPressed(void)
  {
    if(buttonData_.beenPressed)
    {
      buttonData_.beenPressed = false;
      return true;
    } else {return false;}
  }

  bool beenReleased(void)
  {
    if(buttonData_.beenReleased)
    {
      buttonData_.beenReleased = false;
      return true;
    } else {return false;}
  }

  uint64_t getTime(void)
  {
    return buttonData_.timeNow;
  }
  
  uint64_t getLastPress(void)
  {
    return buttonData_.timePress;
  }

  uint64_t getLastRelease(void)
  {
    return buttonData_.timeRelease;
  }

  uint64_t getHoldTime(void)
  {
    if (buttonData_.timeRelease < buttonData_.timePress)
    {
      return (buttonData_.timeNow - buttonData_.timePress);
    }
    else
    {
      return (buttonData_.timeRelease - buttonData_.timePress);
    }
  }

  uint64_t getDTLastPress(void)
  {
    return (buttonData_.timeNow - buttonData_.timePress);
  }
private:
  uint64_t lastDTReleased_;
public:
  uint64_t getDTLastRelease(void)
  {
    
    // Local scoping of uint64_t had a problem that is not understood
    // where the variable would be cleared to zero prior to exiting
    // the function. This was worked around by making the workspace
    // static, but that also makes this function non-reentrant. Not
    // a problem for arduino UNO or similar single threaded applications.
    if (buttonData_.timeRelease >= buttonData_.timePress)
    {
      lastDTReleased_ = buttonData_.timeNow - buttonData_.timeRelease;
    }
    else
    {
      lastDTReleased_ = 0;
    }
    
    return lastDTReleased_;
  }

  uint16_t getPressCount(void)
  {
    return buttonData_.pressCount;
  }

  uint16_t getReleaseCount(void)
  {
    return buttonData_.releaseCount;
  }

  void resetPresses(void)
  {
    buttonData_.pressCount = 0;
  }
  
  void resetReleases(void)
  {
    buttonData_.releaseCount = 0;
  }
};


template <uint8_t NUM_BUTTONS>
class ButtonManager
{
private:
protected:
  Button *pButtons_;
  bool states_[NUM_BUTTONS];
  
public:
  static const uint8_t SIZE = NUM_BUTTONS; 
  //  -------------------------------------------------------------------
  //  CONSTRUCTOR
  //  -------------------------------------------------------------------
  ButtonManager(Button *pButtons)
  : pButtons_(pButtons)
  {
    for EVERY(NUM_BUTTONS)
    {
      states_[i] = pButtons_[i].getState();
    }    
  }

  void run(void)
  {
    for EVERY(NUM_BUTTONS)
    {
      pButtons_[i].run();
    }
  }

  bool * getStates(void)  //  Returns a pointer to an array of bools containing the button outputs
  {
    for EVERY(NUM_BUTTONS)
    {
      states_[i] = pButtons_[i].getState();
    }
    return states_;
  }

  uint8_t detectChanges(void) //  Returns the number of buttons that have changed since their last check
  {
    uint8_t count = 0;
    for EVERY(NUM_BUTTONS)
    {
      if (pButtons_[i].hasChanged()) {count++;}
    }
    return count;
  }

  uint8_t detectPressed(void) //  Returns the number of buttons that have been pressed since their last check
  {
    uint8_t count = 0;
    for EVERY(NUM_BUTTONS)
    {
      if (pButtons_[i].beenPressed()) {count++;}
    }
    return count;
  }

  uint8_t detectReleased(void) //  Returns the number of buttons that have been released since their last check
  {
    uint8_t count = 0;
    for EVERY(NUM_BUTTONS)
    {
      if (pButtons_[i].beenReleased()) {count++;}
    }
    return count;
  }
};

#endif
