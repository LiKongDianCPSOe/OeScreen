/*
V1.1-1.0
作者：理空电CPSOe
此程序为OeScreen开发板的综合测试程序
连接wifi后，将在屏幕上显示读取的温湿度及时间
开源链接：https://gitee.com/CPSOe/OeScreen
许可证：MIT
*/

#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
// #include <WiFi.h> // for WiFi shield
// #include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.ntsc.ac.cn", 60 * 60 * 8, 100); // ntp.ntsc.ac.cn为ntp服务器地址，60*60*8设置时区(中国东八时区，北京时间)，最后一个参数设置更新最小间隔
TFT_eSPI tft = TFT_eSPI();

// #define SHT30Adr 0x44 // 下拉为0x44，上拉为0x45
#define SDAPin 4    // SDA引脚(硬件)
#define SCLPin 5    // SCL引脚(硬件)
#define touchPin 12 // 触摸芯片输出引脚
#define beepPin 16  // 蜂鸣器引脚
#define backgroundColor TFT_BLACK

const char *ssid = "SSID";                 // wifi名称
const char *password = "PASSWORD";         // wifi密码
float SHT30Data[2] = {0.0, 0.0};           // 摄氏度,湿度浮点型数据
int CPSOeColor = tft.color565(48, 64, 82); // CPSOe主题颜色 rgb(48,64,82)

ICACHE_RAM_ATTR void TouchInterrupt(); // 触摸中断函数
// bool SHT30GetData(float *SHT30Output); // 获取SHT30的数据   传入温度及湿度的float类型数组指针，元素0为温度，元素1为湿度

void setup()
{
    // 通讯初始化
    Serial.begin(115200);
    // Wire.begin(SDAPin, SCLPin);

    // tft屏幕初始化
    tft.init();
    pinMode(touchPin, INPUT);
    tft.fillScreen(backgroundColor);
    tft.setRotation(2);
    tft.setCursor(0, 0, 1);

    // 触摸初始化 注意:由于触摸引脚使用硬件MISO(数据输入，本项目使用的tft屏幕没有MISO引脚),触摸引脚初始化放在tft.init()后,否则触摸引脚无法读取及中断
    pinMode(touchPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(touchPin), TouchInterrupt, RISING); // 设置中断

    // tft屏幕显示项目名，作者名
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("OeScreen", 5, 5);
    tft.setTextColor(CPSOeColor);
    tft.drawString("LiKongDian", 5, 23);
    tft.drawString("CPSOe", 5, 38);

    // 连接wifi
    WiFi.begin(ssid, password);
    Serial.print("connecting");
    tft.drawString("connecting", 5, 65);
    for (int i = 5; WiFi.status() != WL_CONNECTED; i += 6)
    {
        delay(700);
        tft.drawString(".", i, 82);
        Serial.print(".");
    }
    tft.setTextColor(backgroundColor);
    tft.drawString("connecting", 5, 65);
    tft.fillRect(5, 82, 123, 20, backgroundColor);
    Serial.print("connected\nto\n");
    Serial.println(ssid);
    timeClient.begin();
}

void loop()
{
    // 获取温度，湿度
    // float *SHT30Data_p = SHT30Data;
    // SHT30GetData(SHT30Data_p);

    // 获取时间
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    // 串口输出
    Serial.print(ptm->tm_hour); // 时
    Serial.print(":");
    Serial.print(ptm->tm_min); // 分
    Serial.print(":");
    Serial.println(ptm->tm_sec); // 秒

    // Serial.print(SHT30Data[0]); // 温度
    // Serial.println("℃");
    // Serial.print(SHT30Data[1]); // 湿度
    // Serial.println("%RH");

    // 将浮点型数据转换为字符串
    char SHT30DegC[7];
    char SHT30Humidity[7];
    dtostrf(SHT30Data[0], 3, 2, SHT30DegC);
    dtostrf(SHT30Data[1], 3, 2, SHT30Humidity);

    // 刷新温湿度显示(填充背景颜色)
    tft.fillRect(5, 57, 70, 25, backgroundColor);

    // tft屏幕显示温度
    tft.setTextColor(TFT_RED);
    tft.setTextSize(1);
    tft.drawString(SHT30DegC, 5, 57);
    tft.drawString("DegC", 41, 57);

    // tft屏幕显示湿度
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString(SHT30Humidity, 5, 72);
    tft.drawString("%RH", 41, 72);

    // 刷新时间显示(填充背景颜色)
    tft.fillRect(5, 90, 82, 16, backgroundColor);

    // 转换时间数据为字符串，若小于10，则在前面添加“0”
    char charHours[4];
    char charMinutes[4];
    char charSeconds[4];
    String strHours;
    String strMinutes;
    String strSeconds;

    itoa(ptm->tm_hour, charHours, 10);
    if (ptm->tm_hour < 10)
    {
        strHours = '0';
        strHours += charHours;
    }
    else
    {
        strHours = charHours;
    }

    itoa(ptm->tm_min, charMinutes, 10);
    if (ptm->tm_min < 10)
    {
        strMinutes = '0';
        strMinutes += charMinutes;
    }
    else
    {
        strMinutes = charMinutes;
    }

    itoa(ptm->tm_sec, charSeconds, 10);
    if (ptm->tm_sec < 10)
    {
        strSeconds = '0';
        strSeconds += charSeconds;
    }
    else
    {
        strSeconds = charSeconds;
    }

    // tft屏幕显示时间
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString(strHours, 5, 90);
    tft.drawString(":", 26, 90);
    tft.drawString(strMinutes, 35, 90);
    tft.drawString(":", 56, 90);
    tft.drawString(strSeconds, 65, 90);

    delay(300);
}

ICACHE_RAM_ATTR void TouchInterrupt() // 触摸中断
{
    analogWrite(beepPin, 512);
    analogWriteFreq(698);
    tft.fillCircle(119, 151, 2, TFT_WHITE);
    delayMicroseconds(100000);
    tft.fillCircle(119, 151, 4, TFT_WHITE);
    delayMicroseconds(110000);
    tft.fillRect(115, 147, 10, 10, backgroundColor);
    analogWrite(beepPin, 0);
}
/*
bool SHT30GetData(float *SHT30Output) // 传入温度及湿度的float类型数组指针，元素0为温度，元素1为湿度
{
    unsigned int RequestData[6];
    Wire.beginTransmission(SHT30Adr);

    Wire.write(0x2C);
    Wire.write(0x06);

    Wire.endTransmission();

    delay(15);

    Wire.requestFrom(SHT30Adr, 6);

    // 读取6字节的数据
    // 这六个字节分别为：温度8位高数据，温度8位低数据，温度8位CRC校验数据
    //                湿度8位高数据，湿度8位低数据，湿度8位CRC校验数据
    if (Wire.available() == 6)
    {
        RequestData[0] = Wire.read();
        RequestData[1] = Wire.read();
        RequestData[2] = Wire.read();
        RequestData[3] = Wire.read();
        RequestData[4] = Wire.read();
        RequestData[5] = Wire.read();
    }
    else
    {
        return 0;
    }
    float RequestDeg = ((((RequestData[0] * 256.0) + RequestData[1]) * 175) / 65535.0) - 45;
    float RequestHumidity = ((((RequestData[3] * 256.0) + RequestData[4]) * 100) / 65535.0);
    *SHT30Output = RequestDeg;
    *(SHT30Output + 1) = RequestHumidity;
    return 1;
}
*/