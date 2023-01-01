/*
by:LiKongDianCPSOe
LICENSE:MIT
*/

#include "OeS.h"
#include "bitmaps.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPADRESS, 60 * 60 * TIMEAREA, 1000 * 40);
TFT_eSPI tft = TFT_eSPI();

const int CPSOeColor = tft.color565(48, 64, 82);         // CPSOe主题颜色 rgb(48,64,82)
const int backgroundColor = tft.color565(212, 212, 212); // 背景颜色rgb (212,212,212)
const int grey = tft.color565(180, 180, 180);            // 浅灰rgb (180,180,180)
const int middleGrey = tft.color565(150, 150, 150);      // 深灰rgb (135，135，135)
const int darkGrey = tft.color565(135, 135, 135);        // 深灰rgb (135，135，135)
const int lightBlue = tft.color565(186, 203, 221);       // 浅蓝rgb (186,203,221)
const int orangeYellow = tft.color565(226, 186, 116);    // 橙黄rgb (226,186,116)

float boardTH[2] = {0.0, 0.0};               // 板上温度,湿度
int int_nowTime[6] = {1970, 10, 1, 0, 0, 0}; // 年,月,日,小时,分钟,星期
char weatherText[7] = "404";                 // 天气现象文字
char weatherTemp[3] = "0";                   // 室外气温
int weatherCode = 99;                        // 天气现象代码,99为未知
bool touchEN = false;                        // 触摸按键按下后中断函数记录,用于引起触摸处理函数
unsigned char nowPage = -1;                  // 现在所在页面
int timerLTime = 0;                          // 计时器剩余时间,单位:s

unsigned char OeS_drawChar(int x, int y, const char Index, int font, int color)
{
    if (font == 0) // fonts0绘制
    {
        for (int i = 0; i < fonts0Amount; i++) // 遍历数组
        {
            if (pgm_read_byte(&fonts0[i].charIndex[0]) == Index) // 对应unicode
            {
                int BmpWidth = 0;
                BmpWidth = pgm_read_word(&fonts0[i].charWidth);                                          // 获取字符宽度
                tft.drawBitmap(x, y, fonts0[i].charBmp, BmpWidth, fonts0Height, color, backgroundColor); // 绘制图标
                return i;
            }
        }
    }

    else if (font == 1) // fonts1绘制
    {
        for (int i = 0; i < fonts1Amount - fonts1LCharAmount; i++) // 遍历数组
        {
            if (pgm_read_byte(&fonts1[i].charIndex[0]) == Index) // 对应unicode
            {
                int BmpWidth = 0;
                BmpWidth = pgm_read_word(&fonts1[i].charWidth);                                          // 获取字符宽度
                tft.drawBitmap(x, y, fonts1[i].charBmp, BmpWidth, fonts1Height, color, backgroundColor); // 绘制图标
                return i;
            }
        }
    }

    return 255; // 错误码
}

bool OeS_drawChars(int x, int y, const char charsIndex[], int font, int color)
{
    int charX = x;                       // 当前字符的横坐标
    unsigned char charNum = 0;           // 字符在结构体数组中的编号
    int BmpWidth = 0;                    // 字符宽度
    int charsLenth = strlen(charsIndex); // 获取字符串长度
    for (int i = 0; i < charsLenth; i++)
    {
        charNum = OeS_drawChar(charX, y, charsIndex[i], font, color); // 绘制一个字符
        if (charNum == 255)
        {
            return false;
        }
        if (font == 1)
        {
            BmpWidth = pgm_read_word(&fonts1[charNum].charWidth); // 获取已绘制的前一个字符宽度
        }
        else if (font == 0)
        {
            BmpWidth = pgm_read_word(&fonts0[charNum].charWidth); // 获取已绘制的前一个字符宽度
        }
        charX += BmpWidth + 1; // 移动游标(字符间隔1个像素)
    }
    return true;
}

bool OeS_drawLChar(int x, int y, const char Index[3], int color)
{
    for (int i = fonts1Amount - fonts1LCharAmount; i < fonts1Amount; i++) // 遍历数组
    {
        if (pgm_read_byte(&fonts1[i].charIndex[0]) == Index[0] && pgm_read_byte(&fonts1[i].charIndex[1]) == Index[1] && pgm_read_byte(&fonts1[i].charIndex[2]) == Index[2]) // 对应unicode
        {
            int BmpWidth = 0;
            BmpWidth = pgm_read_word(&fonts1[i].charWidth);                                          // 获取字符宽度
            tft.drawBitmap(x, y, fonts1[i].charBmp, BmpWidth, fonts1Height, color, backgroundColor); // 绘制图标
            return true;
        }
    }
    return false;
}

bool OeS_drawIcon(int x, int y, int Index, int color)
{
    if (Index == -1)
    {
        return false;
    }
    for (int i = 0; i < iconsAmount; i++) // 遍历数组
    {
        if (pgm_read_word(&icons[i].iconIndex) == Index)
        {
            int BmpWidth = 0;
            int BmpHeight = 0;
            BmpWidth = pgm_read_word(&icons[i].iconWidth);
            BmpHeight = pgm_read_word(&icons[i].iconHeight);
            tft.drawBitmap(x, y, icons[i].iconBmp, BmpWidth, BmpHeight, color, backgroundColor); // 绘制图标
            return true;
        }
    }
    return false;
}

void OeS_Loading(int x, int y, bool loadDir, bool setCase)
{
    int LineColor;

    if (setCase) // 绘制加载条边框
    {
        tft.drawFastHLine(x + 2, y, 72, grey);     // 内框(上)
        tft.drawFastHLine(x + 2, y + 8, 72, grey); // 内框(下)
        tft.drawRect(x, y + 1, 76, 7, darkGrey);   // 外框
        tft.drawPixel(x, y + 1, backgroundColor);
        tft.drawPixel(x, y + 7, backgroundColor);
        tft.drawPixel(x + 75, y + 1, backgroundColor);
        tft.drawPixel(x + 75, y + 7, backgroundColor);
    }

    if (loadDir) // 设置加载条滚动方向
    {
        LineColor = lightBlue; // 加载条为蓝色(滚入)
    }
    else
    {
        LineColor = backgroundColor; // 加载条为背景色(滚出)
    }

    // 加载条滚动部分宽74像素,高5像素
    int rectWidth = 1; // 每次所画矩形宽度
    int add = 0;       // 每次宽度加数(递加)
    int rectX = x + 1; // 矩形左上顶点x坐标

    /*加载条由左向右滚动,每次在原来矩形的基础上,增加宽度,在上一个矩形右侧绘制下一个矩形,
      直至矩形宽度超出剩余空间，使最后一个矩形刚好填满剩余空间*/
    while (rectX <= x + 74)
    {
        if (rectX + rectWidth > 117)
        {
            rectX = x + 74 - rectWidth;
            tft.fillRect(rectX, y + 2, rectWidth + 1, 5, LineColor);
            break;
        }
        tft.fillRect(rectX, y + 2, rectWidth, 5, LineColor);
        rectX += rectWidth;
        rectWidth += add;
        add++;
        delay(100);
    }
} // loadDir设置加载条滚动方向(1为进入，0为滚出)，setCase为是否设置边框，1设置，0不设置

void OeS_intToChars(int src, char *arryPointer)
{
    switch (src)
    {
    case 0:
        strcpy(arryPointer, "00");
        break;
    case 1:
        strcpy(arryPointer, "01");
        break;
    case 2:
        strcpy(arryPointer, "02");
        break;
    case 3:
        strcpy(arryPointer, "03");
        break;
    case 4:
        strcpy(arryPointer, "04");
        break;
    case 5:
        strcpy(arryPointer, "05");
        break;
    case 6:
        strcpy(arryPointer, "06");
        break;
    case 7:
        strcpy(arryPointer, "07");
        break;
    case 8:
        strcpy(arryPointer, "08");
        break;
    case 9:
        strcpy(arryPointer, "09");
        break;
    default:
        itoa(src, arryPointer, 10);
        break;
    }
}

void OeS_getBoradTH()
{
    unsigned int SHT30_Data[6];
    Wire.beginTransmission(SHT30_Adr);
    Wire.write(0x2C);
    Wire.write(0x06);
    Wire.endTransmission();
    delay(15);
    Wire.requestFrom(SHT30_Adr, 6);

    // 读取6字节的数据
    // 这六个字节分别为：温度8位高数据，温度8位低数据，温度8位CRC校验数据
    //                湿度8位高数据，湿度8位低数据，湿度8位CRC校验数据
    if (Wire.available() == 6)
    {
        SHT30_Data[0] = Wire.read();
        SHT30_Data[1] = Wire.read();
        SHT30_Data[2] = Wire.read();
        SHT30_Data[3] = Wire.read();
        SHT30_Data[4] = Wire.read();
        SHT30_Data[5] = Wire.read();
    }

    boardTH[0] = ((((SHT30_Data[0] * 256.0) + SHT30_Data[1]) * 175) / 65535.0) - 45; // 更改温度数据
    boardTH[1] = ((((SHT30_Data[3] * 256.0) + SHT30_Data[4]) * 100) / 65535.0);      // 更改湿度数据
}

bool OeS_getWeather()
{
    // 创建TCP连接
    WiFiClient client;
    if (!client.connect(WEATHERHOST, WEATHERPORT))
    {
        return false;
    }
    // 发送网络请求
    client.print(String("GET ") +
                 "/v3/weather/now.json?key=" + WEATHERKEY + "&location=" + WEATHERLOCATION + "&language=zh-Hans&unit=c" +
                 " HTTP/1.1\r\n" + "Host: " + WEATHERHOST + "\r\n" + "Connection: close\r\n\r\n");
    delay(50);
    // 定义answer变量用来存放请求网络服务器后返回的数据
    String answer;
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        answer += line;
    }
    // 断开服务器连接
    client.stop();
    // 获得json格式的数据
    String jsonAnswer;
    int jsonIndex;
    // 找到有用的返回数据位置i 返回头不要
    for (int i = 0; i < answer.length(); i++)
    {
        if (answer[i] == '{')
        {
            jsonIndex = i;
            break;
        }
    }
    jsonAnswer = answer.substring(jsonIndex);

    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, jsonAnswer);

    JsonObject results_0 = doc["results"][0];

    JsonObject results_0_now = results_0["now"];
    const char *results_0_now_text = results_0_now["text"];               // 天气现象文字 如"阴"
    const char *results_0_now_temperature = results_0_now["temperature"]; // 室外气温 如"21"
    const char *results_0_now_code = results_0_now["code"];               // 天气代码"9"

    strcpy(weatherText, results_0_now_text);
    strcpy(weatherTemp, results_0_now_temperature);
    weatherCode = atoi(results_0_now_code);
}

void OeS_drawWeatherIcon(int x, int y, int color)
{
    int weather_uni = 0;
    switch (weatherCode)
    {
    case 0:
        weather_uni = uni_sunny_Day;
        color = orangeYellow;
        break;
    case 1:
        weather_uni = uni_sunny_Night;
        color = orangeYellow;
        break;
    case 2:
        weather_uni = uni_sunny_Day;
        color = orangeYellow;
        break;
    case 3:
        weather_uni = uni_sunny_Night;
        color = orangeYellow;
        break;
    case 4:
        if (int_nowTime[3] >= 6 && int_nowTime[3] <= 18)
        {
            weather_uni = uni_cloudy_Day;
        }
        else
        {
            weather_uni = uni_cloudy_Night;
        }
        break;
    case 5:
        weather_uni = uni_cloudy_Day;
        break;
    case 6:
        weather_uni = uni_cloudy_Night;
        break;
    case 7:
        weather_uni = uni_cloudy_Day;
        break;
    case 8:
        weather_uni = uni_cloudy_Night;
        break;
    case 9:
        weather_uni = uni_cloudy_Day;
        break;
    case 10:
        if (int_nowTime[3] >= 6 && int_nowTime[3] <= 18)
        {
            weather_uni = uni_rainy_Day;
        }
        else
        {
            weather_uni = uni_rainy_Night;
        }
        break;
    case 11:
        weather_uni = uni_thunderShower;
        break;
    case 12:
        weather_uni = uni_thunderShower;
        break;
    case 13:
        weather_uni = uni_rain;
        break;
    case 14:
        weather_uni = uni_rain;
        break;
    case 15:
        weather_uni = uni_rain;
        break;
    case 16:
        weather_uni = uni_storm;
        break;
    case 17:
        weather_uni = uni_storm;
        break;
    case 18:
        weather_uni = uni_storm;
        break;
    case 34:
        weather_uni = uni_windStorm;
        break;
    case 35:
        weather_uni = uni_windStorm;
        break;
    case 36:
        weather_uni = uni_windStorm;
        break;
    default:
        weather_uni = -1;
        break;
    }
    for (int i = 19; i < 26; i++)
    {
        if (weatherCode == i)
        {
            if (int_nowTime[3] >= 6 && int_nowTime[3] <= 18)
            {
                weather_uni = uni_snowy_Day;
            }
            else
            {
                weather_uni = uni_snowy_Night;
            }
        }
    }
    for (int i = 26; i < 34; i++)
    {
        if (weatherCode == i)
        {
            weather_uni = uni_dustSand;
            if (i <= 29)
            {
                color = orangeYellow;
            }
        }
    }
    OeS_drawIcon(x, y, weather_uni, color);
}

void OeS_drawTemp(bool type, int x, int y, int numColor, int puncColor)
{
    float tempFloat;
    char tempC[5]; // 存储转换后的字符串
    if (type)
    {
        tft.drawRect(x, y, 13, 9, backgroundColor); // 清屏
        int charsX = x;
        strcpy(tempC, weatherTemp);
        if (weatherTemp[0] == '-')
        {
            charsX -= 4;
        }
        OeS_drawChars(charsX, y, tempC, 1, numColor);
    }
    else
    {
        tft.drawRect(x - 4, y - 1, 26, 9, backgroundColor); // 清屏
        tempFloat = boardTH[0];
        if (tempFloat >= 0) // 如果温度为非负
        {
            dtostrf(tempFloat, 2, 1, tempC);
        }
        else // 如果温度为负
        {
            float posTemp = -tempFloat;
            dtostrf(posTemp, 2, 1, tempC);
            OeS_drawChar(x - 4, y, '-', 1, numColor);
        }
        OeS_drawChars(x, y, tempC, 1, numColor);
        OeS_drawLChar(x + 14, y - 1, "℃", puncColor);
    }
}

void OeS_drawHumidity(int x, int y, int numColor, int puncColor)
{
    float humidityFloat;
    char humidity[5];
    humidityFloat = boardTH[1];
    dtostrf(humidityFloat, 2, 1, humidity);

    tft.drawRect(x, y - 1, 22, 8, backgroundColor); // 清屏
    OeS_drawChars(x, y, humidity, 1, numColor);
    OeS_drawChar(x + 14, y, '%', 1, puncColor);
}

bool OeS_getTime()
{
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    if (ptm->tm_year == 1970)
    {
        return false;
    }
    int_nowTime[0] = ptm->tm_year + 1900;
    int_nowTime[1] = ptm->tm_mon + 1;
    int_nowTime[2] = ptm->tm_mday;
    int_nowTime[3] = ptm->tm_hour;
    int_nowTime[4] = ptm->tm_min;
    int_nowTime[5] = ptm->tm_wday;
    // 年,月,日,小时,分钟,星期
    return true;
}

bool OeS_drawTime(int x, int y, int color1, int color2, int puncColor, int selection, bool clean)
{
    char chars_nowTime[6][8] = {"0000", "00", "00", "00", "00", " "};
    if (!clean)
    {
        switch (int_nowTime[5])
        {
        case 0:
            strcpy(chars_nowTime[5], "日");
            break;
        case 1:
            strcpy(chars_nowTime[5], "一");
            break;
        case 2:
            strcpy(chars_nowTime[5], "二");
            break;
        case 3:
            strcpy(chars_nowTime[5], "三");
            break;
        case 4:
            strcpy(chars_nowTime[5], "四");
            break;
        case 5:
            strcpy(chars_nowTime[5], "五");
            break;
        case 6:
            strcpy(chars_nowTime[5], "六");
            break;

        default:
            return false;
        }
        for (int i = 0; i < 5; i++)
        {
            char *arrP = chars_nowTime[i];
            OeS_intToChars(int_nowTime[i], arrP);
        }
    }
    switch (selection)
    {
    case 0:
        if (clean)
        {
            tft.fillRect(x, y, 24, fonts1Height, backgroundColor);
            return true;
        }
        OeS_drawLChar(x, y, "星", color1);
        OeS_drawLChar(x + 7, y, "期", color2);
        OeS_drawLChar(x + 15, y, chars_nowTime[5], puncColor);
        return true;
    case 1:
        if (clean)
        {
            tft.fillRect(x, y, 40, fonts1Height, backgroundColor);
            return true;
        }
        OeS_drawChars(x, y, chars_nowTime[0], 1, color1);
        OeS_drawChar(x + 17, y, '/', 1, puncColor);
        OeS_drawChars(x + 21, y, chars_nowTime[1], 1, color2);
        OeS_drawChar(x + 29, y, '/', 1, puncColor);
        OeS_drawChars(x + 33, y, chars_nowTime[2], 1, color2);
        return true;
    case 2:
        if (clean)
        {
            tft.fillRect(x, y, 112, fonts0Height, backgroundColor);
            return true;
        }
        OeS_drawChars(x, y, chars_nowTime[3], 0, color1);
        OeS_drawChar(x + 54, y, ':', 0, puncColor);
        OeS_drawChars(x + 62, y, chars_nowTime[4], 0, color2);
        return true;
    case 3:
        if (clean)
        {
            tft.fillRect(x, y, 25, fonts1Height, backgroundColor);
            return true;
        }
        else if (chars_nowTime[1] == "04")
        {
            if (chars_nowTime[2] == "05")
            {
                OeS_drawLChar(x, y, "清", color1);
                OeS_drawLChar(x + 8, y, "明", color2);
            }
        }
        else if (chars_nowTime[1] == "05")
        {
            if (chars_nowTime[2] == "01")
            {
                OeS_drawLChar(x, y, "劳", color1);
                OeS_drawLChar(x + 8, y, "动", color2);
            }
        }
        else if (chars_nowTime[1] == "06")
        {
            if (chars_nowTime[2] == "03")
            {
                OeS_drawLChar(x, y, "端", color1);
                OeS_drawLChar(x + 8, y, "午", color2);
            }
        }
        else if (chars_nowTime[1] == "06")
        {
            if (chars_nowTime[2] == "10")
            {
                OeS_drawLChar(x, y, "中", color1);
                OeS_drawLChar(x + 8, y, "秋", color2);
            }
        }
        else if (chars_nowTime[1] == "06")
        {
            if (chars_nowTime[2] == "01")
            {
                OeS_drawLChar(x, y, "国", color1);
                OeS_drawLChar(x + 8, y, "庆", color2);
            }
        }
        else
        {
            return false;
        }
        OeS_drawLChar(x + 16, y, "节", puncColor);
        return true;
    default:
        return false;
    }
}

void OeS_beepSound_00(int beepTime, int beepFreq)
{
    analogWriteFreq(beepFreq);
    analogWrite(BEEPPIN, 511);
    analogWriteFreq(beepFreq);
    delay(beepTime);
    analogWrite(BEEPPIN, 0);
}

ICACHE_RAM_ATTR void OeS_touchInterrupt() // 触摸中断
{
    touchEN = true;
}

bool OeS_handleTouch(unsigned char mode)
{
    int delayTime = 10; // 每帧动画之间间隔
    int x = 158;        // 动画圆圈圆心横坐标
    int y = 2;          // 动画圆圈圆心纵坐标
    int circleSize = 6; // 动画最小圆圈半径

    if (mode == 1)
    {
        tft.fillCircle(x, y, 2, middleGrey);
        delay(delayTime);
        tft.fillCircle(x, y, 4, middleGrey);
        delay(delayTime);
        tft.fillCircle(x, y, 2, backgroundColor);
        delay(delayTime);
        tft.fillCircle(x, y, 4, backgroundColor);
        tft.drawCircle(x, y, circleSize, middleGrey);
        if (digitalRead(TOUCHPIN))
        {
            delay(delayTime);
            tft.drawCircle(x, y, circleSize, backgroundColor);
            delay(delayTime);
            tft.drawCircle(x, y, circleSize + 2, middleGrey);
            delay(delayTime);
            tft.drawCircle(x, y, circleSize + 3, middleGrey);
            delay(delayTime);
            tft.drawCircle(x, y, circleSize + 4, middleGrey);
        }
        touchEN = false;
        return true;
    }
    else if (mode == 2)
    {
        tft.drawCircle(x, y, circleSize + 1, lightBlue);
        delay(delayTime);
        tft.drawCircle(x, y, circleSize + 2, CPSOeColor);
        delay(delayTime);
        tft.drawCircle(x, y, circleSize + 3, CPSOeColor);
        delay(delayTime);
        tft.drawCircle(x, y, circleSize + 4, CPSOeColor);
        return true;
    }
    else if (mode == 3)
    {
        tft.fillCircle(x, y, 4, middleGrey);
        return true;
    }
    else if (mode == 0)
    {
        tft.fillCircle(x, y, circleSize, backgroundColor);
        tft.drawCircle(x, y, circleSize + 1, backgroundColor);
        delay(delayTime);
        tft.drawCircle(x, y, circleSize + 2, backgroundColor);
        tft.drawCircle(x, y, circleSize + 3, backgroundColor);
        delay(delayTime);
        tft.drawCircle(x, y, circleSize + 4, backgroundColor);
        return true;
    }
    else
    {
        return false;
    }
}

#define timerTri0x 101
#define timerTri0y 41

#define timerTri1x 101
#define timerTri1y 82

#define timerTri2x 40
#define timerTri2y 41

#define timerTri3x 40
#define timerTri3y 82
void OeS_drawTimerTri(int selectTri)
{
    switch (selectTri)
    {
    case 0:
        tft.fillTriangle(timerTri0x, timerTri0y + 5, timerTri0x + 4, timerTri0y, timerTri0x + 9, timerTri0y + 5, CPSOeColor);
        break;
    case 1:
        tft.fillTriangle(timerTri0x, timerTri0y + 5, timerTri0x + 4, timerTri0y, timerTri0x + 9, timerTri0y + 5, backgroundColor);
        tft.fillTriangle(timerTri1x, timerTri1y, timerTri1x + 4, timerTri1y + 5, timerTri1x + 9, timerTri1y, CPSOeColor);
        break;
    case 2:
        tft.fillTriangle(timerTri1x, timerTri1y, timerTri1x + 4, timerTri1y + 5, timerTri1x + 9, timerTri1y, backgroundColor);
        tft.fillTriangle(timerTri2x, timerTri2y + 5, timerTri2x + 4, timerTri2y, timerTri2x + 9, timerTri2y + 5, CPSOeColor);
        break;
    case 3:
        tft.fillTriangle(timerTri2x, timerTri2y + 5, timerTri2x + 4, timerTri2y, timerTri2x + 9, timerTri2y + 5, backgroundColor);
        tft.fillTriangle(timerTri3x, timerTri3y, timerTri3x + 4, timerTri3y + 5, timerTri3x + 9, timerTri3y, CPSOeColor);
        break;
    default:
        break;
    }
}

void OeS_drawTimerNum(bool refresh)
{
    if (refresh)
    {
        OeS_drawChars(64, 83, "00:00", 1, darkGrey); // 计时器数字
    }
    else if (timerLTime >= 60)
    {
        tft.fillRect(74, 83, 8, 7, backgroundColor);

        int seconds = timerLTime % 60;
        char charsMin[3];
        char charsSec[3];
        OeS_intToChars((timerLTime - seconds) / 60, charsMin);
        OeS_intToChars(seconds, charsSec);

        OeS_drawChars(64, 83, charsMin, 1, darkGrey);
        OeS_drawChars(74, 83, charsSec, 1, darkGrey);
    }
    else if (timerLTime > 0)
    {
        char charsSec[3];
        OeS_intToChars(timerLTime, charsSec);
        OeS_drawChars(74, 83, charsSec, 1, darkGrey);
    }
}

void OeS_00_Setup()
{
    nowPage = 0;
    tft.init();
    tft.fillScreen(backgroundColor);
    tft.setRotation(3); // tft屏幕初始化

    Wire.begin(SDApin, SCLpin); // I2C初始化

    OeS_drawIcon(31, 48, uni_OeSLogo, CPSOeColor); // 显示项目Logo

    //----连接WiFi----
    OeS_Loading(43, 75, 1, 1);

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(WIFISSID, WIFIPSD);
    while (WiFi.status() != WL_CONNECTED)
    {
        OeS_Loading(43, 75, 0, 0);
        OeS_Loading(43, 75, 1, 0);
    }
    timeClient.begin();
    OeS_Loading(43, 75, 0, 0);
    if (!timeClient.update())
    {
        OeS_Loading(43, 75, 1, 0);
        while (!timeClient.update())
        {
            delay(100);
            timeClient.update();
        }
    }
    OeS_getWeather();
    if (!OeS_getTime())
    {
        timeClient.update();
        OeS_getTime();
    }
    OeS_Loading(43, 75, 0, 0);

    // 清屏
    tft.fillScreen(backgroundColor);

    // 触摸中断初始化
    //  注意:由于触摸引脚使用硬件MISO(数据输入，本项目使用的tft屏幕没有MISO引脚),
    // 触摸引脚初始化放在tft.init()后,否则触摸引脚无法读取及中断
    pinMode(TOUCHPIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(TOUCHPIN), OeS_touchInterrupt, RISING); // 设置中断
}

void OeS_01_Main(unsigned char mode01)
{
    nowPage = 1;
    if (mode01 == 2) // 更新温湿度
    {
        OeS_getBoradTH();
        OeS_drawTemp(0, 47, 118, darkGrey, darkGrey);
        OeS_drawHumidity(66, 108, darkGrey, darkGrey);
    }
    else if (mode01 == 1) // 更新时间
    {
        OeS_getTime();
        OeS_drawTime(23, 37, 0, 0, 0, 2, true);
        OeS_drawTime(23, 37, CPSOeColor, darkGrey, lightBlue, 2, false);
    }
    else if (mode01 == 0) // 初始化
    {
        tft.fillScreen(backgroundColor);
        if (!OeS_getTime()) // 重试获取时间
        {
            timeClient.update();
            OeS_getTime();
        }
        OeS_drawTime(23, 37, CPSOeColor, darkGrey, lightBlue, 2, false); // 时间
        OeS_drawTime(46, 18, darkGrey, darkGrey, lightBlue, 1, false);   // 日期
        OeS_drawTime(89, 18, darkGrey, darkGrey, darkGrey, 0, false);    // 星期
        OeS_drawTime(69, 26, darkGrey, darkGrey, darkGrey, 3, false);    // 节日

        OeS_drawWeatherIcon(97, 69, grey); // 绘制天气图标

        OeS_drawIcon(58, 71, uni_timerCircle, lightBlue); // 计时器圈
        OeS_drawChars(64, 83, "00:00", 1, darkGrey);      // 计时器数字
        tft.setSwapBytes(true);
        tft.pushImage(10, 71, 42, 28, AmiYa); // 阿米娅
        tft.setSwapBytes(false);

        OeS_drawTemp(1, 76, 118, darkGrey, darkGrey);
        OeS_drawLChar(27, 118, "温", darkGrey);
        OeS_drawLChar(35, 118, "度", darkGrey);
        OeS_drawLChar(48, 108, "湿", darkGrey);
        OeS_drawLChar(56, 108, "度", darkGrey);
    }
}

void OeS_02_Timer(unsigned char mode02, char *poiMin, char *poiSec)
{
    tft.fillRect(19, 50, 55, 29, backgroundColor);
    tft.fillRect(80, 50, 55, 29, backgroundColor); // 清屏

    switch (mode02)
    {
    case 0: // 初始化
        tft.fillScreen(backgroundColor);
        for (int i = 45; i < 51; i++)
        {
            tft.drawCircle(187, 64, i, grey);
        }
        OeS_drawChars(19, 50, "00", 0, darkGrey);
        OeS_drawChar(73, 50, ':', 0, darkGrey);
        OeS_drawChars(80, 50, "00", 0, orangeYellow);
        OeS_drawLChar(147, 54, "计", TFT_WHITE);
        OeS_drawLChar(147, 62, "时", TFT_WHITE);
        OeS_drawLChar(147, 70, "器", TFT_WHITE);
        break;
    case 1: // 分钟
        OeS_drawChars(19, 50, poiMin, 0, darkGrey);
        OeS_drawChars(80, 50, poiSec, 0, orangeYellow);
        break;
    case 2: // 秒
        OeS_drawChars(19, 50, poiMin, 0, orangeYellow);
        OeS_drawChars(80, 50, poiSec, 0, darkGrey);
        break;
    default:
        break;
    }
}