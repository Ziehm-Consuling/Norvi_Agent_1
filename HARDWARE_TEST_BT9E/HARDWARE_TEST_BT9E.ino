/*
HARDWARE FUNCTIONAL TEST FOR NORVI AGENT 1 CST-BT9E
TESTS IN THE CODE
- DIGITAL INPUT
- RELAY OUTPUT
- FRONT PANEL BUTTON INPUT
- FRONT PANEL PIXEL LED
- TRANSISTOR OUTPUT
- ANALOG INPUT for CURRENT MEASURING
- DS3231 RTC
- NB-IOT MODEM / SIM DETECT

*/




#include <Adafruit_NeoPixel.h>
#include "RTClib.h"
#include <Wire.h>

const int ACS712_PIN = 32; // Analog pin connected to ACS712 output
//const int=0;
const int INPUT_PIN = 27;   // Digital input pin
const int RELAY_PIN = 23;  // Digital output pin


#define GSM_RX 4
#define GSM_TX 2
#define GSM_RESET 19
#define GSM_ENABLE 18

#define OUTPUT2 26
#define BUTTON_PIN 35

#define PIXEL_PIN    25    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//const int analogInPin = 32; // Analog pin connected to ACS712 sensor
//float voltage = 3.3; // Voltage supplied to ACS712 (usually 3.3V for ESP32)
//float sensitivity = 0.185; // Sensitivity of ACS712 for 5A model (185mV/A)
//float offsetVoltage = 2.5; // ACS712 output at zero current for 5A model (2.5V)

bool oldState = HIGH;
int showType = 0;

int reading[8]={0};
String adcString[8];

RTC_DS3231 rtc; 
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

unsigned long int timer1 = 0;
int current_int = 0;

int analog_value = 0;

  

void setup() {
  
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX); 
  
  pinMode(INPUT_PIN, OUTPUT); pinMode(BUTTON_PIN, INPUT);  pinMode(RELAY_PIN, OUTPUT); pinMode(OUTPUT2, OUTPUT); pinMode(GSM_RESET, OUTPUT); pinMode(GSM_ENABLE, OUTPUT);
  digitalWrite(GSM_RESET, HIGH); digitalWrite(GSM_ENABLE, HIGH); /* Turn ON output*/ 
  
  digitalWrite(RELAY_PIN, HIGH); // Turn ON output
  digitalWrite(INPUT_PIN, LOW); // Turn ON output
  
  delay(1000);  // Delay for 1000ms
  Serial.print("RELAY TURNED ON FOR TESTING");
  
  Wire.begin(21,22);
  
  RTC_Check();
  delay(1000);
  i2c_Check();
  delay(1000);
  
 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  showType++;
      if (showType > 9)
        showType=0;
      startShow(showType);

  // Serial.println("\nI2C Scanner");
  digitalWrite(RELAY_PIN, LOW); // Turn ON output
  digitalWrite(INPUT_PIN, HIGH); // Turn ON output
  pinMode(OUTPUT2, OUTPUT);
  pinMode(INPUT_PIN, INPUT);
  delay(1000);  // Delay for 1000ms

  
 Serial.print("TESTING MODEM ATEMPT 1");
 timer1 = millis();
  Serial1.println("AT");
  while(millis()<timer1+10000){
    while (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
    }
  }

  Serial.print("TESTING MODEM ATEMPT 2");
  timer1 = millis();
  Serial1.println("AT");
  while(millis()<timer1+10000){
    while (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
    }
  }

  Serial.print("TESTING MODEM ATEMPT 3");
  timer1 = millis();
  Serial1.println("AT+CPIN?");
  while(millis()<timer1+10000){
    while (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
    }
  }

   Serial.println("Testing Modem Done");
  
}

void loop() {

 
  // Set the last button state to the old state.
//  oldState = newState;
  
    // Set the last button state to the old state.
    int inputState = digitalRead(INPUT_PIN);
    
 
    while (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
    }

    while (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
    }

    int sensorValue = analogRead(ACS712_PIN);
    float current = sensorValue; // 2.5V offset and 185mV/A sensitivity
  bool newState = digitalRead(INPUT_PIN);

  

  Serial.print("Current: "); Serial.print(current); Serial.print("ma|Input 1: ");Serial.println(newState);
  digitalWrite(RELAY_PIN, LOW); 

    Serial.println(digitalRead(BUTTON_PIN));
  
   
   // Toggle the output pin based on the input state
  if (inputState == HIGH) digitalWrite(RELAY_PIN, HIGH); // Turn ON output
  else digitalWrite(RELAY_PIN, LOW);  // Turn OFF output

 digitalWrite(OUTPUT2, LOW); delay(300);
 digitalWrite(OUTPUT2, HIGH); delay(300);
}


void displayTime(void) {
  DateTime now = rtc.now();
     
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);

}

void RTC_Check(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
 else{
 if (rtc.lostPower()) {
  
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
  }

   
  int a=1;
  while(a<6)
  {
  displayTime();   // printing time function for oled
  a=a+1;
  }
 }
}




void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void i2c_Check(){

 byte error, address;
  int deviceCount = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");

      deviceCount++;
      delay(1);  // Wait for a moment to avoid overloading the I2C bus
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("Scanning complete\n");
  }
  delay(100);

   // put your main code here, to run repeatedly:
  bool newState = digitalRead(INPUT_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(INPUT_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 9)
        showType=0;
      startShow(showType);
    }
  }
}
