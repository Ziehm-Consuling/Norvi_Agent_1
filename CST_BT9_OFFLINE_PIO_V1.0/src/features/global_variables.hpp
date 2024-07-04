#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <Arduino.h>

struct Time {
    uint8_t hour;
    uint8_t minute;
};

struct Date {
    uint8_t day;
    uint8_t month;
    int year;
};

struct Schedule {
    Time on_time;
    Time off_time;
    Date day; // For special dates only
};

extern String MONDAY_ON_TIME;
extern String MONDAY_OFF_TIME;
extern String TUESDAY_ON_TIME;
extern String TUESDAY_OFF_TIME;
extern String WEDNESDAY_ON_TIME;
extern String WEDNESDAY_OFF_TIME;
extern String THURSDAY_ON_TIME;
extern String THURSDAY_OFF_TIME;
extern String FRIDAY_ON_TIME;
extern String FRIDAY_OFF_TIME;
extern String SATURDAY_ON_TIME;
extern String SATURDAY_OFF_TIME;
extern String SUNDAY_ON_TIME;
extern String SUNDAY_OFF_TIME;
extern String D1_ON_TIME;
extern String D1_OFF_TIME;
extern String D2_ON_TIME;
extern String D2_OFF_TIME;
extern String D3_ON_TIME;
extern String D3_OFF_TIME;
extern String D4_ON_TIME;
extern String D4_OFF_TIME;
extern String D5_ON_TIME;
extern String D5_OFF_TIME;
extern String DAY1;
extern String DAY2;
extern String DAY3;
extern String DAY4;
extern String DAY5;
extern String UI_NETWORK;
extern String UI_SIGNAL;
extern String UI_STATUS_CELL;
extern String UI_STATUS_MQTT;
extern String NETWORK_APN;
extern String MQTT_SERVER;
extern String MQTT_USER;
extern String MQTT_PW;
extern String MQTT_DN;
extern String MQTT_ID;
extern String MQTT_SD;
extern Schedule monday;
extern Schedule tuesday;
extern Schedule wednesday;
extern Schedule thursday;
extern Schedule friday;
extern Schedule saturday;
extern Schedule sunday;
extern Schedule specialDates[5];
extern String http_username;
extern String http_password;




//extern const int serverPort = 80;
//WebServer server(serverPort);


#endif // EEPROM_VARIABLES_H
