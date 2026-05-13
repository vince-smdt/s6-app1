/*
* Auteurs:
*  Vincent Simard-Schmidt (simv2104)
*  Maxime Aubin (aubm1811)
*/

#ifndef RAIN_DRIVER_H
#define RAIN_DRIVER_H

#define RAIN_PIN        GPIO_NUM_23

static const uint32_t debounceMs = 5;
static uint32_t rainTips = 0;
static uint32_t loops = 0;
static bool lastState = HIGH;
static uint32_t lastTipTime = 0;

static void init_rain() {
  pinMode(RAIN_PIN, INPUT_PULLUP);
}

static void get_rain(float &rain) {
  bool currentState = digitalRead(RAIN_PIN);
  loops++;

  // Comptage nombre de fois que la balance a basculee
  if (lastState == HIGH && currentState == LOW) {
    if (millis() - lastTipTime > debounceMs) {
      rainTips++;
      lastTipTime = millis();
    }
  }

  // Si apres suffisament de temps sans pluie, retour a 0
  if (loops == 50) {
    loops = 0;
    rainTips = 0;
  }

  // Conversion montant de pluie
  rain = (rainTips)*0.3;
  lastState = currentState;
}

#endif // RAIN_DRIVER_H
