# Tools

## ESP8266FS

This is a plugin of Arduino, which provides an interface in Arduino IDE (Tools -> ESP8266 Sketch Data Upload).

It will upload files under `/data` folder to NOR flash through SPI, and these files will be persistent during power on/off or resets.

Please unzip it to `~/Documents/Arduino/tools` folder if you are under macOS.

## ESP8266LittleFS

This is a plugin of Arduino, which provides an interface in Arduino IDE (Tools -> ESP8266 LitteFS Data Upload).

The only difference with `ESP8266FS` is that it use `LittleFS` instead of `SPIFFS` implementation of file system, which has better performance.
