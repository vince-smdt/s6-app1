#include <Adafruit_DPS310.h>
#include <Wire.h>

#include "driver/gpio.h"

#include "light_driver.ino"
#include "wind_driver.ino"

#define HUMIDITY_PIN    GPIO_NUM_16
#define RAIN_PIN        GPIO_NUM_23

uint32_t rainTips = 0;
uint32_t loops = 0;
bool lastState = HIGH;
uint32_t lastTipTime = 0;
uint32_t lastPrint = 0;

const uint32_t PrintMs = 5000;
const uint32_t debounceMs = 5;
Adafruit_DPS310 dps;

void init_rain() {
  pinMode(RAIN_PIN, INPUT_PULLUP);
}

void get_rain(float& rain){
  bool currentState = digitalRead(RAIN_PIN);
  loops++;

  if (lastState == HIGH && currentState == LOW) {
    if (millis() - lastTipTime > debounceMs) {
      rainTips++;
      lastTipTime = millis();
    }
  }

  if(loops == 100){
    loops     = 0;
    rainTips  = 0;
  }

  rain = (rainTips)*0.3;
  lastState = currentState;
}

void get_humidity(float& humidity) {
  int i, j;
  int duree[42];
  unsigned long pulse;
  byte data[5];
  float humidite;
  float temperature;
  int broche = 16;

  delay(2000);
  
  pinMode(broche, OUTPUT_OPEN_DRAIN);
  digitalWrite(broche, HIGH);
  delay(250);
  digitalWrite(broche, LOW);
  delay(20);
  digitalWrite(broche, HIGH);
  delayMicroseconds(40);
  pinMode(broche, INPUT_PULLUP);
  
  while (digitalRead(broche) == HIGH);
  i = 0;

  do {
        pulse = pulseIn(broche, HIGH);
        duree[i] = pulse;
        i++;
  } while (pulse != 0);
 
  if (i != 42) 
    Serial.printf(" Erreur timing \n"); 

  for (i=0; i<5; i++) {
    data[i] = 0;
    for (j = ((8*i)+1); j < ((8*i)+9); j++) {
      data[i] = data[i] * 2;
      if (duree[j] > 50) {
        data[i] = data[i] + 1;
      }
    }
  }

  if ( (data[0] + data[1] + data[2] + data[3]) != data[4] ) 
    Serial.println(" Erreur checksum");

  humidity = data[0] + (data[1] / 256.0);
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

void print_info(float temperature, float pressure, float humidity, float rain, float light, float wind_dir, float wind_speed) {
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.printf("Humidite: %4.0f \%% \n", humidity);

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println(" %");

  Serial.print("Pluie (mm): ");
  Serial.println(rain);

  Serial.print("Wind Direction: ");
  Serial.print(wind_dir);
  Serial.println(" deg");

  Serial.print("Wind Speed: ");
  Serial.print(wind_speed);
  Serial.println(" km/h");
  
  Serial.println("\n======================");
}

void setup() {
  Serial.begin(9600);

  // ADCs
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  init_barometer();
}

void loop() {
  float temperature, pressure, humidity, rain, wind_dir, wind_speed;
  get_pressure_and_temp(temperature, pressure);
  get_rain(rain);
  //get_humidity(humidity);
  float light = get_light_perc();
  get_wind_direction(wind_dir);
  get_wind_speed(wind_speed);

  if(millis() - lastPrint > PrintMs){
    lastPrint = millis();
    print_info(temperature, pressure, humidity, rain, light, wind_dir, wind_speed);
  }

  delay(50);
}
