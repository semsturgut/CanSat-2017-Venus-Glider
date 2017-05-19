/*
  Pressure.h - Library for pressure sensor.
  Created by johnlenfr, May 15, 2014.
  http://johnlenfr.1s.fr
  
  SOURCE CODE .CPP
  
*/

#include "Arduino.h"
#include "Pressure.h"

Pressure::Pressure(int pin){
//
pinMode(pin,INPUT);
_pin = pin;
}

int Pressure::Init(){
  //
  ref_pressure = analogRead(_pin);
    for (int i=1;i<=200;i++)
    {
      ref_pressure = (analogRead(_pin))*0.25 + ref_pressure*0.75;
      delay(20);
    }
return ref_pressure;
}

int Pressure::GetAirSpeed(){
  //
  int air_pressure = 0;
  for (int i = 0; i < 8; i++)
    air_pressure += analogRead (_pin);
  air_pressure >>= 3;
  if (air_pressure < ref_pressure)
    air_pressure = ref_pressure;
  
  pitotpressure = 5000.0f * ((air_pressure - ref_pressure) / 1024.0f) + PRESSURE_SEA_LEVEL;    // differential pressure in Pa, 1 V/kPa, max 3920 Pa
  ambientpressure = PRESSURE_SEA_LEVEL;
  temperature = 20.0f + ABSOLUTE_0_KELVIN;
  density = (ambientpressure * DRY_AIR_MOLAR_MASS) / (temperature * UNIVERSAL_GAS_CONSTANT);
  airspeed_ms = sqrt ((2 * (pitotpressure - ambientpressure)) / density);
  airspeed_kmh = airspeed_ms * 3.600;  // convert to km/h
  
  return airspeed_ms;
}