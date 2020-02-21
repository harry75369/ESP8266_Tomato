#include "Button.h"
#include "Clock.h"
#include "FileLogger.h"
#include "FileSystem.h"
#include "Led.h"
#include "Music.h"
#include "OneShot.h"
#include "WebServer.h"
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <TZ.h>
#include <time.h>

#define TOMATO_AP_SSID "ESP8266_Tomato_Clock"
#define TOMATO_AP_PASS "12345678"
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
BlinkLed blueLed(blueLedPin, true);
Ticker ledTicker;
Ticker musicTicker;
Ticker clockTicker;
Ticker mdnsTicker;
bool fsReady = false;
FileLogger logger("/logs.json");
WiFiConfig wifiConfig("/wifi.json");
TomatoClock tomatoClock(&logger);
WebServer server(&tomatoClock, &wifiConfig, &logger);

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
  fsReady = FileSystem.begin();

  // start web server
  server.start();

  // init leds
  redLed.turnOff();
  greenLed.turnOff();
  blueLed.turnOff();
}

bool isReady() {
  static String addr = TOMATO_HOST ".local";

  // wait until filesystem and logger is ready
  if (!fsReady || !logger.isReady()) {
    if (fsReady) {
      logger.init();
    }
    redLed.blink(0.1);
    return false;
  } else {
    static OneShot oneShot1;
    oneShot1.trigger([&]() {
      Serial.println("[INFO] File logger is ready.");
      redLed.turnOff();
      wifiConfig.init();
    });
  }

  static OneShot startWiFiAP;
  static OneShot startWiFiSta;
  static unsigned long wifiStaStarted;

  char* staSsid = nullptr;
  char* staPass = nullptr;
  wifiConfig.getStaConfig(&staSsid, &staPass);

  // wait until wifi config is ready
  if (!staSsid || !staPass) {
    startWiFiAP.trigger([&]() {
      // if no config, start AP to let user fill in
      WiFi.mode(WIFI_AP);
      WiFi.softAP(TOMATO_AP_SSID, TOMATO_AP_PASS);
      Serial.print("[INFO] Started Soft AP: ");
      Serial.println(TOMATO_AP_SSID);
      server.setConfigMode();
    });
    redLed.blink(1);
    return false;
  } else {
    startWiFiSta.trigger([&]() {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      WiFi.begin(staSsid, staPass);
      Serial.print("[INFO] Connecting to Wifi: ");
      Serial.println(staSsid);
      server.setNormalMode();
      wifiStaStarted = millis();
      redLed.turnOff();
    });
  }

  // wait until wifi is ready
  if (WiFi.status() != WL_CONNECTED) {
    // try this SSID for 30 seconds before reset
    if (millis() - wifiStaStarted > 1000 * 30) {
      WiFi.disconnect(true);
      startWiFiSta.reset();
      startWiFiAP.reset();
      wifiConfig.reset();
      blueLed.turnOff();
      return false;
    }
    blueLed.blink(0.1);
    return false;
  } else {
    static OneShot oneShot2;
    oneShot2.trigger([&]() {
      Serial.print("[INFO] Wifi connected: ");
      Serial.println(WiFi.localIP());
      blueLed.turnOff();
      Serial.println("[INFO] Synchronizing time...");
      delay(100); // wait network to stablize
      configTime(TZ_Asia_Shanghai, "cn.ntp.org.cn");

      if (!MDNS.begin(TOMATO_HOST)) {
        Serial.println("[ERROR] Failed to start mDNS.");
        addr = WiFi.localIP().toString();
      } else {
        mdnsTicker.attach(0.5, [&]() {
          MDNS.update();
        });
      }
    });
  }

  // wait until time is synced
  time_t now = time(nullptr);
  if (now < 1e9) {
    greenLed.blink(0.1);
    return false;
  } else {
    static OneShot oneShot3;
    oneShot3.trigger([&]() {
      Serial.print("[INFO] Time is synchronized: ");
      time_t now = time(nullptr);
      Serial.print(ctime(&now));
      greenLed.turnOff();
      logger.onSystemStart();
      Serial.print("[INFO] System finished startup. Access it at: http://");
      Serial.println(addr);
    });
  }
  return true;
}

void loop() {
  // handle web server requests
  server.handleClient();

  // wait system to be ready
  if (!isReady()) {
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
    blueLed.turnOff();
  }
}
