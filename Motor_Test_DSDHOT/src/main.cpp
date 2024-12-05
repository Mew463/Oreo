#define LEDS_COUNT  1
#define LEDS_PIN	48
#define CHANNEL		0
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "DShotESC.h"
#include "/Users/mingweiyeoh/Documents/GitHub/Arduino-Projects/libraries/Custom/USBSerialHandler.h"
SerialHandler myComputer = SerialHandler(); 

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, 0, TYPE_GRB); // Channel is always 0 for some reason.....

DShotESC rmot;

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
};

void setLed(int colorWheel) {
	strip.setLedColorData(0, strip.hsv2rgb(colorWheel*30, 100, 100));
	strip.show();
}


void setup()
{
	USBSerial.begin(115200);
	

	rmot.install(GPIO_NUM_7, RMT_CHANNEL_1); //<-- This is the problem line. Apparently this line has to go before initializing the strip
	delay(250);
	rmot.init();
	delay(250);
	rmot.sendMotorStop();
	delay(250);
	// rmot.setReversed(false);
	// rmot.set3DMode(true);
	rmot.throttleArm(); // <--- Super important!!!;
	delay(250);
	// rmot.blueJayArm();
	// delay(1000);
	// for (int i = 0; i < 10; i++) {
	// 	rmot.sendThrottle3D(0);
	// 	delay(10);
	// }

	strip.begin();
	strip.setBrightness(10);	
	USBSerial.println("Done initializing!!!");
}

void loop() {
	int throttle = myComputer.getInt(0);
	rmot.sendThrottle3D(throttle); // Throttle value from -999 to 999
	// lmot.sendThrottle3D(throttle);
	delay(1); // Maybe need to send update faster than 10ms??

	if (throttle != 0) {
		setLed(RED);
	} else {
		setLed(GREEN);
	}

}

