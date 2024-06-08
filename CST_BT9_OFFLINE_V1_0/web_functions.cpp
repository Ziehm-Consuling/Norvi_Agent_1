#include "web_functions.h"
#include "eeprom_functions.h"
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>  // Include EEPROM library

WebServer server(80);

extern String UI_DATE;
extern String UI_TIME;
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

extern Schedule monday, tuesday, wednesday, thursday, friday, saturday, sunday;
extern Schedule specialDates[5];


//RTClib DS3231;
RTC_DS3231 rtc;

extern String currentSchedule;
extern bool alarm_updated;



void handleRoot() {
    // if (!server.authenticate(http_username, http_password)) {
    //     return server.requestAuthentication();
    // }

    DateTime now = rtc.now();
    char timeString[6];
    sprintf(timeString, "%02d:%02d", 3, 18);
    String UI_TIME(timeString);

    char dateString[11];
    sprintf(dateString, "%02d-%02d-%04d", 16, 7, 2024);
    String UI_DATE(dateString);

    String html = "<html><head><title>ESP32 Web Server</title><style>body { font-family: Arial, sans-serif; width: 100%; height: 100%; display: flex; justify-content: center; align-items: center; }";
    html += ".container { width: 648px; height: 728px; }";
    html += ".button { background-color: black; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: block; font-size: 16px; margin: 4px auto 10px; cursor: pointer; border-radius: 5px; }";
    html += ".status { font-family: Arial, sans-serif; font-size: 16px; }";
    html += ".value-box { background-color: lightorange; padding: 5px; border-radius: 5px; font-size: 16px; }";
    html += ".label-value-container { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }";
    html += ".seven h1 { text-align: center; font-size:30px; font-weight:300; color:#222; letter-spacing:1px; text-transform: uppercase; display: grid; grid-template-columns: 1fr max-content 1fr; grid-template-rows: 27px 0; grid-gap: 20px; align-items: center; }";
    html += ".seven h1:after, .seven h1:before { content: ' '; display: block; border-bottom: 1px solid #c50000; border-top: 1px solid #c50000; height: 5px; background-color:#f8f8f8; }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<div class='seven'><h1>Scheduler Configuration Panel</h1></div>";

    html += "<div class='label-value-container'>";
    html += "<h2>Date:</h2><div class='value-box'>";
    html += UI_DATE;
    html += "</div></div>";
    html += "<div class='label-value-container'>";
    html += "<h2>Time:</h2><div class='value-box'>";
    html += UI_TIME;
    html += "</div></div>";
    html += "<div class='label-value-container'>";
    html += "<h2>Network:</h2><div class='value-box'>";
    html += UI_NETWORK;
    html += "</div></div>";
    html += "<div class='label-value-container'>";
    html += "<h2>Signal:</h2><div class='value-box'>";
    html += String(UI_SIGNAL) + "%";
    html += "</div></div>";
    html += "<div class='label-value-container'>";
    html += "<h2>Status:</h2><div class='value-box' style='color: ";
    html += UI_STATUS_CELL == "CONNECTED" ? "green" : "red";
    html += ";'>";
    html += UI_STATUS_CELL;
    html += "</div></div>";
    html += "<div class='label-value-container'>";
    html += "<h2>MQTT Status:</h2><div class='value-box' style='color: ";
    html += UI_STATUS_MQTT == "CONNECTED" ? "green" : "red";
    html += ";'>";
    html += UI_STATUS_MQTT;
    html += "</div></div>";
    html += "<a href='/schedule' class='button'>Configure Schedule</a>";
    html += "<a href='/config' class='button'>Configure Network</a>";
    html += "</div>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleConfigNetwork() {
    // if (!server.authenticate(http_username, http_password)) {
    //     return server.requestAuthentication();
    // }

    UI_NETWORK = readString(ADDR_UI_NETWORK, ADDR_UI_SIGNAL - ADDR_UI_NETWORK);
    UI_SIGNAL = readString(ADDR_UI_SIGNAL, ADDR_UI_STATUS_MQTT - ADDR_UI_SIGNAL);
    UI_STATUS_MQTT = readString(ADDR_UI_STATUS_MQTT, ADDR_NETWORK_APN - ADDR_UI_STATUS_MQTT);
    NETWORK_APN = readString(ADDR_NETWORK_APN, ADDR_MQTT_SERVER - ADDR_NETWORK_APN);
    MQTT_SERVER = readString(ADDR_MQTT_SERVER, ADDR_MQTT_USER - ADDR_MQTT_SERVER);
    MQTT_USER = readString(ADDR_MQTT_USER, ADDR_MQTT_PW - ADDR_MQTT_USER);
    MQTT_PW = readString(ADDR_MQTT_PW, ADDR_MQTT_DN - ADDR_MQTT_PW);
    MQTT_DN = readString(ADDR_MQTT_DN, ADDR_MQTT_ID - ADDR_MQTT_DN);
    MQTT_ID = readString(ADDR_MQTT_ID, ADDR_MQTT_SD - ADDR_MQTT_ID);
    MQTT_SD = readString(ADDR_MQTT_SD, ADDR_MONDAY_ON_TIME - ADDR_MQTT_SD);

    String html = "<html><head><title>ESP32 Web Server</title><style>body { font-family: Arial, sans-serif; width: 100%; height: 100%; display: flex; justify-content: center; align-items: center; }";
    html += ".container { width: 648px; height: 728px; margin: 0 auto; }";
    html += ".button { background-color: black; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 5px; }";
    html += ".status { font-family: Arial, sans-serif; font-size: 16px; }";
    html += ".value-box { background-color: lightorange; padding: 5px; border-radius: 5px; font-size: 16px; }";
    html += ".title-button-container { display: flex; justify-content: space-between; align-items: center; }";
    html += "input[type='text'], input[type='password'] { width: 200px; height: 30px; font-size: 16px; }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<div class='title-button-container'><h1>Scheduler Configuration Page</h1><button class='button' onclick='goBack()'>Back</button></div>";
    html += "<div class='title-button-container'><h1>Network Configuration</h1><button class='button' onclick='saveConfig()'>Save</button></div><br><br>";
    html += "<form method='post' action='/send'>";
    html += "<label style='float:left; width: 400px;'>APN</label><input type='text' name='APN' value='" + NETWORK_APN + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>MQTT Server</label><input type='text' name='MQTT Server' value='" + MQTT_SERVER + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>User Name</label><input type='text' name='User Name' value='" + MQTT_USER + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>Password</label><input type='password' name='Password' value='" + MQTT_PW + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>Device Name</label><input type='text' name='Device Name' value='" + MQTT_DN + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>Device ID</label><input type='text' name='Device ID' value='" + MQTT_ID + "' style='float:right;'><br><br>";
    html += "<label style='float:left; width: 400px;'>Secret Key</label><input type='text' name='Secret Key' value='" + MQTT_SD + "' style='float:right;'><br><br>";
    html += "</form>";
    html += "<script>function saveConfig() { document.forms[0].submit(); }</script>";
    html += "<script>function goBack() { window.location='/' }</script>";
    html += "</div>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleSend() {
    NETWORK_APN = server.arg("APN");
    MQTT_SERVER = server.arg("MQTT Server");
    MQTT_USER = server.arg("User Name");
    MQTT_PW = server.arg("Password");
    MQTT_DN = server.arg("Device Name");
    MQTT_ID = server.arg("Device ID");
    MQTT_SD = server.arg("Secret Key");

    writeString(ADDR_NETWORK_APN, NETWORK_APN);
    writeString(ADDR_MQTT_SERVER, MQTT_SERVER);
    writeString(ADDR_MQTT_USER, MQTT_USER);
    writeString(ADDR_MQTT_PW, MQTT_PW);
    writeString(ADDR_MQTT_DN, MQTT_DN);
    writeString(ADDR_MQTT_ID, MQTT_ID);
    writeString(ADDR_MQTT_SD, MQTT_SD);

    server.send(200, "text/html", "<script>window.location='/';</script>");
}

void handleScheduleConfig() {
    // if (!server.authenticate(http_username, http_password)) {
    //     return server.requestAuthentication();
    // }

    GetLastSaveValues();
    WeekdayUpdate();
    SpecialDayUpdate();

    String html = "<html><head><title>ESP32 Web Server</title><style>body { font-family: Arial, sans-serif; width: 648px; height: 728px; margin: 0 auto; }";
    html += ".button { background-color: black; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 5px; }";
    html += ".status { font-family: Arial, sans-serif; font-size: 16px; }";
    html += ".value-box { background-color: lightorange; padding: 5px; border-radius: 5px; font-size: 16px; }";
    html += ".row { display: flex; align-items: center; }";
    html += ".time-input { margin-right: 10px; height: 30px; }";
    html += ".title-button-container { display: flex; justify-content: space-between; align-items: center; }";
    html += "</style>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body>";
    html += "<div class='title-button-container'><h1>Scheduler Page</h1><button class='button' onclick='goBack()'>Back</button></div>";
    html += "<div class='title-button-container'><h1>Configuration </h1><button class='button' onclick='saveConfig()'>Save</button></div><br><br>";
    html += "<form method='post' action='/save'>";
    html += "<hr>";
    html += "<h1 style='font-size: 20px;'>Standard Schedule</h1>";
    html += "<hr>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><label style='float:left; width: 300px;'>ON TIME</label><label style='float:left; width: 300px;'>OFF TIME</label></div>";
    html += "<hr>";
    html += "<div class='row'><label style='float:left; width: 150px;'>MONDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='monday_on_time' value='" + MONDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='monday_off_time' value='" + MONDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>TUESDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='tuesday_on_time' value='" + TUESDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='tuesday_off_time' value='" + TUESDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>WEDNESDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='wednesday_on_time' value='" + WEDNESDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='wednesday_off_time' value='" + WEDNESDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>THURSDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='thursday_on_time' value='" + THURSDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='thursday_off_time' value='" + THURSDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>FRIDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='friday_on_time' value='" + FRIDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='friday_off_time' value='" + FRIDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>SATURDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='saturday_on_time' value='" + SATURDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='saturday_off_time' value='" + SATURDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>SUNDAY</label><label style='float:left; width: 150px;'></label><label style='float:left; width: 150px;'></label><input type='text' name='sunday_on_time' value='" + SUNDAY_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='sunday_off_time' value='" + SUNDAY_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<hr>";
    html += "<hr>";
    html += "<h1 style='font-size: 20px;'>Special Schedule</h1>";
    html += "<hr>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY 01</label><input type='text' name='da1' value='" + DAY1 + "' class='time-input' onblur='validateSpecialDateInput(this)'><input type='text' name='day1_on_time' value='" + D1_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='day1_off_time' value='" + D1_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY 02</label><input type='text' name='da2' value='" + DAY2 + "' class='time-input' onblur='validateSpecialDateInput(this)'><input type='text' name='day2_on_time' value='" + D2_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='day2_off_time' value='" + D2_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY 03</label><input type='text' name='da3' value='" + DAY3 + "' class='time-input' onblur='validateSpecialDateInput(this)'><input type='text' name='day3_on_time' value='" + D3_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='day3_off_time' value='" + D3_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY 04</label><input type='text' name='da4' value='" + DAY4 + "' class='time-input' onblur='validateSpecialDateInput(this)'><input type='text' name='day4_on_time' value='" + D4_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='day4_off_time' value='" + D4_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<div class='row'><label style='float:left; width: 150px;'>DAY 05</label><input type='text' name='da5' value='" + DAY5 + "' class='time-input' onblur='validateSpecialDateInput(this)'><input type='text' name='day5_on_time' value='" + D5_ON_TIME + "' class='time-input' onblur='validateTimeInput(this)'><input type='text' name='day5_off_time' value='" + D5_OFF_TIME + "' class='time-input' onblur='validateTimeInput(this)'></div><br>";
    html += "<hr>";
    html += "</form>";
    html += "<script>function saveConfig() { document.forms[0].submit(); }</script>";
    html += "<script>function goBack() { window.location='/'; }</script>";
    html += "<script>";
    html += "function validateTimeInput(input) {";
    html += "  input.value = input.value.replace(/[^0-9:]/g, '');";
    html += "  if (input.value.length > 5) input.value = input.value.slice(0, 5);";
    html += "  var timeRegex = /^([01]?[0-9]|2[0-3]):[0-5][0-9]$/;";
    html += "  if (!timeRegex.test(input.value)) input.value = '00:00';";
    html += "}";
    html += "function validateSpecialDateInput(input) {";
    html += "  input.value = input.value.replace(/[^0-9-]/g, '');";
    html += "  if (input.value.length != 10) input.value = '12-12-2024';";
    html += "}";
    html += "</script>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

void handleSave() {
    MONDAY_ON_TIME = server.arg("monday_on_time");
    MONDAY_OFF_TIME = server.arg("monday_off_time");
    TUESDAY_ON_TIME = server.arg("tuesday_on_time");
    TUESDAY_OFF_TIME = server.arg("tuesday_off_time");
    WEDNESDAY_ON_TIME = server.arg("wednesday_on_time");
    WEDNESDAY_OFF_TIME = server.arg("wednesday_off_time");
    THURSDAY_ON_TIME = server.arg("thursday_on_time");
    THURSDAY_OFF_TIME = server.arg("thursday_off_time");
    FRIDAY_ON_TIME = server.arg("friday_on_time");
    FRIDAY_OFF_TIME = server.arg("friday_off_time");
    SATURDAY_ON_TIME = server.arg("saturday_on_time");
    SATURDAY_OFF_TIME = server.arg("saturday_off_time");
    SUNDAY_ON_TIME = server.arg("sunday_on_time");
    SUNDAY_OFF_TIME = server.arg("sunday_off_time");

    D1_ON_TIME = server.arg("day1_on_time");
    D1_OFF_TIME = server.arg("day1_off_time");
    D2_ON_TIME = server.arg("day2_on_time");
    D2_OFF_TIME = server.arg("day2_off_time");
    D3_ON_TIME = server.arg("day3_on_time");
    D3_OFF_TIME = server.arg("day3_off_time");
    D4_ON_TIME = server.arg("day4_on_time");
    D4_OFF_TIME = server.arg("day4_off_time");
    D5_ON_TIME = server.arg("day5_on_time");
    D5_OFF_TIME = server.arg("day5_off_time");

    DAY1 = server.arg("da1");
    DAY2 = server.arg("da2");
    DAY3 = server.arg("da3");
    DAY4 = server.arg("da4");
    DAY5 = server.arg("da5");

    WeekdayUpdate();
    SpecialDayUpdate();

    EEPROM.write(ADDR_MONDAY_ON_TIME, monday.on_time.hour);
    EEPROM.write(ADDR_MONDAY_ON_TIME + 1, monday.on_time.minute);
    EEPROM.write(ADDR_MONDAY_OFF_TIME, monday.off_time.hour);
    EEPROM.write(ADDR_MONDAY_OFF_TIME + 1, monday.off_time.minute);
    EEPROM.write(ADDR_TUESDAY_ON_TIME, tuesday.on_time.hour);
    EEPROM.write(ADDR_TUESDAY_ON_TIME + 1, tuesday.on_time.minute);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME, tuesday.off_time.hour);
    EEPROM.write(ADDR_TUESDAY_OFF_TIME + 1, tuesday.off_time.minute);
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME, wednesday.on_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_ON_TIME + 1, wednesday.on_time.minute);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME, wednesday.off_time.hour);
    EEPROM.write(ADDR_WEDNESDAY_OFF_TIME + 1, wednesday.off_time.minute);
    EEPROM.write(ADDR_THURSDAY_ON_TIME, thursday.on_time.hour);
    EEPROM.write(ADDR_THURSDAY_ON_TIME + 1, thursday.on_time.minute);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME, thursday.off_time.hour);
    EEPROM.write(ADDR_THURSDAY_OFF_TIME + 1, thursday.off_time.minute);
    EEPROM.write(ADDR_FRIDAY_ON_TIME, friday.on_time.hour);
    EEPROM.write(ADDR_FRIDAY_ON_TIME + 1, friday.on_time.minute);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME, friday.off_time.hour);
    EEPROM.write(ADDR_FRIDAY_OFF_TIME + 1, friday.off_time.minute);
    EEPROM.write(ADDR_SATURDAY_ON_TIME, saturday.on_time.hour);
    EEPROM.write(ADDR_SATURDAY_ON_TIME + 1, saturday.on_time.minute);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME, saturday.off_time.hour);
    EEPROM.write(ADDR_SATURDAY_OFF_TIME + 1, saturday.off_time.minute);
    EEPROM.write(ADDR_SUNDAY_ON_TIME, sunday.on_time.hour);
    EEPROM.write(ADDR_SUNDAY_ON_TIME + 1, sunday.on_time.minute);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME, sunday.off_time.hour);
    EEPROM.write(ADDR_SUNDAY_OFF_TIME + 1, sunday.off_time.minute);

    EEPROM.write(ADDR_D1_ON_TIME, specialDates[0].on_time.hour);
    EEPROM.write(ADDR_D1_ON_TIME + 1, specialDates[0].on_time.minute);
    EEPROM.write(ADDR_D1_OFF_TIME, specialDates[0].off_time.hour);
    EEPROM.write(ADDR_D1_OFF_TIME + 1, specialDates[0].off_time.minute);
    EEPROM.write(ADDR_D2_ON_TIME, specialDates[1].on_time.hour);
    EEPROM.write(ADDR_D2_ON_TIME + 1, specialDates[1].on_time.minute);
    EEPROM.write(ADDR_D2_OFF_TIME, specialDates[1].off_time.hour);
    EEPROM.write(ADDR_D2_OFF_TIME + 1, specialDates[1].off_time.minute);
    EEPROM.write(ADDR_D3_ON_TIME, specialDates[2].on_time.hour);
    EEPROM.write(ADDR_D3_ON_TIME + 1, specialDates[2].on_time.minute);
    EEPROM.write(ADDR_D3_OFF_TIME, specialDates[2].off_time.hour);
    EEPROM.write(ADDR_D3_OFF_TIME + 1, specialDates[2].off_time.minute);
    EEPROM.write(ADDR_D4_ON_TIME, specialDates[3].on_time.hour);
    EEPROM.write(ADDR_D4_ON_TIME + 1, specialDates[3].on_time.minute);
    EEPROM.write(ADDR_D4_OFF_TIME, specialDates[3].off_time.hour);
    EEPROM.write(ADDR_D4_OFF_TIME + 1, specialDates[3].off_time.minute);
    EEPROM.write(ADDR_D5_ON_TIME, specialDates[4].on_time.hour);
    EEPROM.write(ADDR_D5_ON_TIME + 1, specialDates[4].on_time.minute);
    EEPROM.write(ADDR_D5_OFF_TIME, specialDates[4].off_time.hour);
    EEPROM.write(ADDR_D5_OFF_TIME + 1, specialDates[4].off_time.minute);

    writeString(ADDR_DAY1, DAY1);
    writeString(ADDR_DAY2, DAY2);
    writeString(ADDR_DAY3, DAY3);
    writeString(ADDR_DAY4, DAY4);
    writeString(ADDR_DAY5, DAY5);

    EEPROM.commit();

    server.send(200, "text/html", "<script>window.location='/';</script>");
    alarm_updated=false;
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);

    
}

void setupWebServer() {
    
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        // This line sets the RTC with an explicit date & time, e.g., to set
        // January 21, 2014 at 3am you would call:
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_GET, handleConfigNetwork);
    server.on("/send", HTTP_POST, handleSend);
    server.on("/schedule", HTTP_GET, handleScheduleConfig);
    server.on("/save", HTTP_POST, handleSave);

    server.begin();
}

void checkAndSetAlarms() {
    DateTime now = rtc.now();
    if(alarm_updated==false){
      currentSchedule = "";
    for (int i = 0; i < 5; ++i) {
        if (specialDates[i].day.day == now.day() && specialDates[i].day.month == now.month() && specialDates[i].day.year == now.year()) {
            rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), specialDates[i].on_time.hour, specialDates[i].on_time.minute, 0), DS3231_A1_Hour);
            rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), specialDates[i].off_time.hour, specialDates[i].off_time.minute, 0), DS3231_A2_Hour);
            currentSchedule = "Special Date:"+ String(i) + "On Time: " + String(specialDates[i].on_time.hour) + ":" + String(specialDates[i].on_time.minute) + ", Off Time: " + String(specialDates[i].off_time.hour) + ":" + String(specialDates[i].off_time.minute);
            break;
        }
    }

    if (currentSchedule == "") {
        switch (now.dayOfTheWeek()) {
            case 1: 
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), monday.on_time.hour, monday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), monday.off_time.hour, monday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Monday: On Time: " + String(monday.on_time.hour) + ":" + String(monday.on_time.minute) + ", Off Time: " + String(monday.off_time.hour) + ":" + String(monday.off_time.minute);
                break;
            case 2:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), tuesday.on_time.hour, tuesday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), tuesday.off_time.hour, tuesday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Tuesday: On Time: " + String(tuesday.on_time.hour) + ":" + String(tuesday.on_time.minute) + ", Off Time: " + String(tuesday.off_time.hour) + ":" + String(tuesday.off_time.minute);
                break;
            case 3:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), wednesday.on_time.hour, wednesday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), wednesday.off_time.hour, wednesday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Wednesday: On Time: " + String(wednesday.on_time.hour) + ":" + String(wednesday.on_time.minute) + ", Off Time: " + String(wednesday.off_time.hour) + ":" + String(wednesday.off_time.minute);
                break;
            case 4:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), thursday.on_time.hour, thursday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), thursday.off_time.hour, thursday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Thursday: On Time: " + String(thursday.on_time.hour) + ":" + String(thursday.on_time.minute) + ", Off Time: " + String(thursday.off_time.hour) + ":" + String(thursday.off_time.minute);
                break;
            case 5:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), friday.on_time.hour, friday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), friday.off_time.hour, friday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Friday: On Time: " + String(friday.on_time.hour) + ":" + String(friday.on_time.minute) + ", Off Time: " + String(friday.off_time.hour) + ":" + String(friday.off_time.minute);
                break;
            case 6:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), saturday.on_time.hour, saturday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), saturday.off_time.hour, saturday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Saturday: On Time: " + String(saturday.on_time.hour) + ":" + String(saturday.on_time.minute) + ", Off Time: " + String(saturday.off_time.hour) + ":" + String(saturday.off_time.minute);
                break;
            case 0:
                rtc.setAlarm1(DateTime(now.year(), now.month(), now.day(), sunday.on_time.hour, sunday.on_time.minute, 0), DS3231_A1_Day);
                rtc.setAlarm2(DateTime(now.year(), now.month(), now.day(), sunday.off_time.hour, sunday.off_time.minute, 0), DS3231_A2_Day);
                currentSchedule = "Sunday: On Time: " + String(sunday.on_time.hour) + ":" + String(sunday.on_time.minute) + ", Off Time: " + String(sunday.off_time.hour) + ":" + String(sunday.off_time.minute);
                break;
        }
    }
    alarm_updated=true;
}
    Serial.print("Date: ");
    Serial.print(now.day());
    Serial.print("-");
    Serial.print(now.month());
    Serial.print("-");
    Serial.print(now.year());

    Serial.print(" ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.print(now.second());
    Serial.print(" - ");
    Serial.print(currentSchedule);
    Serial.print(" OUTPUT ");
    Serial.println(power_output);
    

}

void executeScheduledTasks() {
    DateTime now = rtc.now();

    int currentMinutes = now.hour() * 60 + now.minute();
    bool isSpecialDate = false;

    for (int i = 0; i < 5; ++i) {
        if (specialDates[i].day.day == now.day() && specialDates[i].day.month == now.month() && specialDates[i].day.year == now.year()) {
            int onMinutes = specialDates[i].on_time.hour * 60 + specialDates[i].on_time.minute;
            int offMinutes = specialDates[i].off_time.hour * 60 + specialDates[i].off_time.minute;

            if (currentMinutes >= onMinutes && currentMinutes < offMinutes) {
                function1();
            } else if (currentMinutes >= offMinutes) {
                function2();
            }
            isSpecialDate = true;
            break;
        }
    }

    if (!isSpecialDate) {
        Schedule* todaySchedule;
        switch (now.dayOfTheWeek()) {
            case 1: todaySchedule = &monday; break;
            case 2: todaySchedule = &tuesday; break;
            case 3: todaySchedule = &wednesday; break;
            case 4: todaySchedule = &thursday; break;
            case 5: todaySchedule = &friday; break;
            case 6: todaySchedule = &saturday; break;
            case 0: todaySchedule = &sunday; break;
            default: return; // Invalid day
        }

        int onMinutes = todaySchedule->on_time.hour * 60 + todaySchedule->on_time.minute;
        int offMinutes = todaySchedule->off_time.hour * 60 + todaySchedule->off_time.minute;

        if (currentMinutes >= onMinutes && currentMinutes < offMinutes) {
            function1();
        } else if (currentMinutes >= offMinutes) {
            function2();
        }
    }
}




void handleAlarm1() {
    Serial.println("Alarm 1 triggered!");
    // Add your code to handle Alarm 1 here
    //checkAndSetAlarms();
}

void handleAlarm2() {
    Serial.println("Alarm 2 triggered!");
    // Add your code to handle Alarm 2 here
    //checkAndSetAlarms();
}

void excecute_alarm(){
if (rtc.alarmFired(1)) {
        handleAlarm1();
        rtc.clearAlarm(1);
    }
    if (rtc.alarmFired(2)) {
        handleAlarm2();
        rtc.clearAlarm(2);
    }


}

void function1() {
    //Serial.println("Function 1 is called");
    power_output=true;
}

void function2() {
    //Serial.println("Function 2 is called");
    power_output=false;
}


