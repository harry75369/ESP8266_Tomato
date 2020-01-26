#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "JsonFile.h"
#include <time.h>

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
  class TomatoLogger : public JsonFile {
  public:
    TomatoLogger(const std::string& fn) : JsonFile(fn) {}

    void startWorking(int nCycles, float mins) {
      cJSON* record(cJSON_CreateObject());
      time_t now = time(nullptr);
      cJSON_AddStringToObject(record, "time", ctime(&now));
      cJSON_AddNumberToObject(record, "time_t", now);
      cJSON_AddStringToObject(record, "action", "startWorking");
      cJSON_AddNumberToObject(record, "cycles", nCycles);
      cJSON_AddNumberToObject(record, "takeMinutes", mins);
      cJSON_AddItemToArray(root.get(), record);
      write();
    }

    void startResting(int nCycles, float mins) {
      cJSON* record(cJSON_CreateObject());
      time_t now = time(nullptr);
      cJSON_AddStringToObject(record, "time", ctime(&now));
      cJSON_AddNumberToObject(record, "time_t", now);
      cJSON_AddStringToObject(record, "action", "startResting");
      cJSON_AddNumberToObject(record, "cycles", nCycles);
      cJSON_AddNumberToObject(record, "takeMinutes", mins);
      cJSON_AddItemToArray(root.get(), record);
      write();
    }

    void stop(int nCycles) {
      cJSON* record(cJSON_CreateObject());
      time_t now = time(nullptr);
      cJSON_AddStringToObject(record, "time", ctime(&now));
      cJSON_AddNumberToObject(record, "time_t", now);
      cJSON_AddStringToObject(record, "action", "stop");
      cJSON_AddNumberToObject(record, "cycles", nCycles);
      cJSON_AddItemToArray(root.get(), record);
      write();
    }
  };

private:
  ClockConfig cfg;
  State state;
  int nCycles;
  unsigned long timeStamp;
  TomatoLogger logger;

public:
  TomatoClock()
    : cfg({ .workMinutes = 25, .restMinutes = 1 })
    , state(CLK_STOPPED)
    , nCycles(0)
    , logger("/logs.json")
  {}

  void startWorking(bool addCycle = true) {
    if (addCycle) {
      nCycles += 1;
    }
    state = CLK_WORKING;
    timeStamp = millis();
    logger.startWorking(nCycles, cfg.workMinutes);
  }

  void startResting() {
    state = CLK_RESTING;
    timeStamp = millis();
    logger.startResting(nCycles, cfg.restMinutes);
  }

  void stop() {
    nCycles -= 1;
    state = CLK_STOPPED;
    logger.stop(nCycles);
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

  void initLogger() {
    logger.init();
  }

  inline bool isLoggerReady() {
    return logger.isReady();
  }
};

#endif // __CLOCK_H__
