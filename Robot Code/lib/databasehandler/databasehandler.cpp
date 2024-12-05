#include <databasehandler.h>

database_handler::database_handler() {}

void database_handler::updateFromDatabase() {
  File databaseFile = SPIFFS.open("/motor_settings.txt", "r");

  melty_param_string = databaseFile.readStringUntil('\n');
  tankdrive_param_string = databaseFile.readStringUntil('\n');

  databaseFile.close();
}

void database_handler::storeMeltyParameters(int rot, int tra, float per, int boost) {
  JsonDocument newSettings;

  newSettings["rot"] = rot;
  newSettings["tra"] = tra;
  newSettings["per"] = per;
  newSettings["boost"] = boost;

  File newFile = SPIFFS.open("/motor_settings.txt", "w");

  String newSettingsString;
  serializeJson(newSettings, newSettingsString);
  newFile.println(newSettingsString);
  newFile.println(tankdrive_param_string);

  newFile.close();
  newSettings = 1;
  updateFromDatabase();
}

void database_handler::storeTankParameters(int drive, int turn, int boost) {
  JsonDocument newSettings;

  newSettings["drive"] = drive;
  newSettings["turn"] = turn;
  newSettings["boost"] = boost;

  File newFile = SPIFFS.open("/motor_settings.txt", "w");
  String newSettingsString;
  serializeJson(newSettings, newSettingsString);
  newFile.println(melty_param_string);
  newFile.println(newSettingsString);

  newFile.close();
  newSettings = 1;
  updateFromDatabase();
}