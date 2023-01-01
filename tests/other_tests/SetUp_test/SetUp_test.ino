/*
V1.1-1.0
作者：理空电CPSOe
此程序为OeScreen开发板的冷启动(加载动画)测试程序
将开发版连接电源后，将在屏幕上显示加载画面，同时连接wifi,连接wifi后在串口显示时间，日期
开源链接：https://gitee.com/CPSOe/OeScreen
许可证：MIT
*/

#include <pgmspace.h>
#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.ntsc.ac.cn", 60 * 60 * 8, 100);
// ntp.ntsc.ac.cn为ntp服务器地址，60*60*8设置时区(中国东八时区，北京时间)，最后一个参数设置更新最小间隔

TFT_eSPI tft = TFT_eSPI();

#define touchPin 12 //触摸芯片输出引脚
#define beepPin 16  //蜂鸣器引脚
//#define backgroundColor TFT_BLACK

const char *ssid = "SSID";                       // wifi名称
const char *password = "PASSWORD";               // wifi密码
const int CPSOeColor = tft.color565(48, 64, 82); // CPSOe主题颜色 rgb(48,64,82)
const int backgroundColor = tft.color565(212, 212, 212);

const unsigned char OeScreenLogo[] PROGMEM = {
    0x0F, 0xC0, 0x00, 0x01, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xE0, 0x00,
    0x03, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x70, 0x00, 0x03, 0x87, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x38, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x18, 0x1F, 0x07, 0x00, 0x07, 0xE0, 0xDC, 0x3E, 0x01, 0xF0, 0x77,
    0xC0, 0xE0, 0x1C, 0x7F, 0x87, 0x00, 0x0F, 0xF0, 0xFC, 0xFF, 0x07, 0xF8, 0x7F, 0xE0, 0xE0, 0x1C,
    0x61, 0xC3, 0xE0, 0x0E, 0x38, 0xE0, 0xC3, 0x86, 0x1C, 0x7C, 0xF0, 0xE0, 0x1C, 0xE0, 0xC1, 0xF8,
    0x1C, 0x18, 0xC1, 0xC1, 0x8E, 0x0C, 0x78, 0x70, 0xE0, 0x1C, 0xC0, 0xE0, 0x7E, 0x18, 0x00, 0xC1,
    0x81, 0xCC, 0x0E, 0x70, 0x70, 0xE0, 0x1C, 0xFF, 0xE0, 0x07, 0x18, 0x00, 0xC1, 0xFF, 0xCF, 0xFE,
    0x70, 0x70, 0xE0, 0x1C, 0xFF, 0xC0, 0x03, 0x98, 0x00, 0xC1, 0xFF, 0x8F, 0xFC, 0x70, 0x70, 0x60,
    0x18, 0xC0, 0x06, 0x03, 0x98, 0x00, 0xC1, 0x80, 0x0C, 0x00, 0x70, 0x70, 0x70, 0x38, 0xE0, 0x07,
    0x03, 0x1C, 0x18, 0xC1, 0xC0, 0x0E, 0x00, 0x70, 0x70, 0x38, 0x70, 0x71, 0xC7, 0x87, 0x0E, 0x38,
    0xC0, 0xE3, 0x87, 0x1C, 0x70, 0x70, 0x1F, 0xE0, 0x7F, 0x83, 0xFE, 0x0F, 0xF0, 0xC0, 0xFF, 0x07,
    0xF8, 0x70, 0x70, 0x0F, 0xC0, 0x1F, 0x00, 0xF8, 0x03, 0xE0, 0xC0, 0x3E, 0x01, 0xF0, 0x70, 0x70}; // OeScreen标志

// ICACHE_RAM_ATTR void TouchInterrupt(); //触摸中断函数

void showOeSLogo(int x, int y);
void LineLoad(bool loadDir, bool setCase);

void setup()
{
    //通讯初始化
    Serial.begin(115200);

    // tft屏幕初始化
    tft.init();
    tft.fillScreen(backgroundColor);
    tft.setRotation(3);
    Serial.println("tftStart");

    //显示项目Logo
    showOeSLogo(31, 48);
    Serial.println("showLogo");

    //触摸初始化 注意:由于触摸引脚使用硬件MISO(数据输入，本项目使用的tft屏幕没有MISO引脚),触摸引脚初始化放在tft.init()后,否则触摸引脚无法读取及中断
    // pinMode(touchPin, INPUT);
    // attachInterrupt(digitalPinToInterrupt(touchPin), TouchInterrupt, RISING); //设置中断

    //连接wifi

    LineLoad(1, 1);

    WiFi.begin(ssid, password);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        LineLoad(0, 0);
        Serial.print(".");
        LineLoad(1, 0);
    }
    Serial.print("connected\nto\n");
    Serial.println(ssid);
    timeClient.begin();
    LineLoad(0, 0);
}

void loop()
{
    //清屏
    tft.fillScreen(backgroundColor);

    //获取时间
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    //转换时间数据为字符串，若小于10，则在前面添加“0”
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
    /*
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString(strHours, 5, 90);
    tft.drawString(":", 26, 90);
    tft.drawString(strMinutes, 35, 90);
    tft.drawString(":", 56, 90);
    tft.drawString(strSeconds, 65, 90);
    */
    Serial.print(strHours);
    Serial.print(':');
    Serial.print(strMinutes);
    Serial.print(':');
    Serial.println(strSeconds);
    delay(300);
}

void showOeSLogo(int x, int y)
{
    unsigned char *bufLogo = new unsigned char[210];
    if (bufLogo)
    {
        memcpy_P(bufLogo, OeScreenLogo, 210);
        tft.drawBitmap(x, y, bufLogo, 100, 16, CPSOeColor, backgroundColor);
    }
    delete bufLogo;
}

void LineLoad(bool loadDir, bool setCase)
// loadDir设置加载条滚动方向(1为进入，0为滚出)，setCase为是否设置边框，1设置，0不设置
{
    int LineColor;

    if (setCase) //绘制加载条边框
    {
        int color1 = tft.color565(180, 180, 180);    //外条颜色
        int colorCase = tft.color565(135, 135, 135); //外框颜色
        tft.drawFastHLine(45, 75, 72, color1);
        tft.drawFastHLine(45, 83, 72, color1);
        tft.drawRect(43, 76, 76, 7, colorCase);
        tft.drawPixel(43, 76, backgroundColor);
        tft.drawPixel(43, 82, backgroundColor);
        tft.drawPixel(118, 76, backgroundColor);
        tft.drawPixel(118, 82, backgroundColor);
    }

    if (loadDir) //设置加载条滚动方向
    {
        LineColor = tft.color565(186, 203, 221); //加载条为蓝色(滚入)
    }
    else
    {
        LineColor = backgroundColor; //加载条为背景色(滚出)
    }

    // 加载条滚动部分宽74像素,高5像素
    int rectWidth = 1; //每次所画矩形宽度
    int add = 0;       //每次宽度加数(递加)
    int rectX = 44;    //矩形左上顶点x坐标

    /*加载条由左向右滚动,每次在原来矩形的基础上,增加宽度,在上一个矩形右侧绘制下一个矩形,
      直至矩形宽度超出剩余空间，使最后一个矩形刚好填满剩余空间*/
    while (rectX <= 117)
    {
        if (rectX + rectWidth > 117)
        {
            rectX = 117 - rectWidth;
            tft.fillRect(rectX, 77, rectWidth + 1, 5, LineColor);
            break;
        }
        tft.fillRect(rectX, 77, rectWidth, 5, LineColor);
        rectX += rectWidth;
        rectWidth += add;
        add++;
        delay(100);
    }
}

// ICACHE_RAM_ATTR void TouchInterrupt() //触摸中断
//{
// }
