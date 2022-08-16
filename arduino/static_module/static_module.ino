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

 const byte module_number[2]= {0,0};
 const byte nrf_address[6] = "R2RHO";
 char sensor_readings[32];
 

 
 bool soil_ok = 0;
 bool light_ok =0;
 bool temperature_ok=0;

 int soil = 0;
 int light = 0;
 float humidity = 0;
 float temperature =0;
 
 int soil_v = 0;
 int light_v = 0;
 float humidity_v = 0;
 float temperature_v =0;

 int measurement_count = 0;
 
 DHT dht(DHTPIN, DHTTYPE);
 
 Servo waterValve;
 int valve = 0;
 int valve_v = 0;
 RF24 antena(CEPIN,CSNPIN);
 message my_message{MODULETYPE,{module_number[0],module_number[1]},0.0,0.0,0,0,0};
 
void setup() {
  use_serial();
  Serial.println("Starting module :");
  
  delay(1000);
  
  antena.begin();
  antena_connected();
  antena_setup();

  delay(1000);
  
  pinMode(MOISTUREPIN, INPUT);
  soil_ok = check_analog(MOISTUREPIN);
  delay(5000);
  
  pinMode(PHOTODIODEPIN, INPUT_PULLUP);
  light_ok = check_analog(PHOTODIODEPIN);
  delay(5000);
  
  dht.begin();
  temperature_ok=check_digital();
  delay(1000);
  
  waterValve.attach(SERVOPIN);
  Serial.print("Valve PWM control attached to pin: ");
  Serial.print(SERVOPIN);
  Serial.println(". Make sure actuator is in the wright pin.");

}

void loop() {
  /*
   * TODO: Enviar mensaje a SCADA
   * TODO: Fuzzi controll para servo / en este punto puede ser on/off
   */
  if(soil_ok){
    soil += map(analogRead(MOISTUREPIN),165,455,100,0);
  }
  if(light_ok){
    light += map(analogRead(PHOTODIODEPIN),0,1023,100,0);
  }
  if(temperature_ok){
    humidity +=dht.readHumidity();
    temperature +=dht.readTemperature();
  }
  measurement_count += 1;
  
  water_controll();

  if(measurement_count > 30){
    update_message();
    send_message();
  }
  
  Serial.print("soil: ");
  Serial.println(soil);
  Serial.print("light: ");
  Serial.println(light);
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("temperature: ");
  Serial.println(temperature);
  delay(10000);
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

bool check_analog(int pin){
  if(pin==PHOTODIODEPIN){
    Serial.println("Preparing to check light sensor: please shine a bright light to sensor.");
    delay(5000);
    Serial.println("Checking now.");
    
    if(analogRead(pin)>950){
      Serial.print("LIGHT sensor not connected. ");
      Serial.println("Reset the system to add sensor and check again.");
      return(false);
  }
  }
  
  else{
    Serial.println("Preparing to check moisture sensor: please place sensor in water.");
    delay(5000);
    Serial.println("Checking now.");
    
    if(analogRead(pin)>200){
      Serial.print("SOIL MOISTURE Sensor not conected. ");
      Serial.println("Reset the system to add sensor and check again.");
      return(false);
    }
  }
  Serial.println("Sensor OK");
  return(true);
}

bool check_digital(){
  Serial.println("Preparing to check Humidity and temperature sensor:");
  delay(1000);
  Serial.println("Checking now.");
  if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) {
    Serial.println("TEMPERATURE & HUMDITY Sensor not conected.");
    Serial.println("Reset the system to add sensor and check again");
    return(false);
  }
  return(true);
}

void use_serial(){
  String confirmation="confirmation";
  Serial.begin(9600);
  Serial.println("Are you setting up in a computer? press any key followed by enter");
  delay(7000);
  if(Serial.available()>0){
    confirmation = Serial.readString();
    Serial.println("Use of serial confirmed" );
  }
  if(confirmation.equals("confirmation")){
      Serial.println("No further comunication will be provided");
      Serial.end();
  }
}

void water_controll(){
  int valve_position;
  int decide = (100-soil)*7+temperature*2+(100-humidity)+light;
  decide=map(decide, 0,1000,0,4);
  valve+=decide;
  switch(decide){
    case 0:
      valve_position=0;
      Serial.println("really wet: valve close!");
      break;
    case 1:
      Serial.println("Somewhat wet: quarter open.");
      valve_position=45;
      break;
    case 2:
      Serial.println("H2O is no harm: half open.");
      valve_position=90;
      break;
    case 3:
      Serial.println("Starting to feel thirsty: three quarters open.");
      valve_position=135;
      break;
    case 4:
      Serial.println("Im about to erode: fully open.");
      valve_position=180;  
      break;
  }
  waterValve.write(valve_position);  

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
