# TwitterMoodLight
TwitterMoodLight using ESP8266 + Neopixels

<a href="url"><img src="https://github.com/HanYangZhao/MoodLight/blob/master/photos/1.jpg"  width="500" ></a>
<a href="url"><img src="https://github.com/HanYangZhao/MoodLight/blob/master/photos/DSC_9209-2_3000.jpg"  width="500" ></a>
<a href="url"><img src="https://github.com/HanYangZhao/MoodLight/blob/master/photos/DSC_9166.JPG"  width="500" ></a>

# 3D Models 

https://www.thingiverse.com/thing:784917

https://www.thingiverse.com/thing:774456

# How to build 

http://www.instructables.com/id/Twitter-Mood-Lamps-With-ESP8266/

#Software Installation Instructions

1. Download the Arduino IDE  https://www.arduino.cc/en/Main/Software

2. Enable ESP8266 Support in Arduino IDE

    Installing with Boards Manager
    Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).
    
    Install Arduino 1.6.8 from the Arduino website.
    Start Arduino and open Preferences window.
    
    Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
    
    Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
    
3. Copy the libraries inside the Libraries folder into your Arduino library folder (.../Documents/Arduino/libraries)

4.  In Tools/Board choose Adafruit Huzzah ESP8266 , 4M flash size , 160mhz cpu speed , 115200 upload speed.

Full guide here https://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html
