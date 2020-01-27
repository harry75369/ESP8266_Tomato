#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "FileLogger.h"

#define WORK_MINUTES 25
#define REST_MINUTES 1

class TomatoClock {
  struct ClockConfig {
    float workMinutes;
    float restMinutes;
  };
  enum State {
    CLK_WORKING,
    CLK_RESTING,
    CLK_STOPPED,
  };

private:
  ClockConfig cfg;
  State state;
  int nCycles;
  unsigned long timeStamp;
  FileLogger *logger;

public:
  TomatoClock(FileLogger* p = nullptr)
    : cfg({ .workMinutes = WORK_MINUTES, .restMinutes = REST_MINUTES })
    , state(CLK_STOPPED)
    , nCycles(0)
    , logger(p)
  {}

  void startWorking(bool addCycle = true) {
    if (addCycle) {
      nCycles += 1;
    }
    state = CLK_WORKING;
    timeStamp = millis();
    if (logger) {
      logger->onClockStartWorking(nCycles, cfg.workMinutes, addCycle);
    }
  }

  void startResting() {
    state = CLK_RESTING;
    timeStamp = millis();
    if (logger) {
      logger->onClockStartResting(nCycles, cfg.restMinutes);
    }
  }

  void stop() {
    nCycles -= 1;
    state = CLK_STOPPED;
    if (logger) {
      logger->onClockStop(nCycles);
    }
  }

  inline int getNCycles() {
    return nCycles;
  }

  inline bool isWorking() {
    return (state == CLK_WORKING);
  }

  inline bool isResting() {
    return (state == CLK_RESTING);
  }

  inline bool isStopped() {
    return (state == CLK_STOPPED);
  }

  inline float getWorkMinutes() {
    return cfg.workMinutes;
  }

  inline float getRestMinutes() {
    return cfg.restMinutes;
  }

  inline unsigned long getTimeStamp() {
    return timeStamp;
  }
};

#endif // __CLOCK_H__
