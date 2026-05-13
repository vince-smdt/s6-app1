/*
* Auteurs:
*  Vincent Simard-Schmidt (simv2104)
*  Maxime Aubin (aubm1811)
*/

#ifndef BAROMETER_DRIVER_H
#define BAROMETER_DRIVER_H

#include <Adafruit_DPS310.h>
#include <Wire.h>

#include "barometer_driver.ino"

inline Adafruit_DPS310 dps;

inline void init_barometer() {
  // Initialise pins I2C pour barometre
  Wire.begin(21, 22);
  if (!dps.begin_I2C()) {
    Serial.println("DPS310 not found! Barometer will not work.");
  } else {
    Serial.println("DPS310 ready! Barometer will work.");
  }
}

// Returns temperature in degrees celsius and pressure in hPa.
inline void get_pressure_and_temp(float &temperature, float &pressure) {
  // Utilisation librairie DPS310 pour recuperer temperature et pression
  sensors_event_t temp_event, pressure_event;
  dps.getEvents(&temp_event, &pressure_event);
  temperature = temp_event.temperature;
  pressure = pressure_event.pressure;
}

#endif // BAROMETER_DRIVER_H
