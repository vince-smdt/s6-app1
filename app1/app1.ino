#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "driver/gpio.h"

#include "barometer_driver.ino"
#include "humidity_driver.ino"
#include "light_driver.ino"
#include "wind_driver.ino"

#define SERVICE_UUID           "88d01c2e-cec9-4ae4-9597-515a7fd707de"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "88d01c2e-cec9-4ae4-9597-515a7fd707de"
#define CHARACTERISTIC_UUID_TX "88d01c2e-cec9-4ae4-9597-515a7fd707de"

#define RAIN_PIN        GPIO_NUM_23

uint32_t rainTips = 0;
uint32_t loops = 0;
bool lastState = HIGH;
uint32_t lastTipTime = 0;
uint32_t lastPrint = 0;

const uint32_t PrintMs = 5000;
const uint32_t debounceMs = 5;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 1;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

void init_BLE() {
  // Create the BLE Device
  BLEDevice::init("Weather Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  // Descriptor 2902 is not required when using NimBLE as it is automatically added based on the characteristic properties
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

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

void print_info(float temperature, float pressure, float humidity, float rain, float light, float wind_dir, float wind_speed) {
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial2.printf("Humidite: %4.0f \%% \n", humidity);

  Serial2.print("Pressure: ");
  Serial2.print(pressure);
  Serial2.println(" hPa");

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println(" %");

  Serial2.print("Pluie (mm): ");
  Serial2.println(rain);

  Serial2.print("Wind Direction: ");
  Serial2.print(wind_dir);
  Serial2.println(" deg");

  Serial2.print("Wind Speed: ");
  Serial2.print(wind_speed);
  Serial2.println(" km/h");
  
  Serial2.println("\n======================");
}

void setup() {
  Serial.begin(9600);

  // ADCs
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial2.begin(9600, SERIAL_8N1, 15, 14);

  init_BLE();
  init_barometer();
}

void loop() {
  float temperature, pressure, rain, wind_dir, wind_speed;
  get_pressure_and_temp(temperature, pressure);
  get_rain(rain);
  float humidity = get_humidity();
  float light = get_light_perc();
  get_wind_direction(wind_dir);
  get_wind_speed(wind_speed);

  if(millis() - lastPrint > PrintMs){
    if (deviceConnected) {
      pTxCharacteristic->setValue(&txValue, 1);
      pTxCharacteristic->notify();
      Serial.println("----- NOTIFY -----");
    }
    /*lastPrint = millis();
    print_info(temperature, pressure, humidity, rain, light, wind_dir, wind_speed);*/
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("Started advertising again...");
    oldDeviceConnected = false;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = true;
  }

  String receivedMessage = "";
  while (Serial2.available()) {
    char incomingChar = Serial2.read();  // Read each character from the buffer
    
    if (incomingChar == 'R') {  // Check if the user pressed Enter (new line character)
      // Print the message
      Serial.println("Received request");
      print_info(temperature, pressure, humidity, rain, light, wind_dir, wind_speed);
      
      // Clear the message buffer for the next input
      receivedMessage = "";
    } else {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }
  delay(50);
}
