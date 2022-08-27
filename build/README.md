# OeScreen V1.1-1.0
/bulid Files
此文件夹包含编译所需文件：
    .\data          -包含需存储至Falsh文件系统的文件
    .\main.ino      -Arduino程序文件
编译说明:
    1.上传程序前请先添加esp8266fs插件，
        在工具-Flash Size选择“4MB(FS：1MB ...)”并上传文件(参考http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/spiffs/upload-files/)
    2.请使用Arduino或VS Code-Arduino插件编译(请先配置Arduino esp8266环境 版本为2.7.4，下载TFT_eSPI库)