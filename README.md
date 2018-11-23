# qtCyberDIP_C17
![CyberDIP](/pic/CyberDIP.png)


[CyberDIP](https://github.com/LostXine/qtCyberDIP) is developed by LostXine.

We use CyberDIP platform to develop a program that can automatically play jigsaw puzzle on smart phone or pad.
![Test1](/pic/test1.png)
![Test2](/pic/test2.jpg)


***
### TODO
* Find solution to replace Airplayer.exe+PrintScreen

### Dependence
* [Qt](https://www.qt.io/) ≥5.7
* [FFmpeg](https://ffmpeg.org/) 2.2.2 (included in [/3rdparty](/3rdparty))
* [QTFFmpegWrapper](https://inqlude.org/libraries/qtffmpegwrapper.html) (included in [/qtCyberDip/QTFFmpegWrapper](/qtCyberDip/QTFFmpegWrapper))
* [Android Debug Bridge](http://developer.android.com/tools/help/adb.html) (included in [/qtCyberDip/prebuilts](/qtCyberDip/prebuilts))
* (Optional)[OpenCV](http://www.opencv.org/) ≥2.4.9
* [jigsaw](https://play.google.com/store/apps/details?id=com.outfit7.jigtyfree) in Android

### Framework
qtCyberDIP contains 4 + 1 parts:
* __Serial Port(COM)__: Serial port and g-code module to work with Arduino.
* __[BBQScreen Client](https://github.com/xplodwild/bbqscreen_client)__: BBQScreen client module to decode frames from Android devices.
* __Screen Capture__: Win32 API module to grab screen or windows, working with [AirPlayer](http://pro.itools.cn/airplayer) for iOS devices and [Total Control](http://tc.sigma-rt.com.cn/index.php) for Android devices.
* __Video Input__: Read image sequences from files.
* __Game Controller__: User defined game controller, change this to play different games.

### Usage
* Install Visual Studio (≥2013)
* Install Qt and Qt VS Add-In
* Install git for windows
* Clone this project using script below at git bash:
```
git clone https://github.com/LostXine/qtCyberDIP.git
```
* (If necessary)Install Driver for CH340 Chipset after CyberDIP connected. ([Driver](/CH340) is included)
* Add '3rdparty/x64' (Default, if you are using 32-bit OS, add '3rdparty/x86') into Environment Variable 'Path' (__ATTENTION! NEVER CLEAN 'Path'__, unless you are exactly conscious of the consequence)
* Open [qtCyberDip.sln](/qtCyberDip.sln) in Visual Studio and follow the instructions in [stdafx.h](qtCyberDip/stdafx.h)
* Screenshot of origin image of jigsaw should be got first and renamed as 'source_image_X.png' (X means the order that the jigsaw shows in the app). All these origin should be put in qtCyberDip folder.


