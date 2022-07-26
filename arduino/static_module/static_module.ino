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
 *  DHT pin 6 
 *  Photodiode sensor pin A1
 *  Soil moisture pin A2 
 *  
 *  Actuator (Servo motor) is to be connected to pin 6
 */
 
 #include <SPI.h>
 #include <nRF24L01.h>
 #include <RF24.h>
 #include <DHT.h>
 #include <Servo.h>

 #define CEPIN 10
 #define CSNPIN 9
 #define PHOTODIODEPIN A1
 #define MOISTUREPIN A2
 #define DHTPIN 7
 #define DHTTYPE DHT22   // DHT 22  (AM2302)
 #define SERVOPIN 6
 
 const byte nrf_address[6] = "00042";
 RF24 antena(CEPIN,CSNPIN);
 
 bool soil_ok = 0;
 bool light_ok =0;
 bool temperature_ok=0;

 int soil = 0;
 int light = 0;
 float humidity = 0;
 float temperature =0;
 

 DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(9600);
  Serial.println("Starting module :");
  delay(1000);
  antena.begin();
  antena.setDataRate( RF24_250KBPS );
  antena.setPALevel(RF24_PA_MAX );
  //connected();
  delay(1000);
  pinMode(PHOTODIODEPIN, INPUT_PULLUP);
  pinMode(MOISTUREPIN, INPUT);
  dht.begin();
  soil_ok = check_analog(MOISTUREPIN);
  delay(5000);
  light_ok = check_analog(PHOTODIODEPIN);
  delay(5000);
  temperature_ok=check_digital();

 
}

void loop() {
  if(soil_ok){
    soil = map(analogRead(MOISTUREPIN),160,450,100,0);
    light = map(analogRead(PHOTODIODEPIN),0,1023,100,0);
    humidity =dht.readHumidity();
    temperature=dht.readTemperature();
    Serial.print("soil: ");
    Serial.println(soil);
    Serial.print("light: ");
    Serial.println(light);
    Serial.println("humidity: ");
    Serial.println(humidity);
    Serial.println("temperature: ");
    Serial.println(temperature);
    delay(5000);
  }

}

void connected(){
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
  }
  return(true);
}
  
