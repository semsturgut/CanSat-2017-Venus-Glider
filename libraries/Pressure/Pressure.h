/*
  Pressure.h - Library for pressure sensor.
  Created by johnlenfr, May 15, 2014.
  http://johnlenfr.1s.fr
  
  HEADER CODE .H
  
*/

#ifndef Pressure_h // Basically, this prevents problems if someone accidently #include's the library twice.
#define Pressure_h // starting .h

#include "Arduino.h" // needed for constants and functions declarations

// DEFINE CONSTANTS
// #define	constant	value
#define ABSOLUTE_0_KELVIN      273.16f
#define PRESSURE_SEA_LEVEL     101325.0f
#define UNIVERSAL_GAS_CONSTANT 8.3144621f
#define DRY_AIR_MOLAR_MASS     0.0289644f

class Pressure{ // name of the class

  public:               // public methods and variables
  Pressure(int pin);    // constructor of the class
  int Init();
  int GetAirSpeed();
  
  private:              // private methods and variables
  int   _pin;
  int   ref_pressure;
  float pitotpressure;
  float ambientpressure;
  float temperature;
  float airspeed_ms;
  float airspeed_kmh;
  float density;
};

//extern Pressure Pressure;  //Externalisation de l'objet. 

#endif // ending .h