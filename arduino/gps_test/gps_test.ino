#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#define GPSSERIALRX 8
#define GPSSERIALTX 7

int satellites_found;
int quality;
bool data_fixed;
float parallel;
float meridian;
uint32_t timer = millis();

SoftwareSerial gps_serial(GPSSERIALRX,GPSSERIALTX);
Adafruit_GPS GPS(&gps_serial);
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting GPS");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
}

void loop() {
  GPS.read();
   if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  if (millis() - timer > 2000) {
    timer=millis();
    satellites_found=(int)GPS.satellites;
    quality = GPS.fixquality;
    data_fixed =GPS.fix;
    parallel= GPS.latitudeDegrees;
    meridian = GPS.longitudeDegrees;
    
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
}
