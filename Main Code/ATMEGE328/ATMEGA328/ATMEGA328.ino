#include "DHT.h"
#include <ArduinoJson.h>

//define all pins
#define WEIGHT_SENSOR_PIN A1
#define ESP32_GPIO_WAKEUP A2
#define DHTPIN 2  // Digital pin connected to the DHT sensor
#define BEECOUNTERPIN 3  // Pin for the entry sensor

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int beeCount = 0;

void temperature(float &h, float &t);
void countBees(int &beeCount);
void readWeight(float &weight);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(BEECOUNTERPIN, INPUT);
}

void loop() {
  float h, t, weight;

  temperature(h, t);
  countBees(beeCount);
  readWeight(weight);
  
  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["count"] = beeCount / 2;
  doc["weight"] = weight;

  // Serialize JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send JSON string over Serial
  Serial.println(jsonString);

  delay(5000);  // Delay for 5 seconds before sending the next reading
}



//sensors management 

void temperature(float &h, float &t) {
  // Wait a few seconds between measurements.
  delay(2000);
  
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    // Set values to NaN to indicate failure
    h = t = NAN;
    return;
  }
}

void countBees(int &beeCount) {
  // Read the state of the entry sensor
  if (digitalRead(BEECOUNTERPIN) == HIGH) {
    beeCount++;
    Serial.print("Bees Entered: ");
    Serial.println(beeCount);
    delay(100);  // Debounce delay to avoid multiple counts for a single bee
    return;
  }
}

void readWeight(float &weight) {
  // Read weight from sensor
  // Example code to read weight from a load cell sensor
  // Replace with your actual weight sensor code
  int rawValue = analogRead(WEIGHT_SENSOR_PIN);
  weight = map(rawValue, 0, 1023, 0, 5000); // Assuming 0-5000g range
  return weight;
}


//memory and process management part
void blabla(){}


//power mannagement part 
void esp_deep_sleep_start() {
  }
  
void powerMannager() {
  // Put ESP32 to sleep
  digitalWrite(ESP32_GPIO_WAKEUP, LOW);  // Wakeup pin LOW
  delay(100);  // Wait for serial data to be sent
  esp_deep_sleep_start();
  //add other codes
}

void powerUp(){
  //calibrating IR sensors
  //chack for each sensor is working
  //chack for esp32 and its connectivity
  }



//error management part

void onBoardLED(){
  // desply the funtion running ,data communication and some other data from a LED beeps. For error mannaging.
  }

void errorDiagnosis(){
  //try to detect any posible errors in the system
  //unable to communicate with esp
  //not enought power : indentify from power down of ic when tring to power up a sensor.disconnect ESP after some time in a periodic way
  //water leak : unexpected signal from disconnected pins
  //continuvity check
  }
