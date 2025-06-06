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

#define IS_HOCKEY_PUCK 0

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};
const int headings[] = {0, 45, 90, 135, 180, 225, 270, 315};
bool wasMeltying = false;
int slowDownSpeed = 200;
Robot_BLE_Uart transmitter = Robot_BLE_Uart(laptop_packetBuffer, packSize);
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
  init_led();
  setLeds(ORANGE);
  
  SPIFFS.begin(true);
  
  motor_settings.updateFromDatabase();
  
  driveMotors.init_motors(); // <- This needs to be init first or else something with RMT doesnt work....
  
  pinMode(RED_LED_BOT, OUTPUT);
  pinMode(RED_LED_TOP, OUTPUT);
  digitalWrite(RED_LED_BOT, HIGH);
  digitalWrite(RED_LED_TOP, HIGH);
  
  driveMotors.arm_motors();
  transmitter.init_ble("Oreo");
  setLeds(BLACK); 
  
  delay(250); // Power up delay for the Serial
  USBSerial.begin(115200);
}
 
void loop()
{
  if (!transmitter.isConnected() || laptop_packetBuffer[0] != '1') { // Turn off high powered leds if not meltying
    digitalWrite(RED_LED_TOP, HIGH); 
    digitalWrite(RED_LED_BOT, HIGH);
  }

  if (transmitter.isConnected()) {
    EVERY_N_MILLIS(50) {
      // myPTs.printDebugInfo();
      if (!myPTs.checkIsFlipped()) { // Not flipped
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

 
    if (transmitter.getMode() == ROBOT_MODES::MELTY) { // Currently enabled and meltybraining!!!
      setLedMode(BOTH);
      setLeds(BLACK);
      if (oreo.update()) { // If seen the LED
        if (oreo.useTopIr)
          digitalWrite(RED_LED_TOP, LOW);
        else  
          digitalWrite(RED_LED_BOT, LOW);
        EVERY_N_MILLIS(250) {
          transmitter.send("seen");
        }

      } else {
        digitalWrite(RED_LED_TOP, HIGH); 
        digitalWrite(RED_LED_BOT, HIGH);
      }

      int boostVal = 0;
      if (laptop_packetBuffer[3] == '1')
        boostVal = melty_parameters.boost;

      int adjRotValue = melty_parameters.rot + boostVal;
      int adjTransValue = melty_parameters.tra + boostVal;
      float transMultiplier = oreo.translate() / 100.0;

      if (transMultiplier != 0) {
        driveMotors.l_motor_write(adjRotValue - adjTransValue * transMultiplier);
        driveMotors.r_motor_write(adjRotValue + adjTransValue * transMultiplier);
      } else {
        driveMotors.set_both_motors((adjRotValue));
      } 

      int drivecmd = laptop_packetBuffer[1] - '0';
      if (drivecmd > 0 && drivecmd < 9) { // 1,2,3,4,5,6,7,8
        drivecmd -= 1; // Make it 0 indexed, so now it goes 0 -> 7
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
        transmitter.send(msg);
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
          transmitter.send(msg);

          motor_settings.storeMeltyParameters(melty_parameters.rot, melty_parameters.tra, melty_parameters.per, melty_parameters.boost);
        }
      }

      wasMeltying = 1;

    } else if (transmitter.getMode() == ROBOT_MODES::TANK) { // Tank driving mode!
      
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
          transmitter.send(msg);

          motor_settings.storeTankParameters(tank_drive_parameters.drive, tank_drive_parameters.turn, tank_drive_parameters.boost);
        }
      }

      driveMotors.l_motor_write(-lmotorpwr);
      driveMotors.r_motor_write(rmotorpwr);
      toggleLeds(WHITE, BLACK, 500);
    
    } else if (transmitter.getMode() == ROBOT_MODES::IDLE ){ // Currently DISABLED
      toggleLeds(RED, GREEN, 500);
      driveMotors.set_both_motors(0); 

      EVERY_N_SECONDS(1) {
        transmitter.send("SOC: " + String(get3sSOC(BAT_PIN)) + " %");
      }
    }
  } else { // Currently DISCONNECTED
    setLedMode(BOTH);
    driveMotors.set_both_motors(0);
    toggleLeds(RED, BLACK, 500);
  }

}

// this is katie hello :)) 