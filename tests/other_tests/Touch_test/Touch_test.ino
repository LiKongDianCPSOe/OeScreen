/* 作者：理空电CPSOe
   许可证：MIT
*/

#define TouchPin 12

void setup()
{
    Serial.begin(9600);
    pinMode(TouchPin,INPUT);
}

void loop()
{
    // put your main code here, to run repeatedly:
    Serial.print("pin");
    Serial.print(TouchPin);
    Serial.print(':');
    Serial.println(digitalRead(TouchPin));

    Serial.print("pin");
    Serial.print("14");
    Serial.print(':');
    Serial.println(digitalRead(14));  
    delay(100);
}
