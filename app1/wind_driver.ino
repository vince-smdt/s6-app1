#ifndef WIND_DRIVER_H
#define WIND_DRIVER_H

#define WIND_SPD_PIN          27
#define WIND_DIR_PIN          35

#define ADC_RESOLUTION        4095
#define VOLTAGE_REF           3.3
#define ADC_ATTEN             ADC_11db

typedef struct {
  uint16_t raw;
  float    dir;
} WindDirection;

static const WindDirection wind_dir_table[] = {
  {  264, 112.5 },
  {  335,  67.5 },
  {  372,  90   },
  {  506, 157.5 },
  {  738, 135   },
  {  979, 202.5 },
  { 1149, 180   },
  { 1624,  22.5 },
  { 1845,  45   },
  { 2397, 247.5 },
  { 2520, 225   },
  { 2810, 337.5 },
  { 3143,   0   },
  { 3309, 292.5 },
  { 3548, 315   },
  { 3780, 270   },
};

static const int TABLE_SIZE = sizeof(wind_dir_table) / sizeof(wind_dir_table[0]);

void init_wind_sensor() {
  // Wind speed
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << WIND_SPD_PIN);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  // Wind direction
  analogReadResolution(12);
  analogSetAttenuation(ADC_ATTEN);
}

void get_wind_speed(float& wind_speed) {
  static unsigned long last_us = 0;
  static int last_state = LOW;
  static float speed_kmh = 0.0;

  int state = digitalRead(WIND_SPD_PIN);
  if (state == HIGH && last_state == LOW) {
    unsigned long now = micros();
    unsigned long dt = now - last_us;
    if (last_us != 0 && dt > 50000) { // debounce 50ms
      speed_kmh = 2.4 / (dt / 1000000.0); // one tick per second is wind speed of 2.4km/h
    }
    last_us = now;
  }
  last_state = state;

  if (last_us != 0 && micros() - last_us > 5000000) {
    speed_kmh = 0.0; // after 5 sec without tick, assume 0km/h
  }

  wind_speed = speed_kmh;
}

// Returns wind direction in degrees
void get_wind_direction(float& wind_dir) {
  int raw = analogRead(WIND_DIR_PIN);

  int best_idx = 0;
  int best_delta = abs(raw - wind_dir_table[0].raw);
  for (int i = 1; i < TABLE_SIZE; i++) {
    int d = abs(raw - wind_dir_table[i].raw);
    if (d < best_delta) { best_delta = d; best_idx = i; }
  }
  wind_dir = wind_dir_table[best_idx].dir;
}

#endif // WIND_DRIVER_H
