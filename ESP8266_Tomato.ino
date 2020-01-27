#include "Button.h"
#include "Clock.h"
#include "FileLogger.h"
#include "Led.h"
#include "Music.h"
#include "OneShot.h"
#include "WebServer.h"
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <TZ.h>
#include <time.h>

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

#define TOMATO_HOST "tomato"

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
BlinkLed redLed(redLedPin);
BlinkLed greenLed(greenLedPin);
BlinkLed blueLed(blueLedPin);
Ticker ledTicker;
Ticker musicTicker;
Ticker clockTicker;
Ticker mdnsTicker;
bool fsReady = false;
FileLogger logger("/logs.json");
TomatoClock tomatoClock(&logger);
WebServer server(&logger);

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
      if ((millis() - tomatoClock.getTimeStamp()) > 1000 * 60 * tomatoClock.getWorkMinutes()) {
        greenLed.turnOff();
        redLed.turnOn();
        stopAllMusic();
        huluwa.playFromStart();
        tomatoClock.startResting();
      }
    } else if (tomatoClock.isResting()) {
      if ((millis() - tomatoClock.getTimeStamp()) > 1000 * 60 * tomatoClock.getRestMinutes()) {
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

  // start serial console
  Serial.begin(115200);
  Serial.println("\n--- Welcome to ESP8266 Tomato Clock! ---");

  // init file system
  fsReady = SPIFFS.begin();

  // start web server
  server.start();
}

bool isReady() {
  static OneShot readyCb1([&]() {
    Serial.println("[INFO] File logger is ready.");
    redLed.turnOff();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("[INFO] Connecting to Wifi: ");
    Serial.println(WIFI_SSID);
  });
  static OneShot readyCb2([&]() {
    Serial.print("[INFO] Wifi connected: ");
    Serial.println(WiFi.localIP());
    blueLed.turnOn();
    Serial.println("[INFO] Synchronizing time...");
    delay(100); // wait network to stablize
    configTime(TZ_Asia_Shanghai, "cn.ntp.org.cn");

    if (!MDNS.begin(TOMATO_HOST)) {
      Serial.println("[ERROR] Failed to start mDNS.");
    } else {
      mdnsTicker.attach(0.5, [&]() {
        MDNS.update();
      });
    }
  });
  static OneShot readyCb3([&]() {
    Serial.print("[INFO] Time is synchronized: ");
    time_t now = time(nullptr);
    Serial.print(ctime(&now));
    greenLed.turnOff();
    logger.onSystemStart();
    Serial.print("[INFO] System finished startup. Access it at: http://");
    Serial.print(TOMATO_HOST);
    Serial.println(".local/");
  });

  // wait until filesystem and logger is ready
  if (!fsReady || !logger.isReady()) {
    if (fsReady) {
      logger.init();
    }
    redLed.blink(0.1);
    return false;
  } else {
    readyCb1.trigger();
  }

  // wait until wifi is ready
  if (WiFi.status() != WL_CONNECTED) {
    blueLed.blink(0.1);
    return false;
  } else {
    readyCb2.trigger();
  }

  // wait until time is synced
  time_t now = time(nullptr);
  if (now < 1e9) {
    greenLed.blink(0.1);
    return false;
  } else {
    readyCb3.trigger();
  }
  return true;
}

void loop() {
  // wait system to be ready
  if (!isReady()) {
    return;
  }

  // handle web server requests
  server.handleClient();

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
