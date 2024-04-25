// Define pin connections
#define LEFT_SENSOR_PIN A0
#define LEFT_MOTOR_PIN 13

// Define sensor threshold values
#define SENSOR_THRESHOLD 500

void setup() {
  // Set motor pins as outputs
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  //Serial.begin(9600);
  
}

void loop() {
  // Read sensor values
  int leftSensorValue = analogRead(LEFT_SENSOR_PIN);
  //delay(1000);
  //Serial.println(leftSensorValue);
  if (leftSensorValue < SENSOR_THRESHOLD) {
    // Turn left
    digitalWrite(LEFT_MOTOR_PIN, HIGH);
  }
  else {
    // No line detected, stop
    digitalWrite(LEFT_MOTOR_PIN, LOW);
  }
}