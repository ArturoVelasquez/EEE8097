/*
 * for 5V input:
 * soil moisture 180-450
 * light sensor 20-1020
 */
#define PIN_PHOTODIODE A1

int light_intensity = 0;

void setup() {
  pinMode(PIN_PHOTODIODE, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  light_intensity = analogRead(PIN_PHOTODIODE);
  Serial.print("Lightvalue is: ");
  Serial.println(light_intensity);
  delay(1000);
}
