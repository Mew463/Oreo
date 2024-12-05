#include <vector>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

using namespace std;

class database_handler {
  public: 
  String melty_param_string;
  String tankdrive_param_string;

  bool newSettings = 1;

  database_handler();

  void updateFromDatabase();

  void storeMeltyParameters(int rot, int tra, float per, int boost);

  void storeTankParameters(int drive, int turn, int boost);

  private:

};