#include <Adafruit_DPS310.h>
#include <Wire.h>

#include "driver/gpio.h"

#define LIGHT_PIN       GPIO_NUM_34

Adafruit_DPS310 dps;

void init_light() {
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << LIGHT_PIN);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
}

// Returns 1 if light detected, else 0.
int get_light_bool() {
  return gpio_get_level(LIGHT_PIN);
}

void init_barometer() {
  Wire.begin(21, 22);
  if (!dps.begin_I2C()) {
    Serial.println("DPS310 not found! Barometer will not work.");
  } else {
    Serial.println("DPS310 ready! Barometer will work.");
  }
}

// Returns temperature in degrees celsius and pressure in hPa.
void get_pressure_and_temp(float& temperature, float& pressure) {
  sensors_event_t temp_event, pressure_event;
  dps.getEvents(&temp_event, &pressure_event);
  temperature = temp_event.temperature;
  pressure = pressure_event.pressure;
}

void setup() {
  Serial.begin(9600);
  init_light();
  init_barometer();
}

void loop() {
  float temperature, pressure;
  get_pressure_and_temp(temperature, pressure);

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  int light_value = get_light_bool();
  Serial.print("Light: ");
  Serial.println(light_value);

  delay(1000);
}
