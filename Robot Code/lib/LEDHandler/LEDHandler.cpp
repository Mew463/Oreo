#include "LEDHandler.h"

bool ledToggleState = 0;
unsigned long lastdelayToggle = millis();
unsigned long currentDelayToggle = millis();

Colors lastColor1;
Colors lastColor2;

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(2, LEDPIN, 0, TYPE_GRB); 

void init_led() {
    strip.begin();
    strip.setBrightness(100);	
}

void setLeds(Colors color) {
    static unsigned long lastLedShowing = 0;
    if (millis() - lastLedShowing > 5) {
        if (color == BLACK)
            strip.setAllLedsColorData(0, 0, 0);
        else if (color == WHITE)
            strip.setAllLedsColorData(255, 255, 255);
        else 
            strip.setAllLedsColorData(strip.hsv2rgb(color*30, 100, 100)); 
        strip.show();   
        lastLedShowing = millis();
    }
    
}

void toggleLeds(Colors color1, Colors color2, int delayMS) {
    if (color1 != lastColor1 || color2 != lastColor2 || millis() - lastdelayToggle > delayMS + 50) { 
        lastdelayToggle = millis(); 
        ledToggleState = 0;
        setLeds(color1);
    }
    currentDelayToggle = millis();
    
    if (currentDelayToggle - lastdelayToggle > delayMS) {
        ledToggleState = !ledToggleState;
        lastdelayToggle = currentDelayToggle;
    }

    if (ledToggleState) 
        setLeds(color1);
    else
        setLeds(color2);

    lastColor1 = color1;
    lastColor2 = color2;
}