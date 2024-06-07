#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN 2  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
#define BEECOUNTERPIN 3  // Pin for the entry sensor

DHT dht(DHTPIN, DHTTYPE);

int beeCount = 0;

void temperature(float &h, float &t);
void countBees(int &beeCount);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(BEECOUNTERPIN, INPUT);
}

void loop() {
  float h, t;
  
  temperature(h, t);
  countBees(beeCount);
  
  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["count"] = beeCount / 2;

  // Serialize JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send JSON string over Serial
  Serial.println(jsonString);

  delay(5000);  // Delay for 5 seconds before sending the next reading
}

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
  }
}
