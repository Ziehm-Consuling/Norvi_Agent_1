#ifndef WIFI_FEATURES_H
#define WIFI_FEATURES_H


#include <WebServer.h>
#include "eeprom_variables.h"
#include "eeprom_addresses.h"
#include <EEPROM.h>  // Include EEPROM library
#include <RTClib.h>

extern String currentSchedule;
extern bool alarm_updated;
extern bool power_output;


void handleRoot();
void handleConfigNetwork();
void handleSend();
void handleScheduleConfig();
void handleSave();
void setupWebServer();
void checkAndSetAlarms();
void handleAlarm1();
void handleAlarm2();
void excecute_alarm();
void executeScheduledTasks();
void function1();
void function2();

#endif
