/* 作者：理空电CPSOe
   许可证：MIT
*/

#include <Wire.h>

#define SHT30_Adr 0x44
#define SDApin 4
#define SCLpin 5

void setup()
{
  Wire.begin(SDApin, SCLpin);
  Serial.begin(115200);

  delay(100);
}

void loop()
{
  unsigned int SHT30_Data[6];
  Wire.beginTransmission(SHT30_Adr);

  Wire.write(0x2C);
  Wire.write(0x06);

  Wire.endTransmission();

  delay(15);

  Wire.requestFrom(SHT30_Adr, 6);

  //读取6字节的数据
  //这六个字节分别为：温度8位高数据，温度8位低数据，温度8位CRC校验数据
  //               湿度8位高数据，湿度8位低数据，湿度8位CRC校验数据
  if (Wire.available() == 6)
  {
    SHT30_Data[0] = Wire.read();
    SHT30_Data[1] = Wire.read();
    SHT30_Data[2] = Wire.read();
    SHT30_Data[3] = Wire.read();
    SHT30_Data[4] = Wire.read();
    SHT30_Data[5] = Wire.read();
  }

  float cTemp = ((((SHT30_Data[0] * 256.0) + SHT30_Data[1]) * 175) / 65535.0) - 45;
  float humidity = ((((SHT30_Data[3] * 256.0) + SHT30_Data[4]) * 100) / 65535.0);

  Serial.print(humidity);
  Serial.println(" %RH");
  Serial.print(cTemp);
  Serial.println("°C");

  delay(85);
}
