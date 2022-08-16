/*
 * TODO: setup the message and transmission
 */
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
//#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CEPIN 10
#define CSNPIN 9
#define NRFCHANNEL 100

#define RE 5 //gray
#define DE 4 //purple
#define RO 2 //blue 
#define DI 3 //green

#define GPSSERIALRX 8
#define GPSSERIALTX 7

#define LED1 13
#define LED2 12

#define MODULETYPE 0x02

struct message {
  byte module_type;
  byte module_number[2];
  float parallel;
  float meridian;
  int nitrogen;
  int potasium;
  int phosphorus;
  };

const byte NREQUEST[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte PREQUEST[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte KREQUEST[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
const byte CHECKRESPONSE[] = {0x01,0x03, 0x02};

const byte module_number[2]= {0,0};
const byte nrf_address[6] = "R2RHO";

byte soil_values[8];

int N=9999,P=9999,K=9999;
//int satellites_found,quality;
bool sent=false;
bool data_fixed;
float parallel,meridian;
uint32_t timer = millis();

SoftwareSerial gps_serial(GPSSERIALRX,GPSSERIALTX);
SoftwareSerial npk_serial(RO,DI);
Adafruit_GPS GPS(&gps_serial);
RF24 antena(CEPIN,CSNPIN);
message my_message{MODULETYPE,{module_number[0],module_number[1]},0.0,0.0,0,0,0};


void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Starting GPS");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  
  //Serial.println("Starting NPK Sensor");
  npk_serial.begin(9600);
  delay(1000);

  antena.begin();
  antena_setup();
  delay(1000);
  
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  delay(1000);
}

void loop() {
/*
 * The number of satelites or wanted aquiracy can be added in this statement
 */
  if(!data_fixed){
    get_gps_reading();
  }
  else{
    digitalWrite(LED1,HIGH);
    //Serial.println("Location aquire, procede to insert NPK-sensor into soil");
    if(N!=9999){
      N = npk_read(NREQUEST,sizeof(NREQUEST));
      delay(250);
    }
    else if(K!=9999){
      K = npk_read(KREQUEST,sizeof(KREQUEST));
      delay(250);
    }
    else if(P!=9999){
      P = npk_read(PREQUEST,sizeof(PREQUEST));
      delay(250);
    }
    else if(!sent){
      digitalWrite(LED2,HIGH);
      //Serial.println("NPK reading aquire, sending data to main board");
      update_message();
      send_message();
    }
  }

}

void get_gps_reading(){
  GPS.read();
   if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  if (millis() - timer > 200) {
    timer=millis();
    //satellites_found=GPS.satellites;
    //quality = GPS.fixquality;
    data_fixed =GPS.fix;
    parallel= GPS.latitudeDegrees;
    meridian = GPS.longitudeDegrees;
  }
}

int npk_read(const byte request[8],int request_size){
  int reading;
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(npk_serial.write(request,request_size)==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<request_size;i++){
    soil_values[i] = npk_serial.read();
    //Serial.println(values[i],HEX);
    }
    //Serial.println();
    if(CHECKRESPONSE[0] == soil_values[0] && CHECKRESPONSE[1]== soil_values[1] && CHECKRESPONSE[2]== soil_values[2] ){
      //Serial.println("Correct response format detected.");
      reading= int(soil_values[3])+int(soil_values[4]);
    }
    else{
      //Serial.println("Incorrect reading");
      reading = 9999;
    }
  }
  return reading;
}


void antena_setup(){
  antena.enableDynamicPayloads();
  antena.setDataRate(RF24_250KBPS);
  antena.setPALevel(RF24_PA_MAX);
  antena.setChannel(NRFCHANNEL);
  antena.setDataRate(RF24_250KBPS);
  antena.setCRCLength(RF24_CRC_16);
  antena.openWritingPipe(nrf_address);
  antena.stopListening();
}

void update_message(){
  my_message.parallel=parallel;
  my_message.meridian=meridian;
  my_message.nitrogen=N;
  my_message.potasium=K;
  my_message.phosphorus=P;
}

void send_message(){
  sent =antena.write(&my_message, sizeof(my_message));
  if (sent) {
    //Serial.print("Message sent successfully. Retries="); 
    //Serial.println(antena.getARC());
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    delay(500);
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    delay(1000);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    delay(500);
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
  }
}

void show_readings(){
  Serial.print("Location: ");
  Serial.print(parallel,6);
  Serial.print(", ");
  Serial.println(meridian,6);
  //Serial.print("Satellites : ");
  //Serial.println(satellites_found);
  Serial.print("Fixed : ");
  Serial.print(data_fixed);
  //Serial.print(" Quality : ");
  //Serial.println(quality);
}
