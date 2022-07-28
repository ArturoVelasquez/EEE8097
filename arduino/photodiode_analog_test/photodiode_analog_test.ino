/*
 * for 5V input:
 * soil moisture 165(sumerged in water) - 455 (dry)
 * light intesity 1000(no light)-20(light)
 */
//#define PIN_PHOTODIODE A1 
#define PIN_PHOTODIODE A2

int light_intensity = 0;

void setup() {
  pinMode(PIN_PHOTODIODE, INPUT);
  //pinMode(PIN_PHOTODIODE, INPUT_PULLUP);
  delay(1000);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  light_intensity = analogRead(PIN_PHOTODIODE);
  Serial.print("Light value is: ");
  Serial.println(light_intensity);
  delay(1000);
}
