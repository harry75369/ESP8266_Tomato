#include "Music.h"
#include "Button.h"
#include "Led.h"
#include "Clock.h"
#include <Ticker.h>

/// TODO
/// 1. connect to wifi and sync datetime
/// 2. keep record of tomato clock logs
/// 3. setup web server to show logs

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

  // register tickers
  ledTicker.attach(0.02, [&]() {
    redLed.update();
    greenLed.update();
    blueLed.update();
  });
  huluwa.setPin(beeperPin);
  huanlesong.setPin(beeperPin);
  musicTicker.attach(0.02, [&]() {
    huluwa.update();
    huanlesong.update();
  });
  clockTicker.attach(0.02, [&]() {
    if (tomatoClock.isWorking()) {
      if ((millis() - tomatoClock.timeStamp) > 1000 * 60 * cfg.workMinutes) {
        greenLed.turnOff();
        redLed.turnOn();
        huluwa.play();
        huanlesong.stop();
        tomatoClock.startResting();
      }
    } else if (tomatoClock.isResting()) {
      if ((millis() - tomatoClock.timeStamp) > 1000 * 60 * cfg.restMinutes) {
        greenLed.turnOff();
        redLed.turnOff();
        huluwa.stop();
        huanlesong.play();
        tomatoClock.stop();
      }
    }
  });
  
  // debug console
  Serial.begin(115200);
  Serial.println("\nWelcome to ESP8266 Tomato Project!");
}

void loop() {
  // start working
  if (blueButton.isPressed()) {
    greenLed.turnOn();
    redLed.turnOff();
    huluwa.stop();
    huanlesong.stop();
    tomatoClock.startWorking();
  }

  // turn off notification music
  if (redButton.isPressed()) {
    huluwa.stop();
    huanlesong.stop();
  }

  // live indicator
  if (tomatoClock.isStopped()) {
    blueLed.blink(1);
  } else {
    blueLed.turnOn();
  }
}
