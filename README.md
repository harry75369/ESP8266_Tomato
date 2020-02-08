# ESP8266 Tomato Clock (a.k.a Pomodoro)

A Tomato Clock based on ESP8266 chip (WeMos D1 mini).

Actual boards:

![](https://raw.githubusercontent.com/harry75369/ESP8266_Tomato/master/imgs/photo.jpg)

WebUI:

![](https://raw.githubusercontent.com/harry75369/ESP8266_Tomato/master/imgs/ui.png)

## Prerequisites

1. Arduino IDE
2. [Arduino core for ESP8266](https://github.com/esp8266/Arduino)
3. Arduino Plugins (which can be found in `tools` directory, see README [there](https://github.com/harry75369/ESP8266_Tomato/blob/master/tools/README.md))
    - `ESP8266FS`
    - `ESP8266LittleFS`

## Build Instructions

1. Use Arduino IDE to open this project and compile/upload program to ESP8266.
2. Use `ESP8266LittleFS` plugin to upload files under `data` directory to ESP8266.
3. Done!

## Usage Instructions

### 1) Initialization process

When the power is on:

1. The red LED blinks quickly until filesystem is ready.
2. The builtin blue LED blinks quickly until WiFi is connected.
    - If WiFi is not connected within 30 seconds, it goes into AP mode for user to fill in new WiFi config.
    - The red LED will blink slowly.
    - Once WiFi config is updated, AP mode is turned off and it goes back to the second step.
3. The green LED blinks quickly until time is synchronized.

After initialization is done, the builtin blue LED will blink slowly to indicate the system is ready.

### 2) Start to work

- You can press the blue button multiple times, each press means adding a full tomato clock cycle.
- Each time the blue button is pressed, it beeps to indicate how many cycles there are.
- The green LED will be turned on to indicate the working status.

### 3) Start to rest

- When the work time is up, the beeper will play music (Huluwa) to tell you to have a rest.
- The green LED will be turned off.
- The red LED will be turned on to indicate the resting status.

### 4) Continue to work

- If there are more than one cycles, you need to continue working after a rest.
- The beeper will play music (TaiYangZhaoChangShengQi) to tell you to continue to work.
- Before the music, it beeps to indicate how many cycles is remained.
- The red LED will be turned off.
- The green LED will be turned on to indicate the working status.

### 5) Finish all work and rest

- The beeper will play music (HuanLeSong).
- The red and green LEDs will be turned off.
- The builtin blue LED will start to blink slowly.

### Notes

- All music must be turned off manually by pressing the red button.
- Initially, the work duration of each tomato cycle is `25` minutes, and the rest duration is `5` minutes.

## WebUI Instructions

When the system is ready, you can access the WebUI at `http://tomato.local`.

The address is provided by the UDP packet of mDNS protocol, however, the mDNS implementation is buggy and won't work at times. So you can access it directly through its IP address.

Please note the WebUI is for visualization purpose only. All controllers are on the boards for now.

# License

Copyright © 2020 Chaoya Li.

Released under MIT license.
