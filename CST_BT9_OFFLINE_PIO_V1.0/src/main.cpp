// put function declarations here:

#include <Arduino.h>


#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Wire.h>
#include <RTClib.h>
#include "eeprom_functions.h"
#include "web_functions.h"
#include "eeprom_addresses.h"
#include "eeprom_variables.h" // Include the variables



const char* ssid = "ESP32-Hotspot";
const char* password = "123456789";



  #define SDA   16    // 
  #define SCL   17    // 


// Define starting addresses for each text zone
#define EEPROM_SIZE 256
// Define EEPROM addresses for each variable
/*#define ADDR_UI_DATE 0
#define ADDR_UI_STATUS_CELL 14
#define ADDR_UI_TIME 10*/


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
String http_username = "admin";
String http_password = "admin";

String currentSchedule = "";
bool alarm_updated = false;
bool power_output = false;



/********************************************************************************************************************PARSE DIGIT  FUNCTION*************************************************/
// parseDigit
uint8_t parseDigits(char* str, uint8_t count)
{
    uint8_t val = 0;
    while(count-- > 0) val = (val * 10) + (*str++ - '0');
    return val;
}
/********************************************************************************************************************HANDLE ROOT PAGE FUNCTION****************************************/


/********************************************************************************************************************HANDLE SCHEDULE CONFIG UPDATE FUNCTION****************************************/
// Handler for the schedule configuration page

/********************************************************************************************************************SETUP FUNCTION****************************************/
void setup() {
  Serial.begin(115200);

  // Set up Wi-Fi hotspot
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);
  
  EEPROM_RESET_CHECK();
  Serial.println("RESET CHECK DONE ");
  // Initialize EEPROM
  EEPROM.begin(512);  delay(100); Serial.println("EEPROM BEGIN DONE ");

  GetLastSaveValues();
  WeekdayUpdate();
  SpecialDayUpdate();
     //VarInit() ;
  Wire.begin(SDA,SCL); Serial.println("I2C BEGIN DONE ");
  
  // Route setup
  setupWebServer();
Serial.println("WEB SERVER SETTUP DONE ");
alarm_updated=false;
}
/********************************************************************************************************************LOOP FUNCTION****************************************/
void loop() {
  server.handleClient();
  checkAndSetAlarms();
  executeScheduledTasks();
  excecute_alarm();
  delay(100);
  

}

/********************************************************************************************************************SET TO DEFAULE PARAMETERS FUNCTION****************************************/
