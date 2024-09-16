#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>

#define ADXL375_SCK 13
#define ADXL375_MISO 12
#define ADXL375_MOSI 11
#define ADXL375_CS 10

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);

/* Uncomment for software SPI */
//Adafruit_ADXL375 accel = Adafruit_ADXL375(ADXL375_SCK, ADXL375_MISO, ADXL375_MOSI, ADXL375_CS, 12345);

/* Uncomment for hardware SPI */
//Adafruit_ADXL375 accel = Adafruit_ADXL375(ADXL375_CS, &SPI, 12345);

void displayDataRate(void)
{
  USBSerial.print  ("Data Rate:    ");

  switch(accel.getDataRate())
  {
    case ADXL343_DATARATE_3200_HZ:
      USBSerial.print  ("3200 ");
      break;
    case ADXL343_DATARATE_1600_HZ:
      USBSerial.print  ("1600 ");
      break;
    case ADXL343_DATARATE_800_HZ:
      USBSerial.print  ("800 ");
      break;
    case ADXL343_DATARATE_400_HZ:
      USBSerial.print  ("400 ");
      break;
    case ADXL343_DATARATE_200_HZ:
      USBSerial.print  ("200 ");
      break;
    case ADXL343_DATARATE_100_HZ:
      USBSerial.print  ("100 ");
      break;
    case ADXL343_DATARATE_50_HZ:
      USBSerial.print  ("50 ");
      break;
    case ADXL343_DATARATE_25_HZ:
      USBSerial.print  ("25 ");
      break;
    case ADXL343_DATARATE_12_5_HZ:
      USBSerial.print  ("12.5 ");
      break;
    case ADXL343_DATARATE_6_25HZ:
      USBSerial.print  ("6.25 ");
      break;
    case ADXL343_DATARATE_3_13_HZ:
      USBSerial.print  ("3.13 ");
      break;
    case ADXL343_DATARATE_1_56_HZ:
      USBSerial.print  ("1.56 ");
      break;
    case ADXL343_DATARATE_0_78_HZ:
      USBSerial.print  ("0.78 ");
      break;
    case ADXL343_DATARATE_0_39_HZ:
      USBSerial.print  ("0.39 ");
      break;
    case ADXL343_DATARATE_0_20_HZ:
      USBSerial.print  ("0.20 ");
      break;
    case ADXL343_DATARATE_0_10_HZ:
      USBSerial.print  ("0.10 ");
      break;
    default:
      USBSerial.print  ("???? ");
      break;
  }
  USBSerial.println(" Hz");
}

void setup(void)
{
  USBSerial.begin(115200);
  while (!USBSerial);
  delay(1000);
  USBSerial.println("ADXL375 Accelerometer Test"); USBSerial.println("");
  Wire.begin(5,6);
  /* Initialise the sensor */
  while(!accel.begin())
  {
    /* There was a problem detecting the ADXL375 ... check your connections */
    USBSerial.println("Ooops, no ADXL375 detected ... Check your wiring!");
    delay(100);
  }

  // Range is fixed at +-200g

  /* Display some basic information on this sensor */
  accel.setTrimOffsets(0, 0, 0);
  
  USBSerial.println("Hold accelerometer flat to set offsets to 0, 0, and -1g...");
  delay(1000);
  int16_t x, y, z;
  x = accel.getX();
  y = accel.getY();
  z = accel.getZ();
  USBSerial.print("Raw X: "); USBSerial.print(x); USBSerial.print("  ");
  USBSerial.print("Y: "); USBSerial.print(y); USBSerial.print("  ");
  USBSerial.print("Z: "); USBSerial.print(z); USBSerial.print("  ");USBSerial.println(" counts");

  // the trim offsets are in 'multiples' of 4, we want to round, so we add 2
  accel.setTrimOffsets(-(x+2)/4, 
                       -(y+2)/4, 
                       -(z-20+2)/4);  // Z should be '20' at 1g (49mg per bit)
  
  int8_t x_offset, y_offset, z_offset;
  accel.getTrimOffsets(&x_offset, &y_offset, &z_offset);
  USBSerial.print("Current trim offsets: ");
  USBSerial.print(x_offset);  USBSerial.print(", ");
  USBSerial.print(y_offset);  USBSerial.print(", ");
  USBSerial.println(z_offset);

  USBSerial.println();
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  USBSerial.print("X: "); USBSerial.print(event.acceleration.x); USBSerial.print("  ");
  USBSerial.print("Y: "); USBSerial.print(event.acceleration.y); USBSerial.print("  ");
  USBSerial.print("Z: "); USBSerial.print(event.acceleration.z); USBSerial.print("  ");USBSerial.println("m/s^2 ");
  delay(250);
}