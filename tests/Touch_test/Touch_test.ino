#define TouchPin 12

void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(TouchPin));
  delay(100);
}
