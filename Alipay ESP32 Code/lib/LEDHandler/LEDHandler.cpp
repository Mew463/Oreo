#include "LEDHandler.h"

CRGB leds[1];
bool ledToggleState = 0;
unsigned long lastdelayToggle = millis();
unsigned long currentDelayToggle = millis();


void init_led() {
    FastLED.addLeds<WS2812B, 4, GRB>(leds, 1).setCorrection(TypicalLEDStrip);
}

void setLeds(CRGB color)
{
    fill_solid(leds, 1, color);
    FastLED.show();
}

void toggleLeds(CRGB color1, CRGB color2, int delayMS) {
    currentDelayToggle = millis();
    if (currentDelayToggle - lastdelayToggle > delayMS) {
        ledToggleState = !ledToggleState;
        lastdelayToggle = currentDelayToggle;
    }

    if (ledToggleState)
        setLeds(color1);
    else
        setLeds(color2);
}