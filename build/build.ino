/*
by:LiKongDianCPSOe
LICENSE:MIT
*/

#include "OeS.h"
extern byte nowPage;
extern int timerLTime;
extern bool touchEN;

void setup()
{
    OeS_00_Setup();
    OeS_01_Main(0);
}

void loop()
{
    if (nowPage == 1)
    {
        OeS_01_Main(0); // 初始化

        while (nowPage == 1)
        {
            OeS_01_Main(2); // 更新气温、湿度
            OeS_01_Main(1); // 更新时间

            int readDelayTime = 0; // 按下时间计数
            bool delayEN = false;  // 按下计时使能
            unsigned char timerCounter = 0;

            for (int i = 0; i < 1000; i++)
            {
                if (timerLTime > 0)
                {
                    timerCounter++;
                    if (timerCounter >= 98)
                    {
                        OeS_drawTimerNum(false);
                        timerCounter = 0;
                        timerLTime--;
                        OeS_drawTimerNum(false);
                        if (timerLTime == 0)
                        {
                            OeS_drawTimerNum(true);
                            int i = 0;
                            while (i < timerBeepTimes)
                            {
                                OeS_beepSound_00(timerBeepLenth, timerBeepFreq);
                                delay(timerBeepDelay);
                                i++;
                            }
                        }
                    }
                }

                // 处理触摸
                if (delayEN)
                {
                    readDelayTime++;
                    Serial.println(readDelayTime);
                    if (!digitalRead(TOUCHPIN))
                    {
                        OeS_handleTouch(0);
                        delayEN = false;
                    }
                    else if (readDelayTime >= 180)
                    {
                        OeS_handleTouch(2);
                        OeS_beepSound_00(100, 1046);
                        readDelayTime = 0;
                        nowPage = 2;
                        timerLTime = 0;
                        break;
                    }
                }
                else if (touchEN)
                {
                    OeS_handleTouch(1);
                    touchEN = false;
                    delayEN = true;
                }
                delay(10);
            }
        }
    }

    else if (nowPage == 2)
    {
        int timerTriangleDir = 0; // 0 秒-增加,1 秒-减少,2 分-增加,3 分-减少
        int setMin = 0;
        int setSec = 0;
        char charMin[3];
        char charSec[3];
        char *pMin = charMin;
        char *pSec = charSec;
        int readDelayTime = 0;
        bool delayEN = false;
        OeS_02_Timer(0, pMin, pSec);
        OeS_drawTimerTri(timerTriangleDir);

        while (timerTriangleDir < 4)
        {
            if (delayEN) // 若按住并正在计时
            {
                readDelayTime++;
                if (!digitalRead(TOUCHPIN) && readDelayTime <= 60 && readDelayTime >= 10) // 松开
                {
                    OeS_handleTouch(0);
                    touchEN = false;
                    delayEN = false;
                    readDelayTime = 0;
                    switch (timerTriangleDir) // 更改计时器数值
                    {
                    case 0:
                        setSec++;
                        break;
                    case 1:
                        setSec--;
                        break;
                    case 2:
                        setMin++;
                        break;
                    case 3:
                        setMin--;
                        break;
                    default:
                        break;
                    }

                    if (setSec >= 60 || setSec <= 0)
                    {
                        setSec = 0;
                    }
                    else if (setMin >= 100 || setMin <= 0)
                    {
                        setMin = 0;
                    }

                    OeS_intToChars(setSec, charSec); // 整型秒转换为字符串
                    OeS_intToChars(setMin, charMin); // 整型分转换为字符串

                    // 绘制计时器
                    if (timerTriangleDir == 0 && timerTriangleDir == 1) // 秒
                    {
                        OeS_02_Timer(2, pMin, pSec);
                    }
                    else // 分
                    {
                        OeS_02_Timer(1, pMin, pSec);
                    }
                }
                else if (readDelayTime >= 150) // 长按
                {
                    touchEN = false;
                    OeS_handleTouch(2);
                    OeS_beepSound_00(100, 1046);
                    timerTriangleDir++;
                    if (timerTriangleDir >= 4)
                    {
                        nowPage = 1;
                        timerLTime = setMin * 60 + setSec;
                        break;
                    }
                    OeS_drawTimerTri(timerTriangleDir);
                    readDelayTime = 0;
                }
            }
            else if (touchEN) // 单点
            {
                OeS_handleTouch(3);
                touchEN = false; // 响应中断
                delay(10);
                if (digitalRead(TOUCHPIN))
                {
                    delayEN = true; // 设置按下标志
                }
            }

            delay(10);
        }
    }
}