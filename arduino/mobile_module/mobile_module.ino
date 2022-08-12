/*
 * TODO: setup the message and transmission
 */
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
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

const byte NPKREQUEST[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x34, 0x0D};
const byte NREQUEST[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte PREQUEST[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte KREQUEST[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
const byte CHECKRESPONSE[] = {0x01,0x03, 0x02};

const byte module_number[2]= {0,0};
const byte nrf_address[6] = "R2RHO";

byte soil_values[11];

int N,P,K;
int satellites_found,quality;
bool data_fixed;
bool n_aquire,p_aquire,k_aquire;
float parallel,meridian;
uint32_t timer = millis();

SoftwareSerial gps_serial(GPSSERIALRX,GPSSERIALTX);
SoftwareSerial npk_serial(RO,DI);
Adafruit_GPS GPS(&gps_serial);
RF24 antena(CEPIN,CSNPIN);
message my_message{MODULETYPE,{module_number[0],module_number[1]},0.0,0.0,0,0,0};


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting GPS");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  
  Serial.println("Starting NPK Sensor");
  npk_serial.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  delay(3000);
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
    Serial.println("Location aquire, procede to insert NPK-sensor into soil");
    if(!n_aquire){
      N = npk_read(NREQUEST,sizeof(NREQUEST),n_aquire);
      delay(250);
    }
    else if(!k_aquire){
      K = npk_read(KREQUEST,sizeof(KREQUEST),k_aquire);
      delay(250);
    }
    else if(!p_aquire){
      P = npk_read(PREQUEST,sizeof(PREQUEST),p_aquire);
      delay(250);
      digitalWrite(LED2,HIGH);
      Serial.println("NPK reading aquire, sending data to main board");
    }
    else{
      return;
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
    satellites_found=GPS.satellites;
    quality = GPS.fixquality;
    data_fixed =GPS.fix;
    parallel= GPS.latitudeDegrees;
    meridian = GPS.longitudeDegrees;
  }
}

int npk_read(byte request[], int request_size, bool succsefull_measurement){
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
      succsefull_measurement=true;
    }
    else{
      //Serial.println("Incorrect reading");
      reading = 9999;
      succsefull_measurement=false;
    }
  }
  return reading;
}

void antena_connected(){
  bool connectedToAntena;
  connectedToAntena=antena.isChipConnected();
  Serial.println("Cheking for transmitter:");
  while (!connectedToAntena){
    Serial.println("nRF24l01+ not working");
    Serial.println("Check the antenna connection, 10 seconds until next test.");
    delay(10000);
  }
  if(connectedToAntena){
    Serial.println("nRF24l01+ working ok.");
  }  
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
  temperature_v = temperature/measurement_count;
  humidity_v = humidity/measurement_count;
  soil_v = (int) soil/measurement_count;
  light_v = (int) light/measurement_count;
  valve_v = (int) valve/measurement_count;
  
  my_message.temperature=temperature_v;
  my_message.humidity=humidity_v;
  my_message.light=light_v;
  my_message.soil=soil_v;

  humidity=0.0;
  temperature=0.0;
  soil=0;
  light=0;
  valve= 0;
  measurement_count = 0;
  
}

void send_message(){
  //radio.stopListening();
  //radio.write(&data,sizeof(data));
  if (antena.write(&my_message, sizeof(my_message))) {
    Serial.print("Message sent successfully. Retries="); 
    Serial.println(antena.getARC());
  }
  else {
    Serial.print("Message not sent. Retries=");
    Serial.println(antena.getARC());
  }
}

void show_readings(){
  Serial.print("Location: ");
  Serial.print(parallel,6);
  Serial.print(", ");
  Serial.println(meridian,6);
  Serial.print("Satellites : ");
  Serial.println(satellites_found);
  Serial.print("Fixed : ");
  Serial.print(data_fixed);
  Serial.print(" Quality : ");
  Serial.println(quality);
}
