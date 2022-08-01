
#define PIN_PHOTODIODE 7

int light_intensity = 0;

void setup() {
  pinMode(PIN_PHOTODIODE, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  light_intensity = digitalRead(PIN_PHOTODIODE);
  Serial.print("Lightvalue is: ");
  Serial.println(light_intensity);
  delay(100);
}
