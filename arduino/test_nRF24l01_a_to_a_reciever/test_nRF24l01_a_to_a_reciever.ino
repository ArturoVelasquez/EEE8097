// SimpleRx - the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   10
#define CSN_PIN  9

const byte thisSlaveAddress[5] = {'1','S','N','S','R'};

/* addres can also be declare as so: 
 * const byte thisSlaveAddress[6] = "RxAAA" 
 * Following the rules of strings in C++ a \0 character is added at the end. 
 */


RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[32]; // this must match dataToSend in the TX
bool newData = false;
bool connectedToAntena = false;
//===========

void setup() {

    Serial.begin(9600);
    Serial.println("Test Starting:");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MAX);
    //radio.setChannel(100);
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.startListening();
}

//=============

void loop() {
  connectedToAntena=radio.isChipConnected();
  if(connectedToAntena){
    getData();
    showData();  
  }
  else{
    Serial.println("antena seems to be in trouble, check the connection."); 
    delay(2000);
  }
}

//==============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
    }
}

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        newData = false;
    }
}
