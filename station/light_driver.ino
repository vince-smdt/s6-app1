/*
* Auteurs:
*  Vincent Simard-Schmidt (simv2104)
*  Maxime Aubin (aubm1811)
*/

#ifndef LIGHT_DRIVER_H
#define LIGHT_DRIVER_H

#define LIGHT_PIN                   GPIO_NUM_34

#define LIGHT_ADC_RESOLUTION        4095.0
#define LIGHT_VOLTAGE_REF           3.3
#define LIGHT_MAX_VOLTAGE           2.0
#define R1_RESISTANCE               68_000.0  // in ohms

// Returns brightness level as percentage (0-100)
float get_light_perc() {
  // Conversion voltage a pourcentage eclairage (diviseur de tension)
  int raw = analogRead(LIGHT_PIN);
  float voltage = LIGHT_VOLTAGE_REF * (raw / LIGHT_ADC_RESOLUTION);
  return 100.0 * (voltage / LIGHT_MAX_VOLTAGE);
}

#endif // LIGHT_DRIVER_H
