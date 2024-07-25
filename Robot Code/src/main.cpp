#include <LEDHandler.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 
#include <BLE_Uart.h>
#include <FastLED.h>
#include <Photo_Transistors.h>
#include <pin_definitions.h>
#include "SPIFFS.h"
#include <databasehandler.h>

#define IS_HOCKEY_PUCK 1

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};
const int headings[] = {0, 45, 90, 135, 180, 225, 270, 315};
bool wasMeltying = false;
int slowDownSpeed = 200;
BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);
Drive_Motors driveMotors = Drive_Motors(LEFT_MOTOR_PIN, LEFT_MOTOR_CHANNEL, RIGHT_MOTOR_PIN, RIGHT_MOTOR_CHANNEL);
robotOrientation myPTs = robotOrientation(TOP_PHOTO_TRANSISTOR, BOTTOM_PHOTO_TRANSISTOR);
database_handler motor_settings = database_handler();

melty oreo = melty(TOP_IR_PIN, BOTTOM_IR_PIN);
struct melty_parameters {
  int rot;
  int tra;
  float per;
  int boost;
} melty_parameters;

struct tank_drive_parameters {
  int drive;
  int turn;
  int boost;
} tank_drive_parameters;

int tuningValue = 10;

void setup()
{
  USBSerial.begin(115200);
  SPIFFS.begin(true);

  motor_settings.updateFromDatabase();

  driveMotors.init_motors(); // <- This needs to be init first or else something with RMT doesnt work....
  init_led();
  setLeds(ORANGE);
  driveMotors.arm_motors();
  laptop.init_ble("Oreo");
  setLeds(BLACK); 

}
 
void loop()
{
  if (laptop.isConnected()) {
    EVERY_N_MILLIS(50) {
      myPTs.checkIsFlipped();
      if (!myPTs.isFlippedResult) { // Not flipped
        oreo.useTopIr = 1;
        driveMotors.flip_motors = 0;
        setLedMode(TOP);
      } else {
        oreo.useTopIr = 0;
        driveMotors.flip_motors = 1;
        setLedMode(BOTTOM);
      }

      if (motor_settings.newSettings == true) {
        JsonDocument melty_params_json;
        deserializeJson(melty_params_json, motor_settings.melty_param_string);

        melty_parameters.rot = melty_params_json["rot"].as<signed int>();
        melty_parameters.tra = melty_params_json["tra"].as<signed int>();
        melty_parameters.per = melty_params_json["per"].as<float>();
        melty_parameters.boost = melty_params_json["boost"].as<signed int>();

        JsonDocument tank_params_json;
        deserializeJson(tank_params_json, motor_settings.tankdrive_param_string);

        tank_drive_parameters.drive = tank_params_json["drive"].as<signed int>();
        tank_drive_parameters.turn = tank_params_json["turn"].as<signed int>();
        tank_drive_parameters.boost = tank_params_json["boost"].as<signed int>();

        motor_settings.newSettings = false;

      }
    }


    if (laptop_packetBuffer[0] == '1') { // Currently enabled and meltybraining!!!
      if (oreo.update()) { // If seen the LED
        EVERY_N_MILLIS(250) {
          laptop.send("seen");
        }
      }

      int boostVal = 0;
      if (laptop_packetBuffer[3] == '1')
        boostVal = melty_parameters.boost;

      int adjRotValue = melty_parameters.rot + boostVal;
      int adjTransValue = melty_parameters.tra + boostVal;

      if (myPTs.isFlippedResult && IS_HOCKEY_PUCK) { // Since we need to spin the opposite way for tooth engagement if we are hockey puck
        adjRotValue   *= -1;
        adjTransValue *= -1;
      }
      
      if (oreo.translate()) {
        driveMotors.l_motor_write(adjRotValue - adjTransValue);
        driveMotors.r_motor_write(adjRotValue + adjTransValue);
      } else if (oreo.translateInverse()) {
        driveMotors.l_motor_write(adjRotValue + adjTransValue);
         driveMotors.r_motor_write(adjRotValue - adjTransValue);
      } else {
        driveMotors.set_both_motors((adjRotValue));
      } 

      int drivecmd = laptop_packetBuffer[1] - '0';
      if (drivecmd > 0 && drivecmd < 9) { // 1,2,3,4,5,6,7,8
        drivecmd -= 1; // Make it 0 indexed, so now it goes 0 -> 7
        if (myPTs.isFlippedResult == true && IS_HOCKEY_PUCK) // Perform manipulation on the orientation offsets 1
          drivecmd = drivecmd + 3;
        else
          drivecmd = 7 - drivecmd;

        if (drivecmd < 0 || drivecmd > 7) {
          drivecmd = (drivecmd % 8 + 8) % 8; // Make sure we always in the bounds of the array
        }
        
        oreo.deg = headings[drivecmd];
      }

      if (laptop_packetBuffer[1] != '0') { // Check drive cmd for setting the "neutral" state
        oreo.percentageOfRotation = melty_parameters.per;
      } else {
        oreo.percentageOfRotation = 0;
      }

      EVERY_N_SECONDS(1) { // DEBUGGIN!!!!
        String msg = "RPM : " + String(oreo.ledRPM);
        laptop.send(msg);
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            melty_parameters.rot+=tuningValue;
            break;
          case '2':
            melty_parameters.rot-=tuningValue;
            break;
          case '3':
            melty_parameters.tra+=tuningValue;
            break;
          case '4':
            melty_parameters.tra-=tuningValue;
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

          motor_settings.storeMeltyParameters(melty_parameters.rot, melty_parameters.tra, melty_parameters.per, melty_parameters.boost);
        }
      }

      wasMeltying = 1;

    } else if (laptop_packetBuffer[0] == '2') { // Tank driving mode!
      // if (wasMeltying) { // Was previously meltybraining, we need to slowdown
      //   unsigned long timeout = millis();
      //   while (getAccelY() > 2 && millis() - timeout < 2000) {
      //     updateAccel();
      //     driveMotors.set_both_motors(-slowDownSpeed * melty_parameters.invert);
      //     toggleLeds(WHITE, RED, 150);
      //     if (laptop_packetBuffer[0] != '2')
      //       break;
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
            tank_drive_parameters.drive+=tuningValue;
            break;
          case '2':
            tank_drive_parameters.drive-=tuningValue;
            break;
          case '3':
            tank_drive_parameters.turn+=tuningValue;
            break;
          case '4':
            tank_drive_parameters.turn-=tuningValue;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "drivpwr : " + String(tank_drive_parameters.drive) + " turnpwr : " + String(tank_drive_parameters.turn);
          laptop.send(msg);

          motor_settings.storeTankParameters(tank_drive_parameters.drive, tank_drive_parameters.turn, tank_drive_parameters.boost);
        }
      }

      driveMotors.l_motor_write(-lmotorpwr);
      driveMotors.r_motor_write(rmotorpwr);
      toggleLeds(WHITE, BLACK, 500);
    
    } else { // Currently DISABLED
      toggleLeds(RED, GREEN, 500);
      driveMotors.set_both_motors(0); 

      EVERY_N_SECONDS(1) {
        laptop.send("SOC: " + String(get3sSOC()) + " %");
      }
    }
  } else { // Currently DISCONNECTED
    setLedMode(BOTH);
    driveMotors.set_both_motors(0);
    toggleLeds(RED, BLACK, 500);
  }

}
