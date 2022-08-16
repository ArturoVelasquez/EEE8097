  /* 
 *  This sketch is ment to be used as the static monitoring station
 *  nRF24l01+ convetion in arduino uno is: 
 *  clk  green  pin 13
 *  miso purple pin 12
 *  mosi blue   pin 11
 *  csn  yellow pin 9
 *  ce   orange pin 10
 *  
 *  Sensor are to be connected as follows
 *  DHT pin 7 
 *  Photodiode sensor pin A1
 *  Soil moisture pin A2 
 *  
 *  Actuator (Servo motor) is to be connected to pin 6
 *  
 *  In the comunication the firts 2 bytes  are the module identifier.
 *  This can be the combination of any two bytes. 
 *  This will identify the module in the database later. (Including the type of module).
 */
 
 #include <SPI.h>
 #include <nRF24L01.h>
 #include <RF24.h>

 #define CEPIN 10
 #define CSNPIN 9
 #define NRFCHANNEL 100
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

 const byte module_number[2]= {2,2};
 const byte nrf_address[6] = "1SNSR";
 char sensor_readings[32];
 
 
 int N=111,P=222,K=333;
 float meridian=44.44,parallel=55.55;
 int measurement_count = 1;

 RF24 antena(CEPIN,CSNPIN);

 message my_message{MODULETYPE,{module_number[0],module_number[1]},0.0,0.0,0,0,0};
 
void setup() {
  Serial.begin(115200);
  Serial.println("Starting module :");
  
  delay(1000);
  
  antena.begin();
  antena_connected();
  antena_setup();

  delay(1000);
}

void loop() {
  send_message();
  delay(5000);
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
  antena.setPayloadSize(32);
  antena.openWritingPipe(nrf_address);
  antena.stopListening();
}


void update_message(){
  my_message.parallel=parallel+measurement_count;
  my_message.meridian=meridian+measurement_count;
  my_message.nitrogen=N+measurement_count;
  my_message.potasium=K+measurement_count;
  my_message.phosphorus=P+measurement_count;
}

void send_message(){
  //radio.stopListening();
  //radio.write(&data,sizeof(data));
  Serial.print("mesage length: ");
  Serial.println(sizeof(my_message));
  if (antena.write(&my_message, sizeof(my_message))) {
    Serial.print("Message sent successfully. Retries="); 
    Serial.println(antena.getARC());
    Serial.print("Latitude: ");
    Serial.println(parallel);
    Serial.print("Longitude: ");
    Serial.println(meridian);
    Serial.print("Nitrogen: ");
    Serial.println(N);
    Serial.print("Phosphorus: ");
    Serial.println(P);
    Serial.print("Potasium: ");
    Serial.println(K);
    update_message();
  }
  else {
    Serial.print("Message not sent. Retries=");
    Serial.println(antena.getARC());
  }
}
