#include <Arduino.h>
#include <ringBuffer.h>
#define RING_BUF_PHOTOTRANS_SIZE 10

class robotOrientation{
  
  private:
  bool curIsFlipped = 0;
  
  
  bool lastIsFlipped = 0;
  unsigned long lastTransition = millis();
  int topPin;
  int bottomPin;

  const int delayTimeMS = 100;

  long topPhotoTransVals[RING_BUF_PHOTOTRANS_SIZE] = {0};
  ringBuffer topPhotoRingBuf = ringBuffer(topPhotoTransVals, RING_BUF_PHOTOTRANS_SIZE, 0);

  long bottomPhotoTransVals[RING_BUF_PHOTOTRANS_SIZE] = {0};
  ringBuffer bottomPhotoRingBuf = ringBuffer(bottomPhotoTransVals, RING_BUF_PHOTOTRANS_SIZE, 0);

  public:
  bool isFlippedResult = 0;

  robotOrientation(int topPin, int bottomPin) : topPin(topPin), bottomPin(bottomPin) {}

  void printDebugInfo() {
    USBSerial.printf("Top : %d  Bottom: %d \n", analogRead(topPin), analogRead(bottomPin) );
  }

  bool checkIsFlipped() {
    topPhotoRingBuf.update(analogRead(topPin));
    bottomPhotoRingBuf.update(analogRead(bottomPin));

    curIsFlipped = bottomPhotoRingBuf.getMaxVal() - topPhotoRingBuf.getMaxVal() > 0 ? 1 : 0;
    
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


