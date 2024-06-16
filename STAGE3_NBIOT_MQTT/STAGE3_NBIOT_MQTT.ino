#include <ArduinoJson.h>
#include <EEPROM.h>
/********************************************************************************************************************NETWORK PAGE VARIABLES****************************************/
String NETWORK_APN = "wsim";
String MQTT_SERVER = "portal.edgefactory.io";
String MQTT_USER = "user";
int MQTT_PORT = 1884 ;
String MQTT_PW = "HEI@12345678#";
String MQTT_ID = "clientExample";
String MQTT_DN = "adl_connect";
String MQTT_SD = "551965641856";

/********************************************************************************************************************SCHEDULER PAGE VARIABLES****************************************/
String MONDAY_ON_TIME = "08:05";
String MONDAY_OFF_TIME = "08:50";
String TUESDAY_ON_TIME = "01:3";
String TUESDAY_OFF_TIME = "11:12";
String WEDNESDAY_ON_TIME = "04:6";
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

// Define starting addresses for each text zone
#define EEPROM_SIZE 256
#define ADDR_UI_NETWORK 0
#define ADDR_UI_SIGNAL 7
#define ADDR_UI_STATUS_MQTT 13 
#define ADDR_NETWORK_APN 23
#define ADDR_MQTT_SERVER 33
#define ADDR_MQTT_USER 43
#define ADDR_MQTT_PW 50
#define ADDR_MQTT_DN 60
#define ADDR_MQTT_ID 72
#define ADDR_MQTT_SD 86
#define ADDR_MONDAY_ON_TIME 100 
#define ADDR_MONDAY_OFF_TIME 102
#define ADDR_TUESDAY_ON_TIME 104
#define ADDR_TUESDAY_OFF_TIME 106
#define ADDR_WEDNESDAY_ON_TIME 108
#define ADDR_WEDNESDAY_OFF_TIME 110
#define ADDR_THURSDAY_ON_TIME 112
#define ADDR_THURSDAY_OFF_TIME 114
#define ADDR_FRIDAY_ON_TIME 116
#define ADDR_FRIDAY_OFF_TIME 118
#define ADDR_SATURDAY_ON_TIME 120
#define ADDR_SATURDAY_OFF_TIME 122
#define ADDR_SUNDAY_ON_TIME 124
#define ADDR_SUNDAY_OFF_TIME 126
#define ADDR_D1_ON_TIME 128
#define ADDR_D1_OFF_TIME 130
#define ADDR_D2_ON_TIME 132
#define ADDR_D2_OFF_TIME 134
#define ADDR_D3_ON_TIME 136
#define ADDR_D3_OFF_TIME 138
#define ADDR_D4_ON_TIME 140
#define ADDR_D4_OFF_TIME 142
#define ADDR_D5_ON_TIME 144
#define ADDR_D5_OFF_TIME 146
#define ADDR_DAY1 156
#define ADDR_DAY2 166
#define ADDR_DAY3 176
#define ADDR_DAY4 186
#define ADDR_DAY5 196
#define ADDR_DAY6 106
/**********************************************************************************************************************************/
//Modem Serial Pins
#define RX_PIN 4 // ESP32 RX pin
#define TX_PIN 5 // ESP32 TX pin
#define BAUD_RATE 9600 // Baud rate for ESP32 UART communication
/**********************************************************************************************************************************/
// Define MQTT topics
#define MQTT_TOPIC_1 "v1/devices/me/rpc/request/1"
#define MQTT_TOPIC_2 "v1/devices/me/rpc/request/2"
#define MQTT_TOPIC_5 "v1/devices/me/rpc/request/3"
#define MQTT_TOPIC_3 "v1/devices/me/attributes"
#define MQTT_TOPIC_4 "v1/devices/me/telemetry"
/**********************************************************************************************************************************/
// Define your firmware version and serial number here
String firmwareVersion = "1.23";
String serialNumber = "SN-12345";
String mqttMessage = "";
String payload ="";
bool Ignore = false;
/**********************************************************************************************************************************/
// Define your sensor variables here
int energy = 65451;
float current = 12.5;
/**********************************************************************************************************************************/
//Define a constant for the interval (in milliseconds)
#define MQTT_CHECK_INTERVAL  10000 // 10 seconds
#define SENSOR_SEND_INTERVAL  60000 // 60 seconds
/**********************************************************************************************************************************/
//Get the current time
unsigned long currentMillis = 0;
//Define a variable to store the last time the MQTT connection was checked
unsigned long lastMQTTCheckTime = 0;
unsigned long previousMillis = 0;
//Define a variable to store the last time Sensor data was sent
unsigned long lastSensorValSentTime = 0;

unsigned long last_modem_response=0;
unsigned long millis_qmt_request=0;

/********************************************************************************************************************TIME STURCTURE DEFINITION****************************************/
struct Time {
    uint8_t hour;
    uint8_t minute;
};
/********************************************************************************************************************DATE STRUCTURE DEFINITION****************************************/
struct Date {
    uint8_t day;
    uint8_t month;
    int year;
};
/********************************************************************************************************************SCHEDULE STRUCTURE DEFINITION****************************************/
struct Schedule {
    Time on_time;
    Time off_time;
    Date day; // For special dates only
};
/********************************************************************************************************************PARSE TIME FUNCTION****************************************/
Time parseTime(const String& timeStr) {
    Time t;
    // Check if ':' exists in the string
    int pos = timeStr.indexOf(':');
    if (pos != -1) { // If ':' is found
        // Split the string into hour and minute parts
        String hourStr = timeStr.substring(0, pos);
        String minuteStr = timeStr.substring(pos + 1);
        // Remove leading zeros and trim
        hourStr.trim();
        minuteStr.trim();
        // Store hour and minute as uint8_t
        t.hour = hourStr.toInt();
        t.minute = minuteStr.toInt();
    } else { // If ':' is not found, consider the entire string as hour
        // Store hour as uint8_t
        t.hour = timeStr.toInt();
        // Set minutes to 0
        t.minute = 0;
    }
    return t;
}
/********************************************************************************************************************PARSE DATE FUNCTION ****************************************/
Date parseDate(const String& dateStr) {
    Date d;
    // Find the positions of the dashes in the string
    int firstDash = dateStr.indexOf('-');
    int secondDash = dateStr.indexOf('-', firstDash + 1);
    // Extract day, month, and year substrings
    String dayStr = dateStr.substring(0, firstDash);
    String monthStr = dateStr.substring(firstDash + 1, secondDash);
    String yearStr = dateStr.substring(secondDash + 1);
    // Remove leading zeros and trim
    dayStr.trim();
    monthStr.trim();
    yearStr.trim();
    // Store day, month, and year as uint8_t
    d.day = dayStr.toInt();
    d.month = monthStr.toInt();
    d.year = yearStr.toInt();
    return d;
}
/********************************************************************************************************************WEEK SCHEDULE VARIABLES****************************************/
Schedule monday;
Schedule tuesday;
Schedule wednesday;
Schedule thursday;
Schedule friday;
Schedule saturday;
Schedule sunday;
/********************************************************************************************************************SPECIAL DATE SCHEDULE VARIABLES****************************************/
Schedule specialDates[5];
/********************************************************************************************************************WEEKDAY UPDATE FUNCTION****************************************/
void WeekdayUpdate(){
// Monday
monday.on_time = parseTime(MONDAY_ON_TIME);
monday.off_time = parseTime(MONDAY_OFF_TIME);
// Tuesday
tuesday.on_time = parseTime(TUESDAY_ON_TIME);
tuesday.off_time = parseTime(TUESDAY_OFF_TIME);
// Wednesday
wednesday.on_time = parseTime(WEDNESDAY_ON_TIME);
wednesday.off_time = parseTime(WEDNESDAY_OFF_TIME);
// Thursday
thursday.on_time = parseTime(THURSDAY_ON_TIME);
thursday.off_time = parseTime(THURSDAY_OFF_TIME);
// Friday
friday.on_time = parseTime(FRIDAY_ON_TIME);
friday.off_time = parseTime(FRIDAY_OFF_TIME);
// Saturday
saturday.on_time = parseTime(SATURDAY_ON_TIME);
saturday.off_time = parseTime(SATURDAY_OFF_TIME);
// Sunday
sunday.on_time = parseTime(SUNDAY_ON_TIME);
sunday.off_time = parseTime(SUNDAY_OFF_TIME);
}
/********************************************************************************************************************SPECIAL DAYS UPDATE FUNCTION****************************************/
void SpecialDayUpdate(){  
// Special Date 1
specialDates[0].on_time = parseTime(D1_ON_TIME);
specialDates[0].off_time = parseTime(D1_OFF_TIME);
specialDates[0].day = parseDate(DAY1);
// Special Date 2
specialDates[1].on_time = parseTime(D2_ON_TIME);
specialDates[1].off_time = parseTime(D2_OFF_TIME);
specialDates[1].day = parseDate(DAY2);
// Special Date 3
specialDates[2].on_time = parseTime(D3_ON_TIME);
specialDates[2].off_time = parseTime(D3_OFF_TIME);
specialDates[2].day = parseDate(DAY3);
// Special Date 4
specialDates[3].on_time = parseTime(D4_ON_TIME);
specialDates[3].off_time = parseTime(D4_OFF_TIME);
specialDates[3].day = parseDate(DAY4);
// Special Date 5
specialDates[4].on_time = parseTime(D5_ON_TIME);
specialDates[4].off_time = parseTime(D5_OFF_TIME);
specialDates[4].day = parseDate(DAY5);
}
/********************************************************************************************************************GET TIME FROM EEPROM VALUES FUNCTION****************************************/
String GetTimeFromEEPROM(int Address) {
    int hour = EEPROM.read(Address);
    int minute = EEPROM.read(Address + 1);
    String hourString = (hour < 10 ? "0" + String(hour) : String(hour));
    String minuteString = (minute < 10 ? "0" + String(minute) : String(minute));
    return hourString + ":" + minuteString;
}
/********************************************************************************************************************GET LAST STORED VALUES FUNCTION****************************************/
void GetLastSaveValues(){
  // Read schedule data
MONDAY_ON_TIME = GetTimeFromEEPROM(ADDR_MONDAY_ON_TIME);
MONDAY_OFF_TIME =GetTimeFromEEPROM(ADDR_MONDAY_OFF_TIME);
TUESDAY_ON_TIME = GetTimeFromEEPROM(ADDR_TUESDAY_ON_TIME);
TUESDAY_OFF_TIME =GetTimeFromEEPROM(ADDR_TUESDAY_OFF_TIME);
WEDNESDAY_ON_TIME = GetTimeFromEEPROM(ADDR_WEDNESDAY_ON_TIME);
WEDNESDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_WEDNESDAY_OFF_TIME);
THURSDAY_ON_TIME = GetTimeFromEEPROM(ADDR_THURSDAY_ON_TIME);
THURSDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_THURSDAY_OFF_TIME);
FRIDAY_ON_TIME = GetTimeFromEEPROM(ADDR_FRIDAY_ON_TIME);
FRIDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_FRIDAY_OFF_TIME);
SATURDAY_ON_TIME = GetTimeFromEEPROM(ADDR_SATURDAY_ON_TIME);
SATURDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_SATURDAY_OFF_TIME);
SUNDAY_ON_TIME = GetTimeFromEEPROM(ADDR_SUNDAY_ON_TIME);
SUNDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_SUNDAY_OFF_TIME);
D1_ON_TIME = GetTimeFromEEPROM(ADDR_D1_ON_TIME);
D1_OFF_TIME = GetTimeFromEEPROM(ADDR_D1_OFF_TIME);
D2_ON_TIME = GetTimeFromEEPROM(ADDR_D2_ON_TIME);
D2_OFF_TIME = GetTimeFromEEPROM(ADDR_D2_OFF_TIME);
D3_ON_TIME = GetTimeFromEEPROM(ADDR_D3_ON_TIME);
D3_OFF_TIME = GetTimeFromEEPROM(ADDR_D3_OFF_TIME);
D4_ON_TIME = GetTimeFromEEPROM(ADDR_D4_ON_TIME);
D4_OFF_TIME = GetTimeFromEEPROM(ADDR_D4_OFF_TIME);
D5_ON_TIME = GetTimeFromEEPROM(ADDR_D5_ON_TIME);
D5_OFF_TIME = GetTimeFromEEPROM(ADDR_D5_OFF_TIME);
DAY1 = readString(ADDR_DAY1, ADDR_DAY2 - ADDR_DAY1);
DAY2 = readString(ADDR_DAY2, ADDR_DAY3 - ADDR_DAY2);
DAY3 = readString(ADDR_DAY3, ADDR_DAY4 - ADDR_DAY3);
DAY4 = readString(ADDR_DAY4, ADDR_DAY5 - ADDR_DAY4);
DAY5 = readString(ADDR_DAY5, ADDR_DAY6 - ADDR_DAY5);
}
/********************************************************************************************************************PARSE DIGIT  FUNCTION*************************************************/
// parseDigit
uint8_t parseDigits(char* str, uint8_t count)
{
    uint8_t val = 0;
    while(count-- > 0) val = (val * 10) + (*str++ - '0');
    return val;
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
void handleSave() {
    WeekdayUpdate();//update Week days int values
    SpecialDayUpdate();//Update special days
    //Store values as int in eeprom
    // Monday
    EEPROM.write(ADDR_MONDAY_ON_TIME, monday.on_time.hour);
    EEPROM.write(ADDR_MONDAY_ON_TIME + 1, monday.on_time.minute);
    EEPROM.write(ADDR_MONDAY_OFF_TIME, monday.off_time.hour);
    EEPROM.write(ADDR_MONDAY_OFF_TIME + 1, monday.off_time.minute);
    // Tuesday
    EEPROM.write(ADDR_TUESDAY_ON_TIME, tuesday.on_time.hour);
    EEPROM.write(ADDR_TUESDAY_ON_TIME + 1, tuesday.on_time.minute);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME, tuesday.off_time.hour);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME + 1, tuesday.off_time.minute);
    // Wednesday
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME, wednesday.on_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME + 1, wednesday.on_time.minute);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME, wednesday.off_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME + 1, wednesday.off_time.minute);
    // Thursday
    EEPROM.write(ADDR_THURSDAY_ON_TIME, thursday.on_time.hour);
    EEPROM.write(ADDR_THURSDAY_ON_TIME + 1, thursday.on_time.minute);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME, thursday.off_time.hour);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME + 1, thursday.off_time.minute);
    // Friday
    EEPROM.write(ADDR_FRIDAY_ON_TIME, friday.on_time.hour);
    EEPROM.write(ADDR_FRIDAY_ON_TIME + 1, friday.on_time.minute);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME, friday.off_time.hour);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME + 1, friday.off_time.minute);
    // Saturday
    EEPROM.write(ADDR_SATURDAY_ON_TIME, saturday.on_time.hour);
    EEPROM.write(ADDR_SATURDAY_ON_TIME + 1, saturday.on_time.minute);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME, saturday.off_time.hour);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME + 1, saturday.off_time.minute);
    // Sunday
    EEPROM.write(ADDR_SUNDAY_ON_TIME, sunday.on_time.hour);
    EEPROM.write(ADDR_SUNDAY_ON_TIME + 1, sunday.on_time.minute);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME, sunday.off_time.hour);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME + 1, sunday.off_time.minute);
    // Special Day1
    EEPROM.write(ADDR_D1_ON_TIME, specialDates[0].on_time.hour);
    EEPROM.write(ADDR_D1_ON_TIME + 1, specialDates[0].on_time.minute);
    EEPROM.write(ADDR_D1_OFF_TIME, specialDates[0].off_time.hour);
    EEPROM.write(ADDR_D1_OFF_TIME + 1, specialDates[0].off_time.minute);
    // Special Day2
    EEPROM.write(ADDR_D2_ON_TIME, specialDates[1].on_time.hour);
    EEPROM.write(ADDR_D2_ON_TIME + 1, specialDates[1].on_time.minute);
    EEPROM.write(ADDR_D2_OFF_TIME, specialDates[1].off_time.hour);
    EEPROM.write(ADDR_D2_OFF_TIME + 1, specialDates[1].off_time.minute);
    // Special Day3
    EEPROM.write(ADDR_D3_ON_TIME, specialDates[2].on_time.hour);
    EEPROM.write(ADDR_D3_ON_TIME + 1, specialDates[2].on_time.minute);
    EEPROM.write(ADDR_D3_OFF_TIME, specialDates[2].off_time.hour);
    EEPROM.write(ADDR_D3_OFF_TIME + 1, specialDates[2].off_time.minute);
    // Special Day4
    EEPROM.write(ADDR_D4_ON_TIME, specialDates[3].on_time.hour);
    EEPROM.write(ADDR_D4_ON_TIME + 1, specialDates[3].on_time.minute);
    EEPROM.write(ADDR_D4_OFF_TIME, specialDates[3].off_time.hour);
    EEPROM.write(ADDR_D4_OFF_TIME + 1, specialDates[3].off_time.minute);
    // Special Day5 
    EEPROM.write(ADDR_D5_ON_TIME, specialDates[4].on_time.hour);
    EEPROM.write(ADDR_D5_ON_TIME + 1, specialDates[4].on_time.minute);
    EEPROM.write(ADDR_D5_OFF_TIME, specialDates[4].off_time.hour);
    EEPROM.write(ADDR_D5_OFF_TIME + 1, specialDates[4].off_time.minute);
    writeString(ADDR_DAY1, DAY1);
    writeString(ADDR_DAY2, DAY2);
    writeString(ADDR_DAY3, DAY3);
    writeString(ADDR_DAY4, DAY4);
    writeString(ADDR_DAY5, DAY5);
    EEPROM.commit(); // Committing changes to EEPROM
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// monitor serial
void monitorSerial1() {
  while (Serial1.available()) {
        if(Ignore) { 
      //reset to ignore the sent loop    
        payload = String("No payload!");
        mqttMessage = String("No message!");
    } else {mqttMessage = Serial1.readStringUntil('\n');};
    Serial.println(mqttMessage);
  }
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// function to check if the devce is still connected
bool isConnected() {
    // Send the AT command to check MQTT connection status
    String command = "AT+QMTCONN?";
    Serial1.println(command);
    Serial.println(command);
    
    // Wait for response
    previousMillis = millis();
    while (millis() - previousMillis < 8000) {
        monitorSerial1();
        if (Serial1.find("+QMTCONN: 0,3")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Connected!");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,2")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Being Connected");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,1")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Initializing");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,4")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Disconnected");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return false;
        }
    }
    // If no response received within timeout, consider it disconnected
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    Serial.println("MQTT Status: Disconnected (Timeout)");
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    return false;
}

/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to register on the network with APN
void registerOnNetwork(const String& apn) {
    int failedAttempts = 0;

    while (failedAttempts < 5) {
        // Send CFUN command
        Serial1.println("AT+CFUN=1");
        Serial.println("AT+CFUN=1");
        previousMillis = millis();
        while (millis() - previousMillis < 5000) {
            monitorSerial1();
            if (Serial1.find("OK")) break;
        }

        // Send CEREG command
        Serial1.println("AT+CEREG=1");
        Serial.println("AT+CEREG=1");
        previousMillis = millis();
        while (millis() - previousMillis < 5000) {
            monitorSerial1();
            if (Serial1.find("OK")) break;
        }

        // Send CGDCONT command with provided APN
        String cgdcontCmd = "AT+CGDCONT=1,\"IP\",\"" + apn + "\"";
        Serial1.println(cgdcontCmd);
        Serial.println(cgdcontCmd);
        previousMillis = millis();
        while (millis() - previousMillis < 5000) {
            monitorSerial1();
            if (Serial1.find("OK")) break;
        }

        // Send CGATT command
        Serial1.println("AT+CGATT=1");
        Serial.println("AT+CGATT=1");
        previousMillis = millis();
        while (millis() - previousMillis < 5000) {
            monitorSerial1();
            if (Serial1.find("OK")) break;
        }

        // Wait for registration with the network
        previousMillis = millis();
        bool registrationSuccess = false;
        while (millis() - previousMillis < 20000) {
            monitorSerial1();
            if (Serial1.find("+CEREG:2")) {
                registrationSuccess = true;
                break;
            }
        }

        // Check if registration was successful
        if (registrationSuccess) {
            // Wait for final registration status
            previousMillis = millis();
            while (millis() - previousMillis < 20000) {
                monitorSerial1();
                if (Serial1.find("+CEREG:5")) {
                    Serial.println("Modem configured and registered on the network.");
                    return; // Successful registration, exit function
                }
            }
        }

        // If registration failed, increment the failedAttempts counter
        failedAttempts++;

        // If we've reached 5 failed attempts, reboot the modem and reset the counter
        if (failedAttempts == 5) {
            Serial.println("Failed to register on the network after 5 attempts. Rebooting modem...");
            rebootModem();
            failedAttempts = 0; // Reset the counter
        }

        Serial.println("Failed to register on the network. Retrying...");
    }
}

/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to connect to MQTT Broker
void connectToMQTTBroker(const String& server, const String& user, int port, const String& password, const String& clientId) {
  // Send QMTOPEN command
  OpenMQ:;
  String qmtopenCmd = "AT+QMTOPEN=0,\"" + server + "\"," + String(port);
  Serial1.println(qmtopenCmd);
  Serial.println(qmtopenCmd);
  previousMillis = millis();
  while (millis() - previousMillis < 10000) {
    monitorSerial1();
    if (Serial1.find("OK")) break;
    if (Serial1.find("ERROR")){Serial.println("Retry MQTT OPEN");delay(3000); goto OpenMQ;};
  }
  previousMillis = millis();
  while (millis() - previousMillis < 15000) {
    monitorSerial1();
    if (Serial1.find("+QMTOPEN")) break;
  }
  // Send QMTCONN command
  ConnMQ:;
  String qmtconnCmd = "AT+QMTCONN=0,\"" + clientId + "\",\"" + user + "\",\"" + password + "\"";
  Serial1.println(qmtconnCmd);
  Serial.println(qmtconnCmd);
  previousMillis = millis();
  while (millis() - previousMillis < 10000) {
    monitorSerial1();
    if (Serial1.find("OK")) break;
    if (Serial1.find("ERROR")){Serial.println("Retry MQTT CONN");delay(3000); goto ConnMQ;};
  }
  previousMillis = millis();
  while (millis() - previousMillis < 15000) {
    monitorSerial1();
    if (Serial1.find("+QMTCONN")) break;
  }
  Serial.println("Successfully connected to MQTT broker.");
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to subscribe to MQTT topics one at a time
void subscribeToMQTTTopic(const String& topic) {
  // Send QMTSUB command
  String qmtsubCmd = "AT+QMTSUB=0,1,\"" + topic + "\",2";
  Serial1.println(qmtsubCmd);
  Serial.println(qmtsubCmd);
  previousMillis = millis();
  while (millis() - previousMillis < 10000) {
    monitorSerial1();
  }
  Serial.println("Subscribed to " + topic + " successfully.");
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to publish MQTT message with ArduinoJson payload
void publishMQTTMessage(const JsonDocument& jsonDoc, String topic){
  // Serialize the JSON document to a string
  String payload;
  serializeJson(jsonDoc, payload);
  // Calculate payload length
  int payloadLength = payload.length();
  // Prepare the AT command to publish the message with payload length
  String command = "AT+QMTPUB=0,0,0,0,\"" + topic + "\"," + String(payloadLength);
  Serial1.println(command);
  Serial.print(command);
  // Wait for the modem to respond with '>'
  unsigned long previousMillis = millis();
  while (1) {
    String response = Serial1.readStringUntil('\n');
    if (response.startsWith(">")) {
      monitorSerial1(); 
      break;
    }
    if (millis() - previousMillis > 6000) {
      monitorSerial1();
      Serial.println("Failed to receive prompt for message input.");
      return;
    }
  }
  // Send the payload
  Serial1.println(payload);
  Serial.println(payload);
   previousMillis = millis();
   while (millis() - previousMillis < 10000) {
      monitorSerial1();
      if(Serial1.find("OK"))break;
    };
   previousMillis = millis();
   while (millis() - previousMillis < 10000) {
      monitorSerial1();
      if(Serial1.find("+QMTPUB: 0,0,0"))break;
    };
}


/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to publish device information via MQTT with specified firmware version and serial number
void publishDeviceInfo(const String& firmwareVersion, const String& serialNumber) {
  // Create a JSON document to hold device information
  const size_t capacity = JSON_OBJECT_SIZE(2);
  DynamicJsonDocument jsonDoc(capacity);
  // Add device information to the JSON document
  jsonDoc["firmwareVersion"] = firmwareVersion;
  jsonDoc["serialNumber"] = serialNumber;
  // Publish the JSON document to MQTT topic MQTT_TOPIC_4
  publishMQTTMessage(jsonDoc, MQTT_TOPIC_4);
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to publish sensor values via MQTT with specified energy and current values
void publishSensorValues(int energy, float current) {
  // Create a JSON document to hold sensor values
  const size_t capacity = JSON_OBJECT_SIZE(2);
  DynamicJsonDocument jsonDoc(capacity);
  // Add sensor values to the JSON document
  jsonDoc["energy"] = energy;
  jsonDoc["current"] = current;
  // Publish the JSON document to MQTT topic MQTT_TOPIC_4
  publishMQTTMessage(jsonDoc, MQTT_TOPIC_4);
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to handle messages received on MQTT_TOPIC_1
void handleMQTTTopic1(const String& payload) {
  // Parse the JSON payload
  DynamicJsonDocument doc(512); // Adjust the capacity as needed
  DeserializationError error = deserializeJson(doc, payload);
  // Check for parsing errors
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  // Check if the JSON object contains the "method" field
  if (doc.containsKey("method")) {
    // Get the value of the "method" field
    String method = doc["method"];
    // Check if the method is "reboot"
    if (method == "reboot") {
      // Perform reboot operation
      // Example: Code to reboot the device
      Serial.println("--------------------------------------------------------------------------------------------------------------");
      Serial.println("");
      Serial.println("");
      Serial.print("Rebooting device");
      // Publish the response payload to the MQTT server
      // Create a JSON document to hold the message
      DynamicJsonDocument jsonDoc(512); // Adjust the capacity as needed
      jsonDoc["message"] = "Rebooting device";
      // Publish the JSON document to MQTT topic MQTT_TOPIC_1
      publishMQTTMessage(jsonDoc, MQTT_TOPIC_1);
      Ignore = true;
      delay(1000);
      ESP.restart(); // Perform a device reboot
      //rebootModem();
    } else {
      // If the method is not recognized, you can log an error or perform other actions as needed
      Serial.println("Unknown method in MQTT_TOPIC_1 payload");
    }
  } else {
    Serial.println("No 'method' field found in MQTT_TOPIC_1 payload");
  }
}
/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to handle messages received on MQTT_TOPIC_2
void handleMQTTTopic2(const String& payload) {
    // Parse the JSON payload
    DynamicJsonDocument doc(512); // Adjust the capacity as needed
    DeserializationError error = deserializeJson(doc, payload);
    // Check for parsing errors
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    // Check if the JSON object contains the "AC" field
    if (doc.containsKey("AC")) {
        // Get the value of the "AC" field
        int AC = doc["AC"];
        // Construct response payload based on "AC" value
        JsonObject responseJson = doc.to<JsonObject>(); // Copy the received JSON document
        switch (AC) {
            case 1:
                responseJson["AC"] = AC;
                responseJson["MOON"] = MONDAY_ON_TIME;
                responseJson["MOOF"] = MONDAY_OFF_TIME;
                break;
            case 2:
                responseJson["AC"] = AC;
                responseJson["TUON"] = TUESDAY_ON_TIME;
                responseJson["TUOF"] = TUESDAY_OFF_TIME;
                break;
            case 3:
                responseJson["AC"] = AC;
                responseJson["WEON"] = WEDNESDAY_ON_TIME;
                responseJson["WEOF"] = WEDNESDAY_OFF_TIME;
                break;
            case 4:
                responseJson["AC"] = AC;
                responseJson["THON"] = THURSDAY_ON_TIME;
                responseJson["THOF"] = THURSDAY_OFF_TIME;
                break;
            case 5:
                responseJson["AC"] = AC;
                responseJson["FRON"] = FRIDAY_ON_TIME;
                responseJson["FROF"] = FRIDAY_OFF_TIME;
                break;
            case 6:
                responseJson["AC"] = AC;
                responseJson["SAON"] = SATURDAY_ON_TIME;
                responseJson["SAOF"] = SATURDAY_OFF_TIME;
                break;
            case 7:
                responseJson["AC"] = AC;
                responseJson["SUON"] = SUNDAY_ON_TIME;
                responseJson["SUOF"] = SUNDAY_OFF_TIME;
                break;
            case 8:
                responseJson["AC"] = AC;
                responseJson["D1ON"] = D1_ON_TIME;
                responseJson["D1OF"] = D1_OFF_TIME;
                responseJson["DAY"] = DAY1;
                break;
            case 9:
                responseJson["AC"] = AC;
                responseJson["D2ON"] = D2_ON_TIME;
                responseJson["D2OF"] = D2_OFF_TIME;
                responseJson["DAY"] = DAY2;
                break;
            case 10:
                responseJson["AC"] = AC;
                responseJson["D3ON"] = D3_ON_TIME;
                responseJson["D3OF"] = D3_OFF_TIME;
                responseJson["DAY"] = DAY3;
                break;
            case 11:
                responseJson["AC"] = AC;
                responseJson["D4ON"] = D4_ON_TIME;
                responseJson["D4OF"] = D4_OFF_TIME;
                responseJson["DAY"] = DAY4;
                break;
            case 12:
                responseJson["AC"] = AC;
                responseJson["D5ON"] = D5_ON_TIME;
                responseJson["D5OF"] = D5_OFF_TIME;
                responseJson["DAY"] = DAY5;
                break;
            default:
                Serial.println("Invalid 'AC' value in MQTT_TOPIC_2 payload");
                return;
        }

        // Publish the constructed response payload
        publishMQTTMessage(responseJson, MQTT_TOPIC_5);
        Ignore = true;
    } else {
        Serial.println("No 'AC' field found in MQTT_TOPIC_2 payload");
        return;
    }
}

/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
// Function to handle messages received on MQTT_TOPIC_3
void handleMQTTTopic3(const String& payload) {
  GetLastSaveValues();//get datas from eeprom
  WeekdayUpdate();//update Week days int values
  SpecialDayUpdate();//Update special days
    // Parse the JSON payload
    DynamicJsonDocument doc(128); // Adjust the capacity as needed
    DeserializationError error = deserializeJson(doc, payload);

    // Check for parsing errors
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    // Check if the JSON object contains the "AC" field
    if (doc.containsKey("AC")) {
        // Get the value of the "AC" field
        int AC = doc["AC"];

        // Construct the response JSON document based on the requested attribute code
        DynamicJsonDocument responseDoc(128); // Adjust the capacity as needed

        // Update variables based on the value of "AC"
        switch (AC) {
            case 1:
                MONDAY_ON_TIME = doc["MOON"].as<String>();
                MONDAY_OFF_TIME = doc["MOOF"].as<String>();
                Serial.println("Updated MONDAY_ON_TIME: " + MONDAY_ON_TIME);
                Serial.println("Updated MONDAY_OFF_TIME: " + MONDAY_OFF_TIME);

                break;
            case 2:
                TUESDAY_ON_TIME = doc["TUON"].as<String>();
                TUESDAY_OFF_TIME = doc["TUOF"].as<String>();
                Serial.println("Updated TUESDAY_ON_TIME: " + TUESDAY_ON_TIME);
                Serial.println("Updated TUESDAY_OFF_TIME: " + TUESDAY_OFF_TIME);

                break;
            case 3:
                WEDNESDAY_ON_TIME = doc["WEON"].as<String>();
                WEDNESDAY_OFF_TIME = doc["WEOF"].as<String>();
                Serial.println("Updated WEDNESDAY_ON_TIME: " + WEDNESDAY_ON_TIME);
                Serial.println("Updated WEDNESDAY_OFF_TIME: " + WEDNESDAY_OFF_TIME);

                break;
            case 4:
                THURSDAY_ON_TIME = doc["THON"].as<String>();
                THURSDAY_OFF_TIME = doc["THOF"].as<String>();
                Serial.println("Updated THURSDAY_ON_TIME: " + THURSDAY_ON_TIME);
                Serial.println("Updated THURSDAY_OFF_TIME: " + THURSDAY_OFF_TIME);

                break;
            case 5:
                FRIDAY_ON_TIME = doc["FRON"].as<String>();
                FRIDAY_OFF_TIME = doc["FROF"].as<String>();
                Serial.println("Updated FRIDAY_ON_TIME: " + FRIDAY_ON_TIME);
                Serial.println("Updated FRIDAY_OFF_TIME: " + FRIDAY_OFF_TIME);

                break;
            case 6:
                SATURDAY_ON_TIME = doc["SAON"].as<String>();
                SATURDAY_OFF_TIME = doc["SAOF"].as<String>();
                Serial.println("Updated SATURDAY_ON_TIME: " + SATURDAY_ON_TIME);
                Serial.println("Updated SATURDAY_OFF_TIME: " + SATURDAY_OFF_TIME);

                break;
            case 7:
                SUNDAY_ON_TIME = doc["SUON"].as<String>();
                SUNDAY_OFF_TIME = doc["SUOF"].as<String>();
                Serial.println("Updated SUNDAY_ON_TIME: " + SUNDAY_ON_TIME);
                Serial.println("Updated SUNDAY_OFF_TIME: " + SUNDAY_OFF_TIME);

                break;
            case 8:
                D1_ON_TIME = doc["D1ON"].as<String>();
                D1_OFF_TIME = doc["D1OF"].as<String>();
                DAY1 = doc["DAY"].as<String>();
                Serial.println("Updated D1_ON_TIME: " + D1_ON_TIME);
                Serial.println("Updated D1_OFF_TIME: " + D1_OFF_TIME);
                Serial.println("Updated DAY1: " + DAY1);

                break;
            case 9:
                D2_ON_TIME = doc["D2ON"].as<String>();
                D2_OFF_TIME = doc["D2OF"].as<String>();
                DAY2 = doc["DAY"].as<String>();
                Serial.println("Updated D2_ON_TIME: " + D2_ON_TIME);
                Serial.println("Updated D2_OFF_TIME: " + D2_OFF_TIME);
                Serial.println("Updated DAY2: " + DAY2);

                break;
            case 10:
                D3_ON_TIME = doc["D3ON"].as<String>();
                D3_OFF_TIME = doc["D3OF"].as<String>();
                DAY3 = doc["DAY"].as<String>();
                Serial.println("Updated D3_ON_TIME: " + D3_ON_TIME);
                Serial.println("Updated D3_OFF_TIME: " + D3_OFF_TIME);
                Serial.println("Updated DAY3: " + DAY3);

                break;
            case 11:
                D4_ON_TIME = doc["D4ON"].as<String>();
                D4_OFF_TIME = doc["D4OF"].as<String>();
                DAY4 = doc["DAY"].as<String>();
                Serial.println("Updated D4_ON_TIME: " + D4_ON_TIME);
                Serial.println("Updated D4_OFF_TIME: " + D4_OFF_TIME);
                Serial.println("Updated DAY4: " + DAY4);
                
                break;
            case 12:
                D5_ON_TIME = doc["D5ON"].as<String>();
                D5_OFF_TIME = doc["D5OF"].as<String>();
                DAY5 = doc["DAY"].as<String>();
                Serial.println("Updated D5_ON_TIME: " + D5_ON_TIME);
                Serial.println("Updated D5_OFF_TIME: " + D5_OFF_TIME);
                Serial.println("Updated DAY5: " + DAY5);

                break;
            default:
                Serial.println("Invalid 'AC' value in MQTT_TOPIC_2 payload");
                return;
        }

        //Update the Store variables
        StoreVarIEEPROM();
        // Construct a JsonDocument and add a JsonObject to it
        DynamicJsonDocument doc(128); // Adjust the capacity as needed
        JsonObject messageObj = doc.to<JsonObject>();
        messageObj["message"] = "Variables updated for AC: " + String(AC);

        // Publish the constructed message
        publishMQTTMessage(doc, MQTT_TOPIC_3);
        Ignore = true;

    } else {
        Serial.println("No 'AC' field found in MQTT_TOPIC_2 payload");
    }
}

/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
//Reboot the Modem
void rebootModem() {
  // Send AT command
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 1300) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) break;
    }
  }

  // Send reboot command
  Serial1.println("AT+NRB");
  unsigned long previousMillis = millis();
  while (millis() - previousMillis < 10000) {
    monitorSerial1();
  }
}

//Check Modem Response
bool modem_response_check() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 1300) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_sim_check() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CPIN");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_function() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CFUN?");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("+CFUN:1")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_setfunction() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CFUN=1");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_check_reg() {
  bool response = false;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CEREG?");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      if (Serial1.available()) {
        String command = Serial1.readStringUntil('\n');
        if (command.startsWith("+CEREG:")) {
          int commaIndex = command.indexOf(',');
          if (commaIndex > -1) {
            int status = command.substring(commaIndex + 1).toInt();
            if (status == 1 || status == 5) {
              response = true;
              break;
            }
          }
        }
      }
    }
    if (response) {
      break;
    }
  }
  return response;
}

bool modem_setreg() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CEREG=1");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_check_cgatt() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CGATT?");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("+CGATT:1")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_set_apn(const String& apn) {
  // Send AT command
  bool response=0;

  String cgdcontCmd = "AT+CGDCONT=1,\"IP\",\"" + apn + "\"";
        Serial1.println(cgdcontCmd);
        Serial.println(cgdcontCmd);
        previousMillis = millis();
        while (millis() - previousMillis < 5000) {
            monitorSerial1();
            if (Serial1.find("OK")) {
              response=1;
              break;
            }
        }
  return response;
}

bool modem_setcgatt() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+CGATT=1");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("OK")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool isConnected() {
    // Send the AT command to check MQTT connection status
    String command = "AT+QMTCONN?";
    Serial1.println(command);
    Serial.println(command);
    
    // Wait for response
    previousMillis = millis();
    while (millis() - previousMillis < 8000) {
        monitorSerial1();
        if (Serial1.find("+QMTCONN: 0,3")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Connected!");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,2")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Being Connected");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,1")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Initializing");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return true;
        } else if (Serial1.find("+QMTCONN: 0,4")) {
            Serial.println("--------------------------------------------------------------------------------------------------------------");     
            Serial.println("MQTT Status: Disconnected");
            Serial.println("--------------------------------------------------------------------------------------------------------------");
            return false;
        }
    }
    // If no response received within timeout, consider it disconnected
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    Serial.println("MQTT Status: Disconnected (Timeout)");
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    return false;
}

bool modem_set_broker(const String& server,int port) {
  // Send AT command
  bool response=0;

  String qmtopenCmd = "AT+QMTOPEN=0,\"" + server + "\"," + String(port);
  Serial1.println(qmtopenCmd);
  Serial.println(qmtopenCmd);
  previousMillis = millis();
  while (millis() - previousMillis < 1000) {
    monitorSerial1();
    if (Serial1.find("OK")){
      response=1;
      break;
    } 
    if (Serial1.find("ERROR")){Serial.println("Retry MQTT OPEN");};
  }
  return response;
}

bool modem_qmt_wait() {
  // Send AT command
  bool response=0;
    monitorSerial1();
    if (Serial1.find("+QMTOPEN: 0,0")){
      response=1;
      break;
    } 
 
  return response;
}

bool modem_broker_stat() {
  // Send AT command
  bool response=0;
  for (int i = 0; i < 3; i++) {
    Serial1.println("AT+QMTOPEN?");
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < 500) {
      monitorSerial1();
      String command = Serial1.readStringUntil('\n');
      if (command.startsWith("+QMTOPEN: 0")) {
        response =1;
        break;
      }
  }}
  return response;
}

bool modem_conn_broker(const String& user, const String& password, const String& clientId) {
  // Send AT command
  bool response=0;

  String qmtconnCmd = "AT+QMTCONN=0,\"" + clientId + "\",\"" + user + "\",\"" + password + "\"";
  Serial1.println(qmtopenCmd);
  Serial.println(qmtopenCmd);
  previousMillis = millis();
  while (millis() - previousMillis < 1000) {
    monitorSerial1();
    if (Serial1.find("OK")){
      response=1;
      break;
    } 
    if (Serial1.find("ERROR")){Serial.println("Retry MQTT CONN");};
  }
  return response;
}

bool modem_conn_wait() {
  // Send AT command
  bool response=0;
    monitorSerial1();
    if (Serial1.find("+QMTCONN")){
      response=1;
      break;
    } 
 
  return response;
}


void modem_maintain(){
  //last_modem_response
  
  if(modem_status==0){
     if(modem_response_check())modem_status=1; 
  } 
  if(modem_status==1){
     if(modem_sim_check())modem_status=2; 
  } 
  if(modem_status==2){
     if(modem_function())modem_status=4; 
     else modem_status=3; 
  } 
  if(modem_status==3){
    if(modem_setfunction())modem_status=2;  
  }
  if(modem_status==4){
    if(modem_check_reg())modem_status=6;  
    else modem_status=5;  
  }
  if(modem_status==5){
    if(modem_setreg())modem_status=4;  
  }  
  if(modem_status==6){
    if(modem_check_cgatt())modem_status=9;  
    else modem_status=7;
  }
  if(modem_status==7){
    if(modem_set_apn(NETWORK_APN))modem_status=8;  
  }
  if(modem_status==8){
    if(modem_setcgatt())modem_status=6;  
  }
  if(modem_status==9){
    Serial.println("Network Successfully Activated"); 
    if(isConnected())modem_status=15; 
    else modem_status=10; 
  }
  if(modem_status==10){
    if(modem_set_broker(MQTT_SERVER,MQTT_PORT)){
      modem_status=11; 
      millis_qmt_request = millis();
    }  
  }
  if(modem_status==11){
    if(modem_qmt_wait())modem_status=12;
    if (millis() - millis_qmt_request > 60000) modem_status=6;
  }
  if(modem_status==12){
    if(modem_broker_stat())modem_status=13;
    else modem_status=6;
  }  
  
  if(modem_status==13){
    if(modem_conn_broker(MQTT_USER,MQTT_PW,MQTT_ID)){
      millis_qmt_request = millis();
      modem_status=14;
    }
    else modem_status=6;  
  }  

  if(modem_status==14){
    if(modem_conn_wait())modem_status=9;
    if (millis() - millis_qmt_request > 60000) modem_status=6;
  }
  if(modem_status==15){
    Serial.println("Connected to Broker Successfully"); 
  }

  

}



/********************************************************************************************************************HANDLE SAVE FROM WEB FUNCTION****************************************/
void setup() {
    Serial.begin(9600);
    Serial1.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    delay(1000);
    // Initialize EEPROM
    EEPROM.begin(512);
    delay(1000);
    Serial.println("*************************************************Setup Starting Setup!*****************************************************");

    // Reboot Modem
    rebootModem();

    // Register on the network with APN
    Serial.println("-----------------------Starting the Registration to Network with APN---------------------------------------");
    registerOnNetwork(NETWORK_APN);

    // Connect to MQTT Broker
    Serial.println("------------------------------Connecting to MQTT Broker----------------------------------------------------");
    connectToMQTTBroker(MQTT_SERVER, MQTT_USER, MQTT_PORT, MQTT_PW, MQTT_ID);

    // Subscribe to MQTT topics
    Serial.println("-----------------------------Subscribing to defined Topics-------------------------------------------------");
    subscribeToMQTTTopic(MQTT_TOPIC_1);
    subscribeToMQTTTopic(MQTT_TOPIC_2);
    subscribeToMQTTTopic(MQTT_TOPIC_3);

    // Call the function to publish device info at boot
    Serial.println("------------------------------------Publishing Device Info-------------------------------------------------");
    publishDeviceInfo(firmwareVersion, serialNumber);

    // Call the function to publish sensor values every 60 seconds
    Serial.println("-----------------------------------Publishing Sensor Values-------------------------------------------------");
    publishSensorValues(energy, current);

    //Get last saved values in the EEPROM
    Serial.println("-------------------------------------Get values From EEPROM-------------------------------------------------");
    GetLastSaveValues();
    WeekdayUpdate();//update Week days int values
    SpecialDayUpdate();//Update special days

    Serial.println("**********************************Setup Successfully executed!********************************************");
}

void loop() {











  // Check if it's time to send sensor data again
  if (millis() - lastSensorValSentTime >= SENSOR_SEND_INTERVAL) {
    // Update the last sensor value sent time
    lastSensorValSentTime = millis();
    publishSensorValues(energy, current); // Assuming ENERGY_VALUE and CURRENT_VALUE are variables holding the sensor values
  }
  
  // Check if it's time to check MQTT connection status
  if (millis() - lastMQTTCheckTime >= MQTT_CHECK_INTERVAL) {
    lastMQTTCheckTime = millis();
    if (!isConnected()) {
      // If not connected, register on the network and connect to MQTT broker
      registerOnNetwork(NETWORK_APN);
      connectToMQTTBroker(MQTT_SERVER, MQTT_USER, MQTT_PORT, MQTT_PW, MQTT_ID);
      subscribeToMQTTTopic(MQTT_TOPIC_1);
      subscribeToMQTTTopic(MQTT_TOPIC_2);
      subscribeToMQTTTopic(MQTT_TOPIC_3);
    }
  }
    // Monitor Serial1 transactions
    monitorSerial1();

    if(Ignore){
      //reset to ignore the sent loop    
      payload = "";
      mqttMessage = "";
  };
    // Check if the received message starts with "+QMTRECV"
    if (mqttMessage.startsWith("+QMTRECV")) {
    if(Ignore){
          payload = String("No payload!");
          mqttMessage = String("No message!");
  };
      // Print the received message
      Serial.println("--------------------------------------------------------------------------------------------------------------");
      Serial.print("Received Message:");
      Serial.print(mqttMessage);
      Serial.println("");
      Serial.println("--------------------------------------------------------------------------------------------------------------");

      // Extract the topic name
      int topicStart = 15; // Find the index of escaped double quotes and add 2 to skip them
      int topicEnd = mqttMessage.indexOf("{", topicStart) - 2;
      String topic = mqttMessage.substring(topicStart, topicEnd);
      Serial.println("--------------------------------------------------------------------------------------------------------------");
      Serial.print("Topic:");
      Serial.print(topic);
      Serial.println("");
      // Extract the payload
      int payloadStart = mqttMessage.indexOf("{", topicEnd);
      payload = mqttMessage.substring(payloadStart);
      Serial.print("Payload:");
      Serial.print(payload);
      Serial.println("");
      Serial.println("--------------------------------------------------------------------------------------------------------------");

      // Check if the received topic matches MQTT_TOPIC_1 or MQTT_TOPIC_2
      if (topic == MQTT_TOPIC_1 || topic == MQTT_TOPIC_2 || topic == MQTT_TOPIC_3) {
        // Call the function to handle the message for the corresponding topic
        if (topic == MQTT_TOPIC_1) {
          Serial.println("MQTT_TOPIC_1"); 
          handleMQTTTopic1(payload);
          payload = String("No payload!");
          mqttMessage = String("No message!");

        } else if (topic == MQTT_TOPIC_2) {
          Serial.println("MQTT_TOPIC_2");
          handleMQTTTopic2(payload);
          payload = String("No payload!");
          mqttMessage = String("No message!");
        } else if (topic == MQTT_TOPIC_3) {
          Serial.println("MQTT_TOPIC_3");
          handleMQTTTopic3(payload);
          payload = String("No payload!");
          mqttMessage = String("No message!");
        }
      }
    }
  ;
    if(Ignore){ 
      //reset to ignore the sent loop    
        payload = "";
        mqttMessage = "";
        Ignore = false;
    };
}

/********************************************************************************************************************WRITE STRING FUNCTION****************************************/
void writeString(int startingAddress, String data) {
  // Write each character of the string to EEPROM
  for (unsigned int i = 0; i < data.length(); i++) {
    EEPROM.write(startingAddress + i, data[i]);
  }
  // Write null character at the end to mark the end of the string
  EEPROM.write(startingAddress + data.length(), '\0');
  EEPROM.commit(); // Committing changes to EEPROM
}
/********************************************************************************************************************READ STRING FUNCTION****************************************/
String readString(int startingAddress, int length) {
  // Read characters from EEPROM until requested length is reached or null character is encountered
  String readData = "";
  char ch = '\0';
  for (unsigned int i = 0; i < length; i++) {
    ch = EEPROM.read(startingAddress + i);
    if (ch != '\0') {
      readData += ch;
    } else {
      break; // Stop reading if null character is encountered
    }
  }
  return readData;
}
/********************************************************************************************************************SET TO DEFAULE PARAMETERS FUNCTION****************************************/
void StoreVarIEEPROM(){
    WeekdayUpdate();//update Week days int values
    SpecialDayUpdate();//Update special days

    //Store values as int in eeprom
    // Monday
    EEPROM.write(ADDR_MONDAY_ON_TIME, monday.on_time.hour);
    EEPROM.write(ADDR_MONDAY_ON_TIME + 1, monday.on_time.minute);
    EEPROM.write(ADDR_MONDAY_OFF_TIME, monday.off_time.hour);
    EEPROM.write(ADDR_MONDAY_OFF_TIME + 1, monday.off_time.minute);
    
    // Tuesday
    EEPROM.write(ADDR_TUESDAY_ON_TIME, tuesday.on_time.hour);
    EEPROM.write(ADDR_TUESDAY_ON_TIME + 1, tuesday.on_time.minute);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME, tuesday.off_time.hour);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME + 1, tuesday.off_time.minute);
    
    // Wednesday
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME, wednesday.on_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME + 1, wednesday.on_time.minute);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME, wednesday.off_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME + 1, wednesday.off_time.minute);
    
    // Thursday
    EEPROM.write(ADDR_THURSDAY_ON_TIME, thursday.on_time.hour);
    EEPROM.write(ADDR_THURSDAY_ON_TIME + 1, thursday.on_time.minute);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME, thursday.off_time.hour);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME + 1, thursday.off_time.minute);
    
    // Friday
    EEPROM.write(ADDR_FRIDAY_ON_TIME, friday.on_time.hour);
    EEPROM.write(ADDR_FRIDAY_ON_TIME + 1, friday.on_time.minute);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME, friday.off_time.hour);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME + 1, friday.off_time.minute);
    
    // Saturday
    EEPROM.write(ADDR_SATURDAY_ON_TIME, saturday.on_time.hour);
    EEPROM.write(ADDR_SATURDAY_ON_TIME + 1, saturday.on_time.minute);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME, saturday.off_time.hour);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME + 1, saturday.off_time.minute);
    
    // Sunday
    EEPROM.write(ADDR_SUNDAY_ON_TIME, sunday.on_time.hour);
    EEPROM.write(ADDR_SUNDAY_ON_TIME + 1, sunday.on_time.minute);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME, sunday.off_time.hour);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME + 1, sunday.off_time.minute);

    // Special Day1
    EEPROM.write(ADDR_D1_ON_TIME, specialDates[0].on_time.hour);
    EEPROM.write(ADDR_D1_ON_TIME + 1, specialDates[0].on_time.minute);
    EEPROM.write(ADDR_D1_OFF_TIME, specialDates[0].off_time.hour);
    EEPROM.write(ADDR_D1_OFF_TIME + 1, specialDates[0].off_time.minute);

    // Special Day2
    EEPROM.write(ADDR_D2_ON_TIME, specialDates[1].on_time.hour);
    EEPROM.write(ADDR_D2_ON_TIME + 1, specialDates[1].on_time.minute);
    EEPROM.write(ADDR_D2_OFF_TIME, specialDates[1].off_time.hour);
    EEPROM.write(ADDR_D2_OFF_TIME + 1, specialDates[1].off_time.minute);

    // Special Day3
    EEPROM.write(ADDR_D3_ON_TIME, specialDates[2].on_time.hour);
    EEPROM.write(ADDR_D3_ON_TIME + 1, specialDates[2].on_time.minute);
    EEPROM.write(ADDR_D3_OFF_TIME, specialDates[2].off_time.hour);
    EEPROM.write(ADDR_D3_OFF_TIME + 1, specialDates[2].off_time.minute);

    // Special Day4
    EEPROM.write(ADDR_D4_ON_TIME, specialDates[3].on_time.hour);
    EEPROM.write(ADDR_D4_ON_TIME + 1, specialDates[3].on_time.minute);
    EEPROM.write(ADDR_D4_OFF_TIME, specialDates[3].off_time.hour);
    EEPROM.write(ADDR_D4_OFF_TIME + 1, specialDates[3].off_time.minute);

    // Special Day5 
    EEPROM.write(ADDR_D5_ON_TIME, specialDates[4].on_time.hour);
    EEPROM.write(ADDR_D5_ON_TIME + 1, specialDates[4].on_time.minute);
    EEPROM.write(ADDR_D5_OFF_TIME, specialDates[4].off_time.hour);
    EEPROM.write(ADDR_D5_OFF_TIME + 1, specialDates[4].off_time.minute);

    writeString(ADDR_DAY1, DAY1);
    writeString(ADDR_DAY2, DAY2);
    writeString(ADDR_DAY3, DAY3);
    writeString(ADDR_DAY4, DAY4);
    writeString(ADDR_DAY5, DAY5);

    EEPROM.commit(); // Committing changes to EEPROM 
}

/* 
{"method":"reboot"}

Request
{"AC": 1}
{"AC": 2}
{"AC": 3}
{"AC": 4}
{"AC": 5}
{"AC": 6}
{"AC": 7}
{"AC": 8}
{"AC": 9}
{"AC": 10}
{"AC": 11}
{"AC": 12}

 configurations commands

{"AC": 1, "MOON": "14:23", "MOOF.": "18:51"}
{"AC": 2, "TUON": "10:37", "TUOF.": "14:45"}
{"AC": 3, "WEON": "09:12", "WEOF.": "12:30"}
{"AC": 4, "THON": "12:45", "THOF.": "21:04"}
{"AC": 5, "FRON": "08:29", "FROF.": "18:12"}
{"AC": 6, "SAON": "13:50", "SAOF.": "19:57"}
{"AC": 7, "SUON": "11:02", "SUOF.": "15:48"}
{"AC": 8, "D1ON": "18:24", "D1OF.":  "04:28","DAY": "20-12-2024"}
{"AC": 9, "D2ON": "10:17", "D2OF.": "16:29", "DAY": "12-03-2018"}
{"AC": 10, "D3ON": "15:42", "D3OF.": "22:19", "DAY": "28-07-2022"}
{"AC": 11, "D4ON": "08:51", "D4OF.": "19:38", "DAY": "09-09-2020"}
{"AC": 12, "D5ON": "11:28", "D5OF.": "17:09", "DAY": "15-05-2024"}
*/


/*1 771 569 662
2:42 PM
Pw: posadm1n
2:42 PM*/
