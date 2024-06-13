#define dht11
#define ir
#define loadcell1
#define loadcell2
#define irPower
#define dhtPower
#define esppower

int beeCount;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

int gettime(){
  Serial.begin(9600);
  Serial.write('gt');
  int time=Serial.read();
}
