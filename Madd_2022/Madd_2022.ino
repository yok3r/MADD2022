String ValuesCache, ValuesRecord;

////// Time controller //////
unsigned long previousMillis = 0;
const long interval = 10000;

////// Clock Libraries //////
#include "RTClib.h"
RTC_DS1307 rtc;

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

  ////// Clock Setup //////
#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

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


void readTime() {
  ////// TIME Reading //////
  DateTime time = rtc.now();
  Serial.println(String("DateTime:\t") + time.timestamp(DateTime::TIMESTAMP_FULL));
  Serial.println("\n");

  //Save the time value to variable
  ValuesCache = String("\t") + time.timestamp(DateTime::TIMESTAMP_FULL);
}

void readPM25() {
  ////// PM25 Reading //////
  PM25_AQI_Data data;


  if (!aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    //return;
  }

  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: "));
  Serial.print(data.pm10_standard);
  Serial.print(F("\t\tPM 2.5: "));
  Serial.print(data.pm25_standard);
  Serial.print(F("\t\tPM 10: "));
  Serial.println(data.pm100_standard);

  //Save the time value to variable
  ValuesCache = ValuesCache + "," + data.pm10_standard + "," + data.pm25_standard + "," + data.pm100_standard;
}

void readSCD41() {
  ////// SCD41 Reading //////
  if (mySensor.readMeasurement()) {
    Serial.print(F("CO2(ppm):"));
    Serial.print(mySensor.getCO2());

    Serial.print(F("\tTemperature(C):"));
    Serial.print(mySensor.getTemperature(), 1);

    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(mySensor.getHumidity(), 1);

    Serial.println();
    //Save the time value to variable
  ValuesCache = ValuesCache + "," + mySensor.getCO2() + "," + mySensor.getTemperature() + "," + mySensor.getHumidity();
  }
}

void loop() {

  ////// Time controller //////
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    ValuesCache = "";

    readTime();
    readPM25();
    readSCD41();

    Serial.println(String("Valuecache: ") + ValuesCache);
  }
}
