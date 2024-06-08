#ifndef EEPROM_FUNCTIONS_H
#define EEPROM_FUNCTIONS_H

#include <Arduino.h>
#include <EEPROM.h>  // Include EEPROM library
#include "eeprom_variables.h" // Include the variables
#include "eeprom_addresses.h"


void EEPROM_RESET_CHECK();
void GetLastSaveValues();
void writeString(int startingAddress, String data);
String readString(int startingAddress, int length);
void ResetParam();
void WeekdayUpdate();
void SpecialDayUpdate();
Time parseTime(const String& timeStr);
Date parseDate(const String& dateStr);

#endif // EEPROM_FUNCTIONS_H

