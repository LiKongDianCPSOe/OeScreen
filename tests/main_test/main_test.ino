/*
V1.1
作者：理空电CPSOe
此程序为OeScreen开发板的综合测试程序
将在屏幕上显示此开发板的信息
开源链接：https://gitee.com/CPSOe/OeScreen
许可证：MIT
*/

#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cn.pool.ntp.org", 60 * 60 * 8, 100);
TFT_eSPI tft = TFT_eSPI();

#define SHT30_Adr 0x44 //下拉为0x44，上拉为0x45
#define SDA_Pin 4
#define SCL_Pin 5
#define Touch_Pin 12 //触摸芯片输出引脚
#define Beep_Pin 16  //蜂鸣器引脚

const char *ssid = "<SSID>";             // wifi名称
const char *password = "<PASSWORD>";     // wifi密码
float SHT30_Data[2] = {0, 0};            // 摄氏度,湿度
int nameBlue = tft.color565(48, 64, 82); // CPSOe主题颜色

void PinInterrupt();
int SHT30_getData();

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA_Pin, SCL_Pin);
  pinMode(Touch_Pin, INPUT);
  attachInterrupt(Touch_Pin, PinInterrupt, RISING); //设置中断函数

  // tft屏幕初始化
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);
  tft.setCursor(0, 0, 1);

  // tft屏幕显示项目名，作者名
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("OeScreen", 5, 5);
  tft.setTextColor(nameBlue);
  tft.drawString("LiKongDian", 5, 23);
  tft.drawString("CPSOe", 5, 38);

  //连接wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("connecting");
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
}

void loop()
{
  //获取温度，湿度
  float *SHT30_Data_p = SHT30_Data;
  SHT30_getData(SHT30_Data_p);

  //获取时间
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  //串口输出数据
  Serial.print(ptm->tm_year + 1900); //年
  serial.print("/");
  Serial.print(ptm->tm_mon + 1); //月
  serial.print("/");
  Serial.println(ptm->tm_mday + 1); //日

  Serial.print("星期");
  Serial.println(ptm->tm_wday + 1); //星期

  Serial.print(ptm->tm_hour); //时
  Serial.print(":");
  Serial.print(ptm->tm_min); //分
  Serial.print(":");
  Serial.println(ptm->tm_sec); //秒

  Serial.print(SHT30_Data[0]); //温度
  Serial.println("℃");
  Serial.print(SHT30_Data[1]); //湿度
  Serial.println("%RH");

  //将浮点型数据转换为字符串
  char degTemp[7];
  char humidity[7];
  dtostrf(SHT30_Data[0], 3, 2, degTemp);
  dtostrf(SHT30_Data[1], 3, 2, humidity);

  // tft屏幕显示温度
  tft.setTextColor(TFT_RED);
  tft.setTextSize(1);
  tft.drawString(degTemp, 5, 55);
  tft.drawString("DegC", 45, 55);

  // tft屏幕显示湿度
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  tft.drawString(humidity, 5, 70);
  tft.drawString("%RH", 45, 70);

  delay(300);

  //刷新屏幕
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString(degTemp, 5, 55);
  tft.drawString(humidity, 5, 70);
}

ICACHE_RAM_ATTR void PinInterrupt() //触摸中断
{
  if (digitalRead(Touch_Pin))
  {
    Serial.println("TouchInterrupt");
    tft.drawFastVLine(40, 88, 128, nameBlue);
    analogWrite(Beep_Pin, 513);
    analogWriteFreq(440);
    delay(100);
    tft.drawFastVLine(40, 88, 128, TFT_BLACK);
    analogWrite(Beep_Pin, 0);
  }
}

int SHT30_getData(float *SHT30_Output) //传入温度及湿度的float类型数组指针，元素0为温度，元素1为湿度
{
  unsigned int SHT30_Request[6];
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
    SHT30_Request[0] = Wire.read();
    SHT30_Request[1] = Wire.read();
    SHT30_Request[2] = Wire.read();
    SHT30_Request[3] = Wire.read();
    SHT30_Request[4] = Wire.read();
    SHT30_Request[5] = Wire.read();
  }
  else
  {
    return 0;
  }
  float r_degTemp = ((((SHT30_Request[0] * 256.0) + SHT30_Request[1]) * 175) / 65535.0) - 45;
  float r_humidity = ((((SHT30_Request[3] * 256.0) + SHT30_Request[4]) * 100) / 65535.0);
  *SHT30_Output = r_degTemp;
  *(SHT30_Output + 1) = r_humidity;
  return 1;
}