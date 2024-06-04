#include <Arduino.h>

class robotOrientation{
  
  private:
  bool curIsFlipped = 0;
  
  
  bool lastIsFlipped = 0;
  unsigned long lastTransition = millis();
  int topPin;
  int bottomPin;

  const int delayTimeMS = 500;

  public:
  bool isFlippedResult = 0;

  robotOrientation(int topPin, int bottomPin) : topPin(topPin), bottomPin(bottomPin) {}


  bool checkIsFlipped() {
    curIsFlipped = analogRead(bottomPin) - analogRead(topPin) > 0 ? 1 : 0;
    
    if (curIsFlipped == 1 && lastIsFlipped == 0)
      lastTransition = millis();
    if (curIsFlipped == 1 && millis() - lastTransition > delayTimeMS)
      isFlippedResult = 1;

    if (curIsFlipped == 0 && lastIsFlipped == 1) 
      lastTransition = millis();
    if (curIsFlipped == 0 && millis() - lastTransition > delayTimeMS)
      isFlippedResult = 0;

    lastIsFlipped = curIsFlipped;
    return isFlippedResult;
  } 
};


