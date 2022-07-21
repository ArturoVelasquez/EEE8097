/* 
 *  This sketch is ment to be use for testing the nrf24l01+ antena is working
 *  pin convetion in arduino uno is: 
 *  clk  green  13
 *  miso purple 12
 *  mosi blue   11
 *  csn  yellow 9
 *  ce   orange 10
 */

 #include <SPI.h>
 #include <nRF24L01.h>
 #include <RF24.h>
 
 #define CEPIN 10
 #define CSNPIN 9

 const byte nrf_address[6] = "00042";

 RF24 antena(CEPIN,CSNPIN);
 
 void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Anthena test runing ...");
  antena.begin();
  antena.setDataRate( RF24_250KBPS );
  antena.setPALevel(RF24_PA_MAX );
}

void loop() {
  connected();
  delay(5000);

}

void connected(){
  bool connectedToAntena;
  connectedToAntena=antena.isChipConnected();
  Serial.print("Antena functional: ");
  if(connectedToAntena){
    Serial.println("Working ok");
  }
  else{
    Serial.println("Not working");  
  }
    
}
