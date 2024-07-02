//#include "DHT.h"
#include <DHT11.h>
#include <ArduinoJson.h>

//define all pins
#define DHTPIN 4  // Digital pin connected to the DHT sensor
#define ir 14 
#define loadcell1 2
#define loadcell2 3
#define loadCSwitch 5
#define espPower 6
#define irPower 7
#define ESP32_GPIO_WAKEUP 1
//#define DHTTYPE DHT11
//DHT dht(DHTPIN, DHTTYPE);
DHT11 dht11(4);

int beeCount = 0;
bool currentState;
unsigned long lastUpdate;

void setup() {
  pinMode(ir, INPUT);
  pinMode(loadcell1, INPUT);
  pinMode(loadcell2, INPUT);
  pinMode(loadCSwitch, OUTPUT);
  pinMode(espPower, OUTPUT);
  pinMode(irPower, OUTPUT);//currently use for indicator
  Serial.begin(9600);
  Serial.println();
  powerUp();
  delay(1000);
  //dht.begin();
  currentState= digitalRead(ir);

}

//use the main loop only for functions which should run on every cycle
void loop() {
  updateBeeCount(beeCount);
  update(lastUpdate,beeCount);
}

//get bee count 
void updateBeeCount(int &beeCount){
  bool newState=digitalRead(ir);
  if(currentState!=newState){
    beeCount ++;
    currentState=newState;
    delay(100);
   }
   delay(10);//a delay is need to get readings correctly
  return ;
}
void update(unsigned long &lastUpdate,int &beeCount){
  if(millis()-lastUpdate>3600000){
    int h, t;
    float weight;
    dhtdata(h,t);
    sendData(beeCount,h,t,weight);
    beeCount=0;
    lastUpdate=millis();
  }
}
//function for send data with esp
void sendData(int beeCount,int h,int t, float weight){

  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["count"] = beeCount;
  doc["weight"] = weight;

  // Serialize JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);
  delay(5000);
  // Send JSON string over Serial
  Serial.println(jsonString);
}

//sensors management 
void dhtdata(int &h, int &t) {
  // Wait a few seconds between measurements.
  digitalWrite(espPower,HIGH);
  delay(10000);
  
  h = dht11.readHumidity();
  t = dht11.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    // Set values to NaN to indicate failure
    h = t = NAN;
    return;
  }
  digitalWrite(espPower,LOW);
}



void readWeight(float &weight) {
  // Read weight from sensor
  // Example code to read weight from a load cell sensor
  // Replace with your actual weight sensor code
  int rawValue = analogRead(loadcell1);
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
  digitalWrite(irPower,HIGH); 
  delay(200);
  digitalWrite(irPower,LOW);
  delay(50);
  digitalWrite(irPower,HIGH); 
  delay(100);
  digitalWrite(irPower,LOW);
  delay(50);
  digitalWrite(irPower,HIGH); 
  delay(50);
  digitalWrite(irPower,LOW);
  delay(1000);

  if(digitalRead(loadcell1)==HIGH){
    digitalWrite(irPower,HIGH);
    delay(1000);
    digitalWrite(irPower,LOW);
  }
  while(digitalRead(loadcell1)==HIGH){
    digitalWrite(espPower,HIGH);
    if(digitalRead(ir)==HIGH)
    {digitalWrite(irPower,HIGH);}
    else{digitalWrite(irPower,LOW);delay(500);}
    delay(10);
    if(millis()-lastUpdate>20000){
    float weight;
    int temperature=0;
    int humidity=0;
    humidity = dht11.readHumidity();
    if (humidity != DHT11::ERROR_CHECKSUM && humidity != DHT11::ERROR_TIMEOUT) {
      temperature = dht11.readTemperature();
      if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {;
      } else {
        onBoardLED();
      }
    } else {
        onBoardLED();
        humidity=0;
    }
    sendData(0,humidity,temperature,weight);
    beeCount=0;
    lastUpdate=millis();
  }
  }
  int humidity = dht11.readHumidity();

    // Check the result of the reading.
    // If there's no error, print the humidity value.
    // If there's an error, print the appropriate error message.
    if (humidity != DHT11::ERROR_CHECKSUM && humidity != DHT11::ERROR_TIMEOUT) {
        
    } else {
        onBoardLED();
    }
  }



//error management part

void onBoardLED(){
  // desply the funtion running ,data communication and some other data from a LED beeps. For error mannaging.
  digitalWrite(irPower,HIGH); 
  delay(400);
  digitalWrite(irPower,LOW);
  delay(100);
  digitalWrite(irPower,HIGH); 
  delay(100);
  digitalWrite(irPower,LOW);
  delay(100);
  digitalWrite(irPower,HIGH); 
  delay(100);
  digitalWrite(irPower,LOW);
  delay(400);
  }


void errorDiagnosis(){
  //try to detect any posible errors in the system
  //unable to communicate with esp
  //not enought power : indentify from power down of ic when tring to power up a sensor.disconnect ESP after some time in a periodic way
  //water leak : unexpected signal from disconnected pins
  //continuvity check
  }
