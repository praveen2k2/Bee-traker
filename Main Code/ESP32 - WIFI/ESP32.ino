#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"
#include <ArduinoJson.h>

// Menu lib
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button2.h"

#define BUTTON_PIN  23  // This is the pin that the physical button is attached to

Button2 button;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int MainMenu = 0;
int menuOffset = 0;  // Keeps track of the current offset for scrolling
bool isSleeping = false;

String HomeMenu[]  = {
  "Temperature",
  "Humidity",
  "Weight",
  "Bee Count",
  "QR Code",      
  "Sleep"
};

// This is the amount of items in the MAIN MENU
int const TotalHomeItems = sizeof(HomeMenu) / sizeof(HomeMenu[0]);
const int itemHeight = 16;  // Height of each menu item in pixels
const int visibleItems = SCREEN_HEIGHT / itemHeight;  // Number of items that fit on the screen

unsigned long lastActivity = 0;  // Track the last activity time
const unsigned long sleepTimeout = 10000;  // 10 seconds timeout


#define WIFI_SSID "SLT-LTE-WiFi-FA19"
#define WIFI_PASSWORD "7L0820N1043"
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

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;

FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

float temperature;
float humidity;
float weight = 500;
int count = 404;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;

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
  databasePath = "/UsersData/" + uid + "/Hive-01";

  delay(50);

  button.begin(BUTTON_PIN);

  button.setClickHandler(click);
  button.setLongClickDetectedHandler(longClick);
  button.setDoubleClickHandler(doubleClick);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Start by showing the MAIN MENU
  showMenu();
  lastActivity = millis();  // Initialize last activity time
}

void loop() {
  if (Serial.available() > 0) {
    String jsonString = Serial.readStringUntil('\n');  // Read data until newline character

    // Parse the JSON string
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extract values
    temperature = doc["temperature"];
    humidity = doc["humidity"];

    // Send new readings to database
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      //Get current timestamp
      timestamp = getTime();
      Serial.print("time: ");
      Serial.println(timestamp);

      parentPath = databasePath + "/" + String(timestamp);

      json.set(tempPath.c_str(), float(temperature));
      json.set(humPath.c_str(), float(humidity));
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
  }

  button.loop();
  if (!isSleeping && (millis() - lastActivity > sleepTimeout)) {
    enterSleepMode();
  }
}

// OLED menu
void showMenu() {
  if (isSleeping) return;  // Do not update the display if in sleep mode

  Serial.println("Request to showMenu");

  display.clearDisplay();
  display.setTextColor(WHITE);

  int yPosition = 0;  // Start at the top of the screen

  for (byte idx = menuOffset; idx < menuOffset + visibleItems; idx++) {
    if (idx >= TotalHomeItems) {
      break;  // Stop if we've reached the end of the menu items
    }

    if (MainMenu == idx) {
      display.setTextSize(2);
      display.setCursor(10, yPosition);
      display.fillRect(0, yPosition, SCREEN_WIDTH, itemHeight, WHITE);
      display.setTextColor(BLACK);
      display.print("> ");
      display.print(HomeMenu[idx]);
      display.setTextColor(WHITE);
    } else {
      display.setTextSize(1);
      display.setCursor(15, yPosition);
      display.print(HomeMenu[idx]);
    }
    yPosition += itemHeight;  // Move down by the height of each item
  }

  display.display();
}

void click(Button2& btn) {
  if (isSleeping) {
    wakeUp();
    return;
  }

  MainMenu++;
  if (MainMenu >= TotalHomeItems) {
    MainMenu = 0;
  }

  if (MainMenu >= menuOffset + visibleItems) {
    menuOffset++;
  }

  if (MainMenu < menuOffset) {
    menuOffset = MainMenu;
  }

  showMenu();
  lastActivity = millis();  // Update last activity time
}

// Define what a long press will do
void longClick(Button2& btn) {
  if (isSleeping) {
    wakeUp();
    return;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);

  if (MainMenu == 0) {
    Serial.println("LONG PRESS - TEMPERATURE");
    display.print("Temp: ");
    display.println(temperature);
  } else if (MainMenu == 1) {
    Serial.println("LONG PRESS - HUMIDITY");
    display.print("Humidity: ");
    display.println(humidity);
  } else if (MainMenu == 2) {
    Serial.println("LONG PRESS - WEIGHT");
    display.print("Weight: ");
    display.println(weight);
  } else if (MainMenu == 3) {
    Serial.println("LONG PRESS - BEE COUNT");
    display.print("Count: ");
    display.println(count);
  } else if (MainMenu == 4) {
    Serial.println("LONG PRESS - QR");
    display.print("QR Code");
  } else if (MainMenu == 5) {
    Serial.println("LONG PRESS - SLEEP");
    enterSleepMode();
    return;
  }

  display.display();
  delay(5000); // Display the information for 5 seconds

  showMenu();
  lastActivity = millis();  // Update last activity time
}

void doubleClick(Button2& btn) {
  MainMenu = 0;
  menuOffset = 0;
  showMenu();
  lastActivity = millis();  // Update last activity time
}

void enterSleepMode() {
  Serial.println("Entering sleep mode");
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  isSleeping = true;
}

void wakeUp() {
  Serial.println("Waking up from sleep mode");
  display.ssd1306_command(SSD1306_DISPLAYON);
  isSleeping = false;
  MainMenu = 0;        // Reset MainMenu to start from the beginning
  menuOffset = 0;      // Reset menuOffset to display from the top
  showMenu();          // Update the main menu upon waking up
  lastActivity = millis();  // Update last activity time
}
