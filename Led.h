class Led {
private:
  int pin;
  int state;

public:
  Led(int pin)
    : pin(pin)
    , state(0)
  {}

  void turnOn() {
    state = 1;
  }

  void turnOff() {
    state = 0;
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
  BlinkLed(int pin)
    : Led(pin)
    , lastTime(millis())
  {}

  void blink(float interval = 1.0) {
    if (millis() - lastTime >= 1000 * interval) {
      lastTime = millis();
      toggle();
    }
  }
};
