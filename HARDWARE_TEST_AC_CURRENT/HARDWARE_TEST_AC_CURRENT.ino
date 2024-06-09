#include "ACS712.h"

#define ACS712_PIN 32
#define RELAY 23


ACS712  ACS(ACS712_PIN, 3.3, 4095, 100);

unsigned int current_val;


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

pinMode(ACS712_PIN, INPUT);
pinMode(RELAY, OUTPUT);

ACS.autoMidPoint();
Serial.print("MidPoint: ");
Serial.print(ACS.getMidPoint());
Serial.print(". Noise mV: ");
Serial.println(ACS.getNoisemV());
}

void loop() {
//  digitalWrite(RELAY, LOW);
//  delay(1000);
//  current_val = ACS.mA_AC_sampling();
//  Serial.print("mA: ");
//  Serial.println(current_val);
//  delay(1000);

  digitalWrite(RELAY, HIGH);
  delay(1000);
  current_val = ACS.mA_AC_sampling();
  Serial.print("mA: ");
  Serial.println(current_val);
  delay(1000);

}
