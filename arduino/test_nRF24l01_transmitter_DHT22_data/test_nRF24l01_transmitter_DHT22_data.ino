/*
* Modified Version of:
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>

#define CE_PIN  10
#define CSN_PIN 9
#define DHT_PIN 8
#define DHT_TYPE DHT22

RF24 radio(CE_PIN, CSN_PIN);
DHT dht(DHT_PIN, DHT_TYPE);

const byte address[6] = "00001";
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup() {
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  dht.begin();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  Serial.print("humidity: ");
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp);
  String hum_string =String(hum,1);
  String temp_string =String(temp,1);
  String texto = "s1,t " + temp_string +",h "+ hum_string ;
  char text[32];
  texto.toCharArray(text,32);
  Serial.println(text);
  radio.write(&text, sizeof(text));
  delay(1000);
  connected();
}
void connected(){
  bool connectedToAntena;
  connectedToAntena=radio.isChipConnected();
  Serial.print("Is antena functional ");
  Serial.println(connectedToAntena);  
}
