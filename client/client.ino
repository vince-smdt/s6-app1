#include "driver/gpio.h"

static int last_req_ts_ms = 0;
static const int REQUEST_DELAY_MS = 5000;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 21, 22);
  last_req_ts_ms = millis();
}

void loop() {
  int now = millis();
  if ((now - last_req_ts_ms) > REQUEST_DELAY_MS) {
    Serial2.write('R');
    last_req_ts_ms = now;
  }

  while (Serial2.available()) Serial.write(Serial2.read());
}
