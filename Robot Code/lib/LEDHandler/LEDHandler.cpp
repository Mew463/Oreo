#include "LEDHandler.h"

bool ledToggleState = 0;
unsigned long lastdelayToggle = millis();
unsigned long currentDelayToggle = millis();

Colors lastColor1;
Colors lastColor2;

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, LEDPIN, 0, TYPE_GRB); 

void init_led() {
    strip.begin();
    strip.setBrightness(100);	
}

void setLeds(Colors color) {
    if (color == BLACK)
	    strip.setLedColorData(0, 0, 0, 0);
	else if (color == WHITE)
        strip.setLedColorData(0, 255, 255, 255);
    else 
        strip.setLedColorData(0, strip.hsv2rgb(color*30, 100, 100));
    // delay(1);
    strip.show();
}

void toggleLeds(Colors color1, Colors color2, int delayMS) {
    if (color1 != lastColor1 || color2 != lastColor2 || millis() - lastdelayToggle > delayMS + 50) { // For color syncing purposes between different devices
        lastdelayToggle = millis(); 
        ledToggleState = 0;
    }
    currentDelayToggle = millis();
    
    if (currentDelayToggle - lastdelayToggle > delayMS) {
        ledToggleState = !ledToggleState;
        lastdelayToggle = currentDelayToggle;
        if (ledToggleState) 
            setLeds(color1);
        else
            setLeds(color2);
    }
    lastColor1 = color1;
    lastColor2 = color2;
}