////// Time controller //////
unsigned long previousMillis = 0;
const long interval = 10000;

////// SCD41 Libraries ////// - Sparkfun SCD4X
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
SCD4x mySensor;

////// PM25 Libraries ////// - Adafruit PM25 AQI
#include "Adafruit_PM25AQI.h"


void setup() {
  Serial.begin(115200);

  ////// SCD41 Setup //////
  Wire.begin();
  if (mySensor.begin() == false) {
    Serial.println(F("SCD41 Sensor not detected. Please check wiring."));
    while (1)
      ;
  }
}

void recordValues() {
  ////// SCD41 Setup //////
  if (mySensor.readMeasurement())  // readMeasurement will return true when fresh data is available
  {
    Serial.print(F("CO2(ppm):"));
    Serial.print(mySensor.getCO2());

    Serial.print(F("\tTemperature(C):"));
    Serial.print(mySensor.getTemperature(), 1);

    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(mySensor.getHumidity(), 1);

    Serial.println();
  }
}


void loop() {

  ////// Time controller //////
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    recordValues();
  }
}
