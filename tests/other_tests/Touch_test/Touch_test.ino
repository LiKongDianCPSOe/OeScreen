/* 作者：理空电CPSOe
   许可证：MIT
*/

#define TouchPin 12

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.print("pin");
  Serial.print(TouchPin);
  Serial.print(':');
  Serial.println(digitalRead(TouchPin));
  delay(100);
}
