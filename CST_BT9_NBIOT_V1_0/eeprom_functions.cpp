#include "eeprom_functions.h"

#include <EEPROM.h>

// Write a string to EEPROM
void writeString(int startingAddress, String data) {
    for (unsigned int i = 0; i < data.length(); i++) {
        EEPROM.write(startingAddress + i, data[i]);
    }
    EEPROM.write(startingAddress + data.length(), '\0');
    EEPROM.commit();
}

// Read a string from EEPROM
String readString(int startingAddress, int length) {
    String readData = "";
    char ch = '\0';
    for (unsigned int i = 0; i < length; i++) {
        ch = EEPROM.read(startingAddress + i);
        if (ch != '\0') {
            readData += ch;
        } else {
            break;
        }
    }
    return readData;
}

// Reset parameters to default values
void ResetParam() {
    writeString(ADDR_UI_NETWORK, "CTEL");
    writeString(ADDR_UI_SIGNAL, "100%");
    writeString(ADDR_UI_STATUS_MQTT, "DISCONNECTED");
    writeString(ADDR_NETWORK_APN, "dalta.bb");
    writeString(ADDR_MQTT_SERVER, "delta.bb");
    writeString(ADDR_MQTT_USER, "user1");
    writeString(ADDR_MQTT_PW, "delta.bb");
    writeString(ADDR_MQTT_DN, "adl_connect");
    writeString(ADDR_MQTT_ID, "ade52146kve511");
    writeString(ADDR_MQTT_SD, "551965641856");
    writeString(ADDR_MONDAY_ON_TIME, "08:00");
    writeString(ADDR_MONDAY_OFF_TIME, "17:00");
    writeString(ADDR_TUESDAY_ON_TIME, "09:00");
    writeString(ADDR_TUESDAY_OFF_TIME, "18:00");
    writeString(ADDR_WEDNESDAY_ON_TIME, "10:00");
    writeString(ADDR_WEDNESDAY_OFF_TIME, "19:00");
    writeString(ADDR_THURSDAY_ON_TIME, "11:00");
    writeString(ADDR_THURSDAY_OFF_TIME, "20:00");
    writeString(ADDR_FRIDAY_ON_TIME, "12:00");
    writeString(ADDR_FRIDAY_OFF_TIME, "21:00");
    writeString(ADDR_SATURDAY_ON_TIME, "13:00");
    writeString(ADDR_SATURDAY_OFF_TIME, "22:00");
    writeString(ADDR_SUNDAY_ON_TIME, "14:00");
    writeString(ADDR_SUNDAY_OFF_TIME, "23:00");
    writeString(ADDR_D1_ON_TIME, "08:30");
    writeString(ADDR_D1_OFF_TIME, "16:30");
    writeString(ADDR_D2_ON_TIME, "09:30");
    writeString(ADDR_D2_OFF_TIME, "17:30");
    writeString(ADDR_D3_ON_TIME, "10:30");
    writeString(ADDR_D3_OFF_TIME, "18:30");
    writeString(ADDR_D4_ON_TIME, "11:30");
    writeString(ADDR_D4_OFF_TIME, "19:30");
    writeString(ADDR_D5_ON_TIME, "12:30");
    writeString(ADDR_D5_OFF_TIME, "20:30");
    writeString(ADDR_DAY1, "12-07-2024");
    writeString(ADDR_DAY2, "16-07-1997");
    writeString(ADDR_DAY3, "12-07-2024");
}

// Check if EEPROM needs reset
void EEPROM_RESET_CHECK() {
    int variable_1 = EEPROM.read(ADDR_MONDAY_ON_TIME);
    int variable_2 = EEPROM.read(ADDR_MONDAY_ON_TIME + 1);
    if ((variable_1 > 99) && (variable_2 > 99)) ResetParam();
}

// Get time from EEPROM values
String GetTimeFromEEPROM(int Address) {
    int hour = EEPROM.read(Address);
    int minute = EEPROM.read(Address + 1);
    if (hour > 99) hour = 23;
    if (minute > 99) minute = 59;

    String hourString = (hour < 10 ? "0" + String(hour) : String(hour));
    String minuteString = (minute < 10 ? "0" + String(minute) : String(minute));

    return hourString + ":" + minuteString;
}

// Get last stored values from EEPROM
void GetLastSaveValues() {
    MONDAY_ON_TIME = GetTimeFromEEPROM(ADDR_MONDAY_ON_TIME);
    MONDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_MONDAY_OFF_TIME);
    TUESDAY_ON_TIME = GetTimeFromEEPROM(ADDR_TUESDAY_ON_TIME);
    TUESDAY_OFF_TIME = GetTimeFromEEPROM(ADDR_TUESDAY_OFF_TIME);
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
}






void WeekdayUpdate(){
// Monday
monday.on_time = parseTime(MONDAY_ON_TIME);

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