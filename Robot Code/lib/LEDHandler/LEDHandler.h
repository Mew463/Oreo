#include <Freenove_WS2812_Lib_for_ESP32.h>
#include <FastLED.h>

#ifdef IR_BEACON
    #define LEDPIN 5
#else
    #define LEDPIN 46
#endif

enum Colors {
	RED,
	ORANGE,
	YELLOW,
	LIME,
	GREEN,
	AQUA,
	CYAN,
	SKY,
	BLUE,
	PURPLE,
	RED_PURPLE,
	BLACK,
	WHITE
};

enum ledmodes{
	BOTH, BOTTOM, TOP 
};

void setLedMode(ledmodes newledmode);

void init_led();

void setLeds(Colors color);

void toggleLeds(Colors color1, Colors color2, int delayMS);