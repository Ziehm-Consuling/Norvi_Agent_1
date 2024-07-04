// put function declarations here:

#include <Arduino.h>


#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include "features/EEPROM_Features.hpp"
#include "features/WIFI_Features.hpp"




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
