#define LEDS_COUNT  1
#define LEDS_PIN	4
#define CHANNEL		0
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "DShotESC.h"
#include "/Users/mingweiyeoh/Documents/GitHub/Arduino-Projects/libraries/Custom/USBSerialHandler.h"
SerialHandler myComputer = SerialHandler(); 

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, 0, TYPE_GRB); // Channel is always 0 for some reason.....

DShotESC rmot;
DShotESC lmot;

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
} Colors;

void setLed(int colorWheel) {
	strip.setLedColorData(0, strip.hsv2rgb(colorWheel*30, 100, 100));
	strip.show();
}


void setup()
{
	USBSerial.begin(115200);

	rmot.install(GPIO_NUM_7, RMT_CHANNEL_1); //<-- This is the problem line. Apparently this line has to go before initializing the strip
	rmot.init();
	rmot.setReversed(false);
	rmot.set3DMode(true);
	rmot.throttleArm(); // <--- Super important!!!;

	lmot.install(GPIO_NUM_8, RMT_CHANNEL_2); //<-- This is the problem line. Apparently this line has to go before initializing the strip
	lmot.init();
	lmot.setReversed(false);
	lmot.set3DMode(true);
	lmot.throttleArm(); // <--- Super important!!!;

	strip.begin();
	strip.setBrightness(10);	
	
	for (int i = 0; i < 3 ; i++) {
		setLed(RED);
		delay(1);
	}

	int mydelay = 10;
	for (int i = 0; i < 3000/mydelay; i++)
	{
		rmot.sendThrottle3D(0);
		lmot.sendThrottle(0);
		delay(mydelay);
	}
	USBSerial.println("Armed!!!");
}

void loop() {
	int throttle = myComputer.getInt(0);
	rmot.sendThrottle3D(throttle); // Throttle value from -999 to 999
	lmot.sendThrottle3D(throttle);
	delay(10);

	if (throttle != 0) {
		setLed(RED);
	} else {
		setLed(GREEN);
	}

}

