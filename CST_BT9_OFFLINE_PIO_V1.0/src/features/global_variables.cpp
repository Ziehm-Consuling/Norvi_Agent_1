
#include "global_variables.hpp"
#include <Arduino.h>


String MONDAY_ON_TIME = "08:05";
String MONDAY_OFF_TIME = "08:50";
String TUESDAY_ON_TIME = "01:30";
String TUESDAY_OFF_TIME = "11:12";
String WEDNESDAY_ON_TIME = "04:06";
String WEDNESDAY_OFF_TIME = "07:32";
String THURSDAY_ON_TIME = "09:20";
String THURSDAY_OFF_TIME = "08:18";
String FRIDAY_ON_TIME = "06:30";
String FRIDAY_OFF_TIME = "13:10";
String SATURDAY_ON_TIME = "13:10";
String SATURDAY_OFF_TIME = "13:10";
String SUNDAY_ON_TIME = "04:05";
String SUNDAY_OFF_TIME = "06:30";
String D1_ON_TIME = "04:05";
String D1_OFF_TIME = "04:05";
String D2_ON_TIME = "04:05";
String D2_OFF_TIME = "04:05";
String D3_ON_TIME = "06:30";
String D3_OFF_TIME = "04:05";
String D4_ON_TIME = "01:58";
String D4_OFF_TIME = "13:10";
String D5_ON_TIME = "20:22";
String D5_OFF_TIME = "21:30";
String DAY1 = "12-07-2024";
String DAY2 = "16-07-1997";
String DAY3 = "12-07-2024";
String DAY4 = "10-08-2024";
String DAY5 = "1-17-2024";
String UI_NETWORK = "CTEL";
String UI_SIGNAL = "100";
String UI_STATUS_CELL = "CONNECTED";
String UI_STATUS_MQTT = "DISCONNECTED";
String NETWORK_APN = "dalta.bb";
String MQTT_SERVER = "delta.bb";
String MQTT_USER = "user1";
String MQTT_PW = "delta.bb";
String MQTT_DN = "adl_connect";
String MQTT_ID = "ade52146kve511";
String MQTT_SD = "551965641856";
Schedule monday;
Schedule tuesday;
Schedule wednesday;
Schedule thursday;
Schedule friday;
Schedule saturday;
Schedule sunday;
Schedule specialDates[5];