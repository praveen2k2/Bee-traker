#define dht11
#define ir A0
#define loadcell1
#define loadcell2
#define irPower
#define dhtPower
#define esppower

int beeCount=0;

void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

//get time from esp32
int gettime(){
  Serial.begin(9600);
  Serial.write('gt');
  int time=Serial.read();
  return time;
}

//get bee count 
int updateBeeCount(int beeCount){
   return beeCount++;
}
