#include "DHT.h"

#define DHTPIN 2    // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);


#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
// OLED display TWI address
#define OLED_ADDR   0x3C
// Define pin connections


#define LEFT_SENSOR_PIN A0
#define LEFT_MOTOR_PIN 13
// Define sensor threshold values
#define SENSOR_THRESHOLD 500


Adafruit_SSD1306 display(-1);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {


  // Set motor pins as outputs
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  Serial.begin(9600);


  dht.begin();


  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 63, WHITE);
  display.drawPixel(127, 63, WHITE);

  // display a line of text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  //display.setCursor(27,30);
  

  // update display with all of the above graphics
  display.display();
}

void loop() {


  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);



  // put your main code here, to run repeatedly:
  // Read sensor values
  int leftSensorValue = analogRead(LEFT_SENSOR_PIN);
  delay(100);
  Serial.println(leftSensorValue);
  if (leftSensorValue < SENSOR_THRESHOLD) {
    // Turn left
    digitalWrite(LEFT_MOTOR_PIN, HIGH);
  }
  else {
    // No line detected, stop
    digitalWrite(LEFT_MOTOR_PIN, LOW);
  }

  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  //display.setCursor(5,46);
  //display.print( leftSensorValue );

  display.setCursor(27,1);
  display.print(" Bee Tracker ");
  display.setCursor(5,21);
  display.print("Temperature -     " );
  display.setCursor(5,36);
  display.print("Humidity -     " );
  display.setCursor(70,36);
  display.print( h );
  display.setCursor(88,21);
  display.print(t );
  display.display();
  display.clearDisplay();


}
