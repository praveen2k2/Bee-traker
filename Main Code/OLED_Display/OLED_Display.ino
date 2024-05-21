/*

* SINGLE click:   Go down in MENU or SUBMENU items
* LONG click:     Enter SUBMENU or RUN Command when inside SUBMENU
* DOUBLE click:   when inside SUBMENU, goes back to MAIN MENU

*/


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

float temperature = 32;
float humidity = 80;
float weight = 500;
int count = 404;

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

void setup() {
  Serial.begin(115200);
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
  button.loop();
  if (!isSleeping && (millis() - lastActivity > sleepTimeout)) {
    enterSleepMode();
  }
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
