#define ir 14 
#define loadcell1 2
#define loadcell2 3
#define dhtPower 4
#define loadCSwitch 5
#define espPower 6
#define irPower 7

int beeCount=0;
bool currentState;

void setup() {
  pinMode(ir, INPUT);
  pinMode(loadcell1, INPUT);
  pinMode(loadcell2, INPUT);
  pinMode(dhtPower, OUTPUT);
  pinMode(loadCSwitch, OUTPUT);
  pinMode(espPower, OUTPUT);
  pinMode(irPower, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Starting HiveLink....");
  currentState= digitalRead(ir);
  Serial.print(currentState);
}

void loop() {
  beeCount=updateBeeCount(beeCount);
 

}


//get bee count 
int updateBeeCount(int beeCount){
  bool newState=digitalRead(ir);
  if(currentState!=newState){
    beeCount ++;
    currentState=newState;

     //upadate here to increase the time 
    if(beeCount > 10){
    Serial.begin(9600);
    while (!Serial) {;}
    delay(10);
    Serial.print(1);
    beeCount=0;
    delay(1000);
   }
   }
   delay(10);//a delay is need to get readings correctly
  return beeCount;
}


