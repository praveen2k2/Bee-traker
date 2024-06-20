#include "DHT.h"
#include <ArduinoJson.h>
#define DHTPIN 7  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define WAKE_UP_PIN 10

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(WAKE_UP_PIN, OUTPUT);
}

void loop() {

  digitalWrite(WAKE_UP_PIN, HIGH);

  float h, t;
  // Call the temp_humid function and assign the return values to variables
  temperature(h, t);

  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;

  // Serialize JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send JSON string over Serial
  Serial.println(jsonString);


  digitalWrite(WAKE_UP_PIN, LOW);


  delay(30000);  // Delay for 30 seconds before sending the next reading
}

void temperature(float &h, float &t) {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    // Set values to NaN to indicate failure
    h = t = NAN;
    return;
  }
}
