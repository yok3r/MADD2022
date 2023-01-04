////// Time controller //////
unsigned long previousMillis = 0;
const long interval = 5000;

////// SCD41 Libraries ////// - Sparkfun SCD4X
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
SCD4x mySensor;

////// PM25 Libraries ////// - Adafruit PM25 AQI
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();


void setup() {
  Serial.begin(115200);
  // Wait one second for sensor to boot up!
  delay(1000);

  ////// PM25 Setup //////
  pmSerial.begin(9600);
  if (!aqi.begin_UART(&pmSerial)) {  // connect to the sensor over software serial
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  ////// SCD41 Setup //////
  Wire.begin();
  if (mySensor.begin() == false) {
    Serial.println(F("SCD41 Sensor not detected."));
    while (1)
      ;
  }
}

void recordValues() {
  ////// SCD41 Reading //////
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

  ////// PM25 Reading //////
  PM25_AQI_Data data;

  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: "));
  Serial.print(data.pm10_standard);
  Serial.print(F("\t\tPM 2.5: "));
  Serial.print(data.pm25_standard);
  Serial.print(F("\t\tPM 10: "));
  Serial.println(data.pm100_standard);
  Serial.println(F("Concentration Units (environmental)"));
  Serial.println(F("---------------------------------------"));
}


void loop() {

  ////// Time controller //////
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    recordValues();

  }
}
