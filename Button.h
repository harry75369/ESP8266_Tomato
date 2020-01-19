class Button {
private:
  int pin;
  unsigned long lastDebounceTime;
  int lastVal;
  int state;

public:
  Button(int pin)
    : pin(pin)
    , lastDebounceTime(millis())
    , lastVal(0)
    , state(0)
  {}
  
  bool isPressed(unsigned long debounceDelay = 50) {
    int currVal = digitalRead(pin);
    bool pressed = false;
    
    if (currVal != lastVal) {
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (currVal != state) {
        state = currVal;
        if (state == 0) {
          pressed = true;
        }
      }
    }
    lastVal = currVal;
    
    return pressed;
  }
};
