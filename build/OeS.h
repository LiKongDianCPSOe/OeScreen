/*
by:LiKongDianCPSOe
LICENSE:MIT
*/

#ifndef _OESSETUP_H_
#define _OESSETUP_H_

#define OESCREEN_VERSION "V1.1-1.0"

#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#define WIFISSID "WIFISSID" // WiFi名称,必要
#define WIFIPSD "WIFIPSD"   // WiFi密码,必要

#define NTPADRESS "ntp2.tencent.com"    // ntp服务器地址
#define TIMEAREA 8                      // 设置时区(中国东八时区，北京时间),东区为正，西区为负
#define WEATHERHOST "api.seniverse.com" // 心知天气api地址
#define WEATHERKEY "KEY"                // 心知天气v3版私钥,必要
#define WEATHERPORT 80                  // 心知天气服务器端口号
#define WEATHERLOCATION "Guangzhou"     // 心知天气获取数据的城市名称 请参考https://seniverse.yuque.com/hyper_data/api_v3

#define TOUCHPIN 12    // 触摸芯片输出引脚
#define BEEPPIN 16     // 蜂鸣器引脚
#define SHT30_Adr 0x44 // sht30(温湿度传感器)I2C地址
#define SDApin 4       // SDA引脚
#define SCLpin 5       // SCL引脚

#define timerBeepTimes 5   // 计时器到时蜂鸣器响的次数
#define timerBeepLenth 300 // 计时器到时蜂鸣器响的时间(单位:ms)
#define timerBeepDelay 400 // 计时器到时蜂鸣器响的间隔时间(单位:ms)
#define timerBeepFreq 880  // 计时器到时蜂鸣器响的频率(单位:Hz)

/*---------------------------------声明函数------------------------------------*/

// 绘制单个字符(unicode所占字节数为1)
// 返回值：0-254为绘制的字符在结构体数组中结构体的编号
//         255为绘制失败,没有该字体或字符
// 横坐标,纵坐标,字符unicode,字体(fontsA为1，fontsB为0),颜色
unsigned char OeS_drawChar(int x, int y, const char Index, int font, int color);

// 绘制字符串(unicode所占字节数为1)
// 返回值：true为绘制成功
//        false为绘制失败,没有该字体或字符
// 横坐标,纵坐标,字符串unicode(每个字节一个字符),字体(fontsA为1，fontsB为0),颜色
bool OeS_drawChars(int x, int y, const char Index[], int font, int color);

// 绘制单个长字符字符(unicode所占字节数为3)
// 返回值：true为绘制成功
//       false为绘制失败,没有该字体或字符
// 横坐标,纵坐标,字符unicode,颜色
bool OeS_drawLChar(int x, int y, const char Index[3], int color);

// 绘制单个图标
// 返回值：true为绘制成功
//       false为绘制失败,没有该字体或字符
// 横坐标,纵坐标,图标编号(详见bitmaps.h定义),颜色
bool OeS_drawIcon(int x, int y, int Index, int color);

/*加载条动画,loadDir设置加载条滚动方向(1为进入,0为滚出),setCase为是否设置边框,1设置,0不设置*/
void OeS_Loading(int x, int y, bool loadDir, bool setCase);

// 整型转字符串,若整型为个位数,字符串为'0'+整型数据
// 指针arryPointer指向字符串
void OeS_intToChars(int src, char *arryPointer);

// 获取板上温湿度(sht30),获取的数据存放在全局变量float boardTH[2]中
void OeS_getBoradTH();

// 通过心知天气v3获取天气现象文字,天气代码及室外温度,获取的天气现象文字储存在char weatherText[7]中
// 获取的室外温度储存在char weatherTemp[3]中,获取的天气代码储存在int weatherCode中
// 返回值：请求无响应返回false
bool OeS_getWeather();

// 获取天气信息后,根据char weatherText[7]的内容选择对应天气图标并绘制
// 横坐标,纵坐标,图标颜色
void OeS_drawWeatherIcon(int x, int y, int color);

// 绘制温度数据
//  x,y为数字十位的左上角坐标
// 温度类型(0为板上温度不带符号,1为室外温度带符号),横坐标,纵坐标,字符颜色,背景颜色
void OeS_drawTemp(bool type, int x, int y, int numColor, int puncColor);

// 绘制湿度数据(板上湿度带符号)
//  x,y为数字十位的左上角坐标
// 横坐标,纵坐标,字符颜色,背景颜色
void OeS_drawHumidity(int x, int y, int numColor, int puncColor);

// 通过ntp获取时间
//  获取的时间为字符串,储存至全局二维字符串_char_nowTime[6]中
//  refrensh:true  -只获取小时和分钟
//           false -获取全部时间数据
// 返回值：若获取时间为1970年(ntp获取时间失败)或星期数据有错误返回false
bool OeS_getTime();

// 绘制时间
/*
int_selection取值:
    0   -   星期                                    (fonts1)
        color1:星   color2:期    puncColor:一/...

    1   -   日期(年+'/'+月+'/'+日)                  (fonts1)
        color1:年   color2:月,日 puncColor:'/'

    2   -   时钟(时+':'+分)                         (fonts0)
        color1:时   color2:分    puncColor:':'

    3   -   节日(法定节假日:国庆清明中秋端午劳动节)  (fonts1)
        color1:名称 color2:名称  puncColor:"节"

bool_clean：true为刷屏,绘制方块覆盖时间

返回值：有int_selection对应值为true,若selection为3且今日非节假日返回false
*/
bool OeS_drawTime(int x, int y, int color1, int color2, int puncColor, int selection, bool clean);

// 蜂鸣器响 声音00:beepFreq(单位:Hz)Hz响beepTime(单位:ms)ms
void OeS_beepSound_00(int beepTime, int beepFreq);

// 中断函数
ICACHE_RAM_ATTR void TouchInterrupt(); // 触摸中断

// 处理触摸中断(交互动画,页面切换,功能页面操作)
// mode: 0为清除 1为灰环 2为深蓝(CPSOe颜色) 3为快速单点
bool OeS_handleTouch(unsigned char mode);

// 根据selectTri绘制计时器三角
void OeS_drawTimerTri(int selectTri);

// 绘制主界面计时器
// percent:0-255,对应计时器蓝圈占比,255为完整蓝圈,0为无蓝圈
// void OeS_draw01Timer(unsigned char percent);

// 绘制主界面计时器
void OeS_drawTimerNum(bool refresh);

// 初始化界面
void OeS_00_Setup();

// 主界面
//  1为时间刷新,0为全屏刷新(初始化),2为温湿度刷新
void OeS_01_Main(unsigned char mode01);

// 计时器界面
// mode02:0为界面初始化,1为切换分,2为设置秒,3为设置分
// timerMin为分的2位字符串,timeSec为秒的2位字符串
void OeS_02_Timer(unsigned char mode02, char *poiMin, char *poiSec);

#endif // #ifndef _OESSETUP_H_