#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"

#define WIFI_SSID "ROUTER SSID"
#define WIFI_PASSWORD "ROUTER PASSWORD"
#define API_KEY "AIzaSyA6RyU5sX58C9uhyN1QYAvbMZhn8m3eP3Y"
#define DATABASE_URL "https://hivelink-abd1a-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "user@gmail.com"
#define USER_PASSWORD "User@2002"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

String tempPath = "/temperature";
String humPath = "/humidity";
String timePath = "/timestamp";

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;

FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

float temperature;
float humidity;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(9600);  // Initialize Serial at 9600 baud rate

  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // Read data until newline character

    // Example data format: "Temperature: 25.5 °C, Humidity: 50.2 %"
    int tempIndex = data.indexOf("Temperature: ");  // Find the index of the temperature value
    if (tempIndex != -1) {
      temperature = data.substring(tempIndex + 13, data.indexOf(" °C")).toFloat();  // Extract temperature
    }

    int humidIndex = data.indexOf("Humidity: ");  // Find the index of the humidity value
    if (humidIndex != -1) {
      humidity = data.substring(humidIndex + 10, data.indexOf(" %")).toFloat();  // Extract humidity
    }

    // Send new readings to database
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      //Get current timestamp
      timestamp = getTime();
      Serial.print("time: ");
      Serial.println(timestamp);

      parentPath = databasePath + "/" + String(timestamp);

      json.set(tempPath.c_str(), String(temperature));
      json.set(humPath.c_str(), String(humidity));
      json.set(timePath, String(timestamp));
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
  }
}
