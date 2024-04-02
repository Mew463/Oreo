#include <Arduino.h>
#include <LEDHandler.h>
#include <mpu6050.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 
#include <BLE_Uart.h>
#include <PID.h>
#include <FastLED.h>

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};
const int headings[] = {0, 45, 90, 135, 180, 225, 270, 315};
bool wasMeltying = false;
int slowDownSpeed = 8;

BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);
Drive_Motors driveMotors = Drive_Motors();

melty alipay = melty();
struct melty_parameters {
  int rot = 6;
  int tra = 6;
  float per = 0.5;
  int invert = 1;
  int boost = 5;
} melty_parameters;

struct tank_drive_parameters {
  int drive = 2;
  int turn = 2;
  int boost = 2;
} tank_drive_parameters;

struct pid_tank_drive_parameters {
  int drive = 2;
  int headingTurn = 2.3;
  int boost = 2;
} pid_tank_drive_parameters;

void setup()
{
  init_led();
  for (int i = 0; i < 10; i++) {
    setLeds(ORANGE);
    delay(50);
  }
  USBSerial.begin(115200);
  // init_mpu6050();
  USBSerial.println("init mpu6050");
  driveMotors.init_motors();
  USBSerial.println("init motors");
  laptop.init_ble("Alipay");
  USBSerial.println("init laptop");
  setLeds(BLACK); 
}
 
void loop()
{
  if (laptop.isConnected()) {
    if (driveMotors.isNeutral()) {
      // if (getAccelZ() > 7) {
        alipay.useTopIr = 1;
        driveMotors.flip_motors = 0;
        melty_parameters.invert = 1;
      // }
      // if (getAccelZ() < -7) {
      //   alipay.useTopIr = 0;
      //   driveMotors.flip_motors = 1;
      //   melty_parameters.invert = -1;
      // }
    }

    if (laptop_packetBuffer[0] == '1') { // Currently enabled and meltybraining!!!
      if (alipay.update()) { // If seen the LED
        EVERY_N_MILLIS(250) {
          laptop.send("seen");
        }
      }
      
      
      int boostVal = 0;
      if (laptop_packetBuffer[3] == '1')
        boostVal = melty_parameters.boost;
      if (alipay.translate()) {
        driveMotors.l_motor_write(melty_parameters.invert * (melty_parameters.rot - (melty_parameters.tra + boostVal)));
        driveMotors.r_motor_write(melty_parameters.invert * (melty_parameters.rot + (melty_parameters.tra + boostVal)));
      } else {
        driveMotors.set_both_motors(melty_parameters.invert * (melty_parameters.rot + boostVal));
      }

      int drivecmd = laptop_packetBuffer[1] - '0';
      if (drivecmd > 0 && drivecmd < 9) { // 1,2,3,4,5,6,7,8
        if (melty_parameters.invert == 1)
          drivecmd = 8 - drivecmd;
        else
          drivecmd = drivecmd - 1;
        alipay.deg = headings[drivecmd];
      }

      if (laptop_packetBuffer[1] != '0') { // Check drive cmd for setting the "neutral" state
        alipay.percentageOfRotation = melty_parameters.per;
      } else {
        alipay.percentageOfRotation = 0;
      }

      EVERY_N_SECONDS(1) { // DEBUGGIN!!!!
        String msg = "PR IR_LED: " + String(alipay.photo_resistor_vals.getMaxVal());
        laptop.send(msg);
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            melty_parameters.rot++;
            break;
          case '2':
            melty_parameters.rot--;
            break;
          case '3':
            melty_parameters.tra++;
            break;
          case '4':
            melty_parameters.tra--;
            break;
          case '5':
            melty_parameters.per = melty_parameters.per + .03;
            break;
          case '6':
            melty_parameters.per = melty_parameters.per - .03;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "rotpwr : " + String(melty_parameters.rot) + " tranpwr : " + String(melty_parameters.tra) + " perc : " + String(melty_parameters.per);
          laptop.send(msg);
        }
      }

      wasMeltying = 1;

    } else if (laptop_packetBuffer[0] == '2') { // Tank driving mode!
      // if (wasMeltying) { // Was previously meltybraining, we need to slowdown
      //   unsigned long timeout = millis();
      //   while (getAccelY() > .3 && millis() - timeout < 2000) {
      //     driveMotors.set_both_motors(-slowDownSpeed * melty_parameters.invert);
      //     toggleLeds(CRGB::White, CRGB::Red, 150);
      //   }
      //   wasMeltying = 0;
      // }
      
      int lmotorpwr = 0;
      int rmotorpwr = 0;

      int boostVal = 0;
      if (laptop_packetBuffer[3] == '1')
        boostVal = tank_drive_parameters.boost;


      switch (laptop_packetBuffer[1]) { // Check the drive cmd
      case '0':
        lmotorpwr = 0;
        rmotorpwr = 0;
        break;
      case '1':
        lmotorpwr = tank_drive_parameters.drive + boostVal;
        rmotorpwr = tank_drive_parameters.drive + boostVal;
        break;
      case '2':
        lmotorpwr = tank_drive_parameters.drive + tank_drive_parameters.turn + boostVal;
        rmotorpwr = tank_drive_parameters.drive + boostVal; 
        break;
      case '3':
        lmotorpwr = tank_drive_parameters.turn + boostVal;
        // rmotorpwr = -tank_drive_parameters.turn; 
        break;
      case '4':
        lmotorpwr = -tank_drive_parameters.drive - tank_drive_parameters.turn - boostVal;
        rmotorpwr = -tank_drive_parameters.drive - boostVal;
        break;
      case '5':
        lmotorpwr = -tank_drive_parameters.drive - boostVal;
        rmotorpwr = -tank_drive_parameters.drive - boostVal;
        break;
      case '6':
        lmotorpwr = -tank_drive_parameters.drive - boostVal;
        rmotorpwr = -tank_drive_parameters.drive - tank_drive_parameters.turn - boostVal; 
        break;
      case '7':
        // lmotorpwr = -tank_drive_parameters.turn;
        rmotorpwr = tank_drive_parameters.turn + boostVal;  
        break;
      case '8':
        lmotorpwr = tank_drive_parameters.drive + boostVal;
        rmotorpwr = tank_drive_parameters.drive + tank_drive_parameters.turn + boostVal; 
        break;
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            tank_drive_parameters.drive++;
            break;
          case '2':
            tank_drive_parameters.drive--;
            break;
          case '3':
            tank_drive_parameters.turn++;
            break;
          case '4':
            tank_drive_parameters.turn--;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "drivpwr : " + String(tank_drive_parameters.drive) + " turnpwr : " + String(tank_drive_parameters.turn);
          laptop.send(msg);
        }
      }

      driveMotors.l_motor_write(-lmotorpwr);
      driveMotors.r_motor_write(rmotorpwr);
      toggleLeds(WHITE, BLACK, 500);
    
    } else if (laptop_packetBuffer[0] == '3'){ // Tank driving mode + PID!
      int lmotorpwr;
      int rmotorpwr;
      static float desiredHeading;
      int pidOutput = 0;

      int boostVal = 0;
      if (laptop_packetBuffer[3] == '1')
        boostVal = pid_tank_drive_parameters.boost;

      switch (laptop_packetBuffer[1]) { // Check the drive cmd
      case '0':
        lmotorpwr = 0;
        rmotorpwr = 0;
        // desiredHeading = getGyroZ();
        break;
      case '8':
        lmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        rmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        desiredHeading+=pid_tank_drive_parameters.headingTurn;
        break;
      case '1':
        lmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        rmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        break;
      case '2':
        lmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        rmotorpwr = pid_tank_drive_parameters.drive +boostVal;
        desiredHeading-=pid_tank_drive_parameters.headingTurn;
        break;

      case '4':
        lmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        rmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        desiredHeading+=pid_tank_drive_parameters.headingTurn;
        break;
      case '5':
        lmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        rmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        break;
      case '6':
        lmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        rmotorpwr = -pid_tank_drive_parameters.drive -boostVal;
        desiredHeading-=pid_tank_drive_parameters.headingTurn;
        break;
      }

      // if (laptop_packetBuffer[1] != '0') // We want to drive, therefore start calculating pidout
      //   pidOutput = calcTurnPower(desiredHeading - getGyroZ());

      EVERY_N_MILLIS(10) {
        driveMotors.l_motor_write(- lmotorpwr + pidOutput);
        driveMotors.r_motor_write(rmotorpwr + pidOutput);
      }
      
      toggleLeds(GREEN, BLUE, 500);

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            PID.kP+=.001;
            break;
          case '2':
            PID.kP-=.001;
            break;
          case '3':
            PID.kI+=.01;
            break;
          case '4':
            PID.kI-=.01;
            break;
          case '5':
            PID.kD+=.001;
            break;
          case '6':
            PID.kD-=.001;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "kP : " + String(PID.kP, 3) + " kI : " + String(PID.kI ,3) + " kD : " + String(PID.kD, 3);
          laptop.send(msg);
        }
      }


    } else { // Currently disabled
      toggleLeds(RED, GREEN, 500);
      driveMotors.set_both_motors(0); 
        EVERY_N_SECONDS(10) {
          laptop.send(get3sVoltage());
        }
    }
  } else { // Currently DISCONNECTED
    driveMotors.set_both_motors(0);
    toggleLeds(RED, BLACK, 500);
    // setLeds(RED);
    // delay(500);
    // setLeds(BLACK);
    // delay(500);
  }

  

  
}
