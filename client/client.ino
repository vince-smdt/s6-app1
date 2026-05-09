#include "driver/gpio.h"

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 21, 22);
}

void loop() {
  Serial.println("This is a test");
  delay(1000);
}
