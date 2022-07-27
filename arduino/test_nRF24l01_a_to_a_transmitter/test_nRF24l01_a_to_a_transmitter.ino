
/*
 * https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
 * https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
 * https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123/2
 */

// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   10
#define CSN_PIN  9

const byte slaveAddress[5] = {'R','x','A','A','A'};

const char moduleNumber[] = "Module 10";

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char iteration[10] = "Message 0";
char txNum = '0';
char dataToSend[32];


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second


void setup() {

    Serial.begin(9600);

    Serial.println("SimpleTx Starting");

    strcpy(dataToSend,moduleNumber);
    strcat(dataToSend," ");
    strcat(dataToSend,iteration);
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(4,5); // delay, count
    radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
  currentMillis = millis();
  if (currentMillis - prevMillis >= txIntervalMillis) {
    send();
    prevMillis = millis();
  }
}

//====================

void send() {

    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
    Serial.print("Data Sent ");
    Serial.print(dataToSend);
    if (rslt) {
        Serial.println("  Acknowledge received");
        updateMessage();
    }
    else {
        updateMessage();
        Serial.println("  Tx failed");
    }
    Serial.print("Mensaje: ");
    Serial.println(dataToSend);
}

//================

void updateMessage() {
        // so you can see that new data is being sent
   
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[19] = txNum;
}
