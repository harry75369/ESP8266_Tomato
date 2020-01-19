struct ClockConfig {
  float workMinutes;
  float restMinutes;
};

struct TomatoClock {
  enum State {
    CLK_WORKING,
    CLK_RESTING,
    CLK_STOPPED,
  };
  State state;
  unsigned long timeStamp;

  TomatoClock() : state(CLK_STOPPED) {}

  void startWorking() {
    state = CLK_WORKING;
    timeStamp = millis();
  }

  void startResting() {
    state = CLK_RESTING;
    timeStamp = millis();
  }

  void stop() {
    state = CLK_STOPPED;
  }

  bool isWorking() {
    return (state == CLK_WORKING);
  }

  bool isResting() {
    return (state == CLK_RESTING);
  }

  bool isStopped() {
    return (state == CLK_STOPPED);
  }
};
