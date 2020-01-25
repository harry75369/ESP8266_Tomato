#include "Music.h"
#include "Button.h"
#include "Led.h"
#include "Clock.h"
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <TZ.h>                         // TZ_Asia_Shanghai
#include <time.h>                       // time() ctime()

/// TODO
/// 1. connect to wifi and sync datetime [done]
/// 2. keep record of tomato clock logs
/// 3. setup web server to show logs

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

// pin mappings for WeMos D1 mini
const int d0 = D2;
const int d1 = D3;
const int d2 = D4;
const int d3 = D8;
const int d4 = D9;
const int d5 = D5;
const int d6 = D6;
const int d7 = D7;
const int d8 = D10;

// functional component pins
const int beeperPin = d1;
const int redButtonPin = d7;
const int blueButtonPin = d6;
const int redLedPin = d5;
const int greenLedPin = d0;
const int blueLedPin = LED_BUILTIN;
//const int blePWRC = d3;
//const int bleStatPin = d2;

// global variables
Button redButton(redButtonPin);
Button blueButton(blueButtonPin);
Led redLed(redLedPin);
Led greenLed(greenLedPin);
BlinkLed blueLed(blueLedPin);
ClockConfig cfg = {
  .workMinutes = 25,
  .restMinutes = 5,
};
TomatoClock tomatoClock;
Ticker ledTicker;
Ticker musicTicker;
Ticker clockTicker;
Ticker wifiTicker;

void tellCycles(int counter) {
  static int lastCounter = 0;
  if (lastCounter == counter) {
    return;
  } else {
    lastCounter = counter;
  }
  for (int i = 0; i < counter; i++) {
    tone(beeperPin, NOTE_D3);
    delay(200);
    noTone(beeperPin);
  }
  if (counter > 0) {
    tone(beeperPin, NOTE_D1);
    delay(200);
    noTone(beeperPin);
  }
}

void stopAllMusic() {
  huluwa.stop();
  huanlesong.stop();
  taiyangzhaochangshengqi.stop();
}

void setup() {
  // outputs
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(beeperPin, OUTPUT);
  //pinMode(blePWRC, OUTPUT);

  // inputs
  pinMode(redButtonPin, INPUT_PULLUP);
  pinMode(blueButtonPin, INPUT_PULLUP);
  //pinMode(bleStatPin, INPUT);

  // setup music pin
  huluwa.setPin(beeperPin);
  huanlesong.setPin(beeperPin);
  taiyangzhaochangshengqi.setPin(beeperPin);

  // register tickers
  ledTicker.attach(0.02, [&]() {
    redLed.update();
    greenLed.update();
    blueLed.update();
  });
  musicTicker.attach(0.02, [&]() {
    huluwa.update();
    huanlesong.update();
    taiyangzhaochangshengqi.update();
  });
  clockTicker.attach(0.02, [&]() {
    if (tomatoClock.isWorking()) {
      if ((millis() - tomatoClock.timeStamp) > 1000 * 60 * cfg.workMinutes) {
        greenLed.turnOff();
        redLed.turnOn();
        stopAllMusic();
        huluwa.playFromStart();
        tomatoClock.startResting();
      }
    } else if (tomatoClock.isResting()) {
      if ((millis() - tomatoClock.timeStamp) > 1000 * 60 * cfg.restMinutes) {
        greenLed.turnOff();
        redLed.turnOff();
        stopAllMusic();
        tomatoClock.stop();

        int remainingCycles = tomatoClock.getNCycles();
        if (remainingCycles > 0) {
          greenLed.turnOn();
          taiyangzhaochangshengqi.playFromStart();
          tomatoClock.startWorking(false);
        } else {
          huanlesong.playFromStart();
        }
      }
    }
  });
  
  // debug console
  Serial.begin(115200);
  Serial.println("\nWelcome to ESP8266 Tomato Project!");

  // init wifi and time
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  wifiTicker.attach(0.02, [&]() {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Wifi connected: ");
      Serial.println(WiFi.localIP());
      wifiTicker.detach();
      configTime(TZ_Asia_Shanghai, "cn.ntp.org.cn");
    }
  });
}

void loop() {
  // wait until wifi is ready
  if (WiFi.status() != WL_CONNECTED) {
    blueLed.blink(0.1);
    return;
  }

  // start working
  if (blueButton.isPressed()) {
    greenLed.turnOn();
    redLed.turnOff();
    stopAllMusic();
    tomatoClock.startWorking();
  }
  tellCycles(tomatoClock.getNCycles());

  // turn off notification music
  if (redButton.isPressed()) {
    stopAllMusic();
  }

  // live indicator
  if (tomatoClock.isStopped()) {
    blueLed.blink(1);
  } else {
    blueLed.turnOn();
  }
}
