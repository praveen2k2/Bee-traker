#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button2.h"
#include <esp_sleep.h>

// WiFi and Firebase configuration
#define WIFI_SSID "SLT-LTE-WiFi-FA19"
#define WIFI_PASSWORD "7L0820N1043"
#define API_KEY "AIzaSyA6RyU5sX58C9uhyN1QYAvbMZhn8m3eP3Y"
#define DATABASE_URL "https://hivelink-abd1a-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "user@gmail.com"
#define USER_PASSWORD "User@2002"

// Button and display configuration
#define BUTTON_PIN  23
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Wake-up pin
#define WAKE_UP_PIN GPIO_NUM_25

Button2 button;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Menu variables
int MainMenu = 0;
int menuOffset = 0;
bool isSleeping = false;
const int itemHeight = 16;
const int visibleItems = SCREEN_HEIGHT / itemHeight;
unsigned long lastActivity = 0;
const unsigned long sleepTimeout = 10000; // 10 seconds

// Home menu items
String HomeMenu[]  = {
  "Temperature",
  "Humidity",
  "Weight",
  "Bee Count",
  "QR Code",      
  "Sleep"
};
int const TotalHomeItems = sizeof(HomeMenu) / sizeof(HomeMenu[0]);

// Firebase variables
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String tempPath = "/temperature";
String humPath = "/humidity";
String databasePath;
String parentPath;
int timestamp;
FirebaseJson json;

// NTP server
const char* ntpServer = "pool.ntp.org";

// Sensor readings
float temperature;
float humidity;
float weight = 500;
int count = 404;

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;

// Function declarations
void initWiFi();
unsigned long getTime();
void showMenu();
void click(Button2& btn);
void longClick(Button2& btn);
void doubleClick(Button2& btn);
void enterSleepMode();
void wakeUp();
void deepSleep();
void handleFirebaseUpload();

void setup() {
  Serial.begin(9600);

  // Wake up reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 || wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Woke up from deep sleep");
    initWiFi();
    handleFirebaseUpload(); // Process any incoming data immediately after waking up
    lastActivity = millis();
  } else {
    initWiFi();
    configTime(0, 0, ntpServer);

    // Firebase configuration
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback;
    config.max_token_generation_retry = 5;

    Firebase.begin(&config, &auth);

    // Wait for user UID
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "") {
      Serial.print('.');
      delay(1000);
    }
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);

    databasePath = "/UsersData/" + uid + "/Hive-01";

    button.begin(BUTTON_PIN);
    button.setClickHandler(click);
    button.setLongClickDetectedHandler(longClick);
    button.setDoubleClickHandler(doubleClick);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;);
    }

    showMenu();
    lastActivity = millis();
  }

  // Set up the wake-up pin
  esp_sleep_enable_ext0_wakeup(WAKE_UP_PIN, 0); // Use pin 25 to wake up from serial data
  esp_sleep_enable_ext1_wakeup(BIT(BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH); // Use button to wake up
}

void loop() {
  handleFirebaseUpload();
  button.loop();
  
  // Check if the ESP32 should enter sleep mode
  if (!isSleeping && (millis() - lastActivity > sleepTimeout)) {
    enterSleepMode();
  }
}

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

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now;
}

// OLED menu functions
void showMenu() {
  if (isSleeping) return;
  Serial.println("Request to showMenu");

  display.clearDisplay();
  display.setTextColor(WHITE);

  int yPosition = 0;
  for (byte idx = menuOffset; idx < menuOffset + visibleItems; idx++) {
    if (idx >= TotalHomeItems) break;

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
    yPosition += itemHeight;
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
  lastActivity = millis();
}

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
  delay(5000);
  showMenu();
  lastActivity = millis();
}

void doubleClick(Button2& btn) {
  MainMenu = 0;
  menuOffset = 0;
  showMenu();
  lastActivity = millis();
}

void enterSleepMode() {
  Serial.println("Entering sleep mode");
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  isSleeping = true;
  deepSleep();
}

void wakeUp() {
  Serial.println("Waking up from sleep mode");
  display.ssd1306_command(SSD1306_DISPLAYON);
  isSleeping = false;
  MainMenu = 0;
  menuOffset = 0;
  showMenu();
  lastActivity = millis();
}

void deepSleep() {
  Serial.println("Entering deep sleep");
  esp_sleep_enable_ext0_wakeup(WAKE_UP_PIN, 0); // Use pin 25 to wake up from serial data
  esp_sleep_enable_ext1_wakeup(BIT(BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH); // Use button to wake up
  esp_deep_sleep_start();
}

void handleFirebaseUpload() {
  // Check if serial data is available
  if (Serial.available() > 0) {
    String jsonString = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extract data from JSON
    temperature = doc["temperature"];
    humidity = doc["humidity"];

    // Upload data to Firebase if ready and the timer delay has passed
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();
      timestamp = getTime();
      Serial.print("time: ");
      Serial.println(timestamp);

      parentPath = databasePath + "/" + String(timestamp);
      json.set(tempPath.c_str(), float(temperature));
      json.set(humPath.c_str(), float(humidity));
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
      
      // Update last activity time
      lastActivity = millis();
    }
  }
}
