# OeScreen V2.2-1.0
by:LiKongDianCPSOe
LICENSE:MIT
/bulid
此文件夹包含编译所需文件：
    ./build.ino编译文件
    ./OeS.h项目头文件
    ./OeS.cpp项目.cpp文件
    ./bitmaps.h项目点阵数据文件
编译说明:
    1.下载并安装Arduino
    2.上传程序前请先添加esp8266fs插件，
        在工具-Flash Size选择“4MB(FS：1MB ...)”并上传文件(参考http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/spiffs/upload-files/)
    3.在arduino库管理中搜索TFT_eSPI(版本2.4.72),ntpClient,ArduinoJson库，下载
        配置TFT_eSPI:
            (1)在User_Setup.h中选择st7735_driver
            (2)在User_Setup.h中选择128*160对应屏幕参数
            (3)在User_Setup.h中设置esp8266的DC为PIN_D4,RST为-1
            (3)在User_Setup.h中注释所有默认字体
    5.在编译/烧录前请先在OeS.h中设置WiFi密码和名称,心知天气v3密钥(请前往https://seniverse.com查找说明)(任何版本都可以,本项目开发时使用免费版)
    4.请使用Arduino或VS_Code的PlatfromIO插件编译(请先配置Arduino esp8266环境 版本为2.7.4，下载TFT_eSPI,ntpClient,ArduinoJson库)
    5.若使用PlatformIO编译，请将TFT_eSPI,ntpClient,ArduinoJson库文件夹复制到PlatformIO项目文件夹的lib文件夹中