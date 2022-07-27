/*
* Modified Version of:
* Arduino Wireless Communication Tutorial
*     Example 1 - Reciever Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  10
#define CSN_PIN 9

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  connected();
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}

void connected(){
  bool connectedToAntena;
  connectedToAntena=radio.isChipConnected();
  Serial.print("Is antena functional ");
  Serial.println(connectedToAntena);  
}
