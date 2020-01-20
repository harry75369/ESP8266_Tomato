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
  int nCycles;

  TomatoClock() : state(CLK_STOPPED), nCycles(0) {}

  void startWorking(bool addCycle = true) {
    if (addCycle) {
      nCycles += 1;
    }
    state = CLK_WORKING;
    timeStamp = millis();
  }

  void startResting() {
    state = CLK_RESTING;
    timeStamp = millis();
  }

  void stop() {
    nCycles -= 1;
    state = CLK_STOPPED;
  }

  int getNCycles() {
    return nCycles;
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
