#include <SoftwareSerial.h>
#include <Wire.h>

 
#define RE 5 //gray
#define DE 4 //purple
#define RO 2 //blue 
#define DI 3 //green
 
const byte NPKREQUEST[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x34, 0x0D};
const byte NREQUEST[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte PREQUEST[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte KREQUEST[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
const byte CHECKRESPONSE[] = {0x01,0x03, 0x02};
 
byte values[11];

int N,P,K;

SoftwareSerial mod(RO,DI);
 
void setup() {
  Serial.begin(115200);
  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  
  delay(500);
  Serial.println("NPK Sensor");
  Serial.print("Initializing");
  delay(3000);
}
 
void loop() {
  N = nitrogen();
  delay(250);
  P = phosphorus();
  delay(250);
  K = potassium();
  delay(250);
  print_values();
  delay(2000);
}

void print_values(){
  Serial.print("Nitrogen: ");
  Serial.print(N,DEC);
  Serial.println(" mg/kg");
  Serial.print("Phosphorous: ");
  Serial.print(P,DEC);
  Serial.println(" mg/kg");
  Serial.print("Potassium: ");
  Serial.print(K,DEC);
  Serial.println(" mg/kg");
  }
 
int nitrogen(){
  int reading;
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(NREQUEST,sizeof(NREQUEST))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<8;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.println(values[i],HEX);
    }
    Serial.println();
    if(CHECKRESPONSE[0] == values[0] && CHECKRESPONSE[1]== values[1] && CHECKRESPONSE[2]== values[2] ){
      Serial.println("Correct response format detected.");
      reading= int(values[3])+int(values[4]);
    }
    else{
      Serial.println("Incorrect reading");
      reading = 9999;
    }
  }
  return reading;
}
 
int phosphorus(){
  int reading;
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(PREQUEST,sizeof(PREQUEST))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<8;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.println(values[i],HEX);
    }
    Serial.println();
    if(CHECKRESPONSE[0] == values[0] && CHECKRESPONSE[1]== values[1] && CHECKRESPONSE[2]== values[2] ){
      Serial.println("Correct response format detected.");
      reading= int(values[3])+int(values[4]);
    }
    else{
      Serial.println("Incorrect reading");
      reading = 9999;
    }
  }
  return reading;
}
 
int potassium(){
  int reading;
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(KREQUEST,sizeof(KREQUEST))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<8;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.println(values[i],HEX);
    }
    Serial.println();
    if(CHECKRESPONSE[0] == values[0] && CHECKRESPONSE[1]== values[1] && CHECKRESPONSE[2]== values[2] ){
      Serial.println("Correct response format detected.");
      reading= int(values[3])+int(values[4]);
    }
    else{
      Serial.println("Incorrect reading");
      reading = 9999;
    }
  }
  return reading;
}
