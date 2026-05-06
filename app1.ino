#include <Adafruit_DPS310.h>
#include <Wire.h>

#include "driver/gpio.h"

#define LIGHT_PIN       GPIO_NUM_34

Adafruit_DPS310 dps;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello World!");

  gpio_config_t io_conf = {};

  // Light
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << LIGHT_PIN);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  // Barometer (I2C)
  Wire.begin(21, 22);
  if (!dps.begin_I2C()) {
    Serial.println("DPS310 not found!");
    while (1);
  }
  Serial.println("DPS310 ready!");
}

void loop() {
  sensors_event_t temp_event, pressure_event;

  dps.getEvents(&temp_event, &pressure_event);

  Serial.print("Temp: ");
  Serial.print(temp_event.temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");

  int light_value = gpio_get_level(LIGHT_PIN);
  Serial.print("Light: ");
  Serial.println(light_value);

  delay(1000);
}