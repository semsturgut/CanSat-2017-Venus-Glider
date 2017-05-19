
#include "Pressure.h"

#define PRESSURE_PIN A1

Pressure MPXV7002DP(PRESSURE_PIN);

void setup() {
Serial.begin(9600);
Serial.println("init...");
int val = MPXV7002DP.Init();
Serial.println("init done!");
Serial.println(val);
}

void loop() {
int val2 = MPXV7002DP.GetAirSpeed();
Serial.println(val2);
}
