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
 #include <DHT.h>
 #include <Servo.h>

 #define CEPIN 10
 #define CSNPIN 9
 #define NRFCHANNEL 100
 #define PHOTODIODEPIN A1
 #define MOISTUREPIN A2
 #define DHTPIN 7
 #define DHTTYPE DHT22   // DHT 22  (AM2302)
 #define SERVOPIN 6
 #define MODULETYPE 0x01
 
 struct message {
  byte module_type;
  byte module_number[2];
  float temperature;
  float humidity;
  int soil;
  int light;
  int valve;
  };

 const byte module_number[2]= {1,1};
 const byte nrf_address[6] = "1SNSR";
 char sensor_readings[32];
 
 RF24 antena(CEPIN,CSNPIN);
 
 int soil_v = 0;
 int light_v = 0;
 float humidity_v = 0;
 float temperature_v =0;

 int measurement_count = 0;
 
 DHT dht(DHTPIN, DHTTYPE);
 
 Servo waterValve;
 int valve_v = 0;

 message my_message{MODULETYPE,{module_number[0],module_number[1]},0.0,0.0,0,0,0};
 
void setup() {
  Serial.begin(115200);
  Serial.println("Starting module :");
  
  delay(1000);
  
  antena.begin();
  antena_connected();
  antena_setup();

  delay(1000);
  
  soil_v=70;
  light_v=50;
  temperature_v=18.55;
  humidity_v=55.23;
}

void loop() {

  
  update_message();
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
  temperature_v = temperature_v + measurement_count;
  humidity_v = humidity_v + measurement_count;
  soil_v = (int) soil_v + measurement_count;
  light_v = (int) light_v + measurement_count;
  valve_v = (int) valve_v + measurement_count;
  
  my_message.temperature=temperature_v;
  my_message.humidity=humidity_v;
  my_message.light=light_v;
  my_message.soil=soil_v;
  my_message.valve=valve_v;

  //measurement_count +=1;
  
}

void send_message(){
  //radio.stopListening();
  //radio.write(&data,sizeof(data));
  Serial.print("mesage length: ");
  Serial.println(sizeof(my_message));
  if (antena.write(&my_message, sizeof(my_message))) {
    Serial.print("Message sent successfully. Retries="); 
    Serial.println(antena.getARC());
    Serial.print("soil: ");
    Serial.println(soil_v);
    Serial.print("light: ");
    Serial.println(light_v);
    Serial.print("humidity: ");
    Serial.println(humidity_v);
    Serial.print("temperature: ");
    Serial.println(temperature_v);
  }
  else {
    Serial.print("Message not sent. Retries=");
    Serial.println(antena.getARC());
  }
}
