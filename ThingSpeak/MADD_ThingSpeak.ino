////// Program Control //////
bool Monitor_PM25 = true;
bool Monitor_SCD41 = true;
bool Monitor_log = false;

////// Time controller //////
unsigned long previousMillis = 0;
unsigned long interval = 20000;

///// PM25 Libraries ///// - Adafruit PM25 AQI
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
PM25_AQI_Data data;

///// SCD41 Libraries ///// - Sparkfun SCD4X
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
SCD4x mySensor;

///// Thingspeak libraries /////
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 

//change these names  with your own Wifi configuration!
char ssid[] = "YOUR_WIFI_NAME_HERE";   // your network SSID (name) 
char pass[] = "YOUR_WIFI_PASSWORD_HERE";   // your network password
int keyIndex = 0;   // your network key Index number (needed only for WEP)
WiFiClient  client;

//change these numbers with your own Thingspeak configuration!
unsigned long myChannelNumber = YOUR_CHANNEL_NUMBER_HERE;
const char * myWriteAPIKey = "YOUR_WRITE_API_KEY_HERE";

int number = 0;

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
  
  ///// Thingspeak Setup /////
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void connectThingspeak() {
  //// Thingspeak Wifi /////
  if(WiFi.status() != WL_CONNECTED){ // Connect or reconnect to WiFi
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
    delay(1000);
  }
}

void readPM25() {
  ////// PM25 Reading //////
  while (!aqi.read(&data)) {
    if (Monitor_log) {
      Serial.println("Waiting PM25 sensor data...");
    }
    delay(500);
  }
  if (Monitor_PM25) {
    Serial.print(F("PM1.0:"));
    Serial.print(data.pm10_standard);
    Serial.print("µg/m3\t");
    Serial.print(F("PM2.5:"));
    Serial.print(data.pm25_standard);
    Serial.print("µg/m3\t");
    Serial.print(F("PM10:"));
    Serial.print(data.pm100_standard);
    Serial.print("µg/m3");
    Serial.println();
  }
}

void readSCD41() {
  ////// SCD41 Reading //////
  while (mySensor.readMeasurement() == 0) {
    if (Monitor_log) {
      Serial.println("Waiting SCD41 sensor data...");
    }
    delay(500);
  }
  if (Monitor_SCD41) {
    Serial.print(F("Temperature:"));
    Serial.print(mySensor.getTemperature(), 1);
    Serial.print("ºC\t");
    Serial.print(F("Humidity:"));
    Serial.print(mySensor.getHumidity(), 1);
    Serial.print("%RH\t");
    Serial.print(F("CO2:"));
    Serial.print(mySensor.getCO2());
    Serial.print("ppm");
    Serial.println();
  }
}

void writeThingspeak() {
  //// Thingspeak Writting /////
  // set the fields with the values
  ThingSpeak.setField(1, mySensor.getTemperature());
  ThingSpeak.setField(2, mySensor.getHumidity());
  ThingSpeak.setField(3, mySensor.getCO2());
  ThingSpeak.setField(4, data.pm10_standard);
  ThingSpeak.setField(5, data.pm25_standard);
  ThingSpeak.setField(6, data.pm100_standard);
  
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful");
    }
    else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }  
}

void loop() {
  ////// Time controller //////
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    connectThingspeak();
    readPM25();
    readSCD41();
    writeThingspeak();
  }
}
