#ifndef __LED_H__
#define __LED_H__

class Led {
private:
  int pin;
  int state;
  bool reversed;

public:
  Led(int pin, bool reversed = false)
    : pin(pin)
    , state(0)
    , reversed(reversed)
  {}

  void turnOn() {
    state = reversed ? 0 : 1;
  }

  void turnOff() {
    state = reversed ? 1 : 0;
  }

  void toggle() {
    state = !state;
  }

  void update() {
    digitalWrite(pin, state);
  }
};

class BlinkLed : public Led {
private:
  unsigned long lastTime;

public:
  BlinkLed(int pin, bool reversed = false)
    : Led(pin, reversed)
    , lastTime(millis())
  {}

  void blink(float interval = 1.0) {
    if (millis() - lastTime >= 1000 * interval) {
      lastTime = millis();
      toggle();
    }
  }
};

#endif // __LED_H__
