#include "LEDHandler.h"

bool ledToggleState = 0;
unsigned long lastdelayToggle = millis();
unsigned long currentDelayToggle = millis();

CRGB leds[1];

void init_led() {
    FastLED.addLeds<WS2812B, LEDPIN, GRB>(leds, 1).setCorrection(TypicalLEDStrip);
}

void setLeds(CRGB color)
{
    leds[0] = color;
    FastLED.show();
}

void syncToggle() {
    lastdelayToggle = millis();
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