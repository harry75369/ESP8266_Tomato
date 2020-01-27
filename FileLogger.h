#ifndef __FILE_LOGGER_H__
#define __FILE_LOGGER_H__

#include "JsonFile.h"
#include <time.h>

class FileLogger : public JsonFile {
public:
  FileLogger(const std::string& fn) : JsonFile(fn) {}

  void onSystemStart() {
    cJSON* record(cJSON_CreateObject());
    time_t now = time(nullptr);
    cJSON_AddStringToObject(record, "time", ctime(&now));
    cJSON_AddNumberToObject(record, "time_t", now);
    cJSON_AddStringToObject(record, "action", "systemStart");
    cJSON_AddItemToArray(root.get(), record);
    write();
  }

  void onClockStartWorking(int nCycles, float mins, bool addNew) {
    cJSON* record(cJSON_CreateObject());
    time_t now = time(nullptr);
    cJSON_AddStringToObject(record, "time", ctime(&now));
    cJSON_AddNumberToObject(record, "time_t", now);
    cJSON_AddStringToObject(record, "action", "clockStartWorking");
    cJSON_AddStringToObject(record, "startType", addNew ? "new" : "continue");
    cJSON_AddNumberToObject(record, "cycles", nCycles);
    cJSON_AddNumberToObject(record, "takeMinutes", mins);
    cJSON_AddItemToArray(root.get(), record);
    write();
  }

  void onClockStartResting(int nCycles, float mins) {
    cJSON* record(cJSON_CreateObject());
    time_t now = time(nullptr);
    cJSON_AddStringToObject(record, "time", ctime(&now));
    cJSON_AddNumberToObject(record, "time_t", now);
    cJSON_AddStringToObject(record, "action", "clockStartResting");
    cJSON_AddNumberToObject(record, "cycles", nCycles);
    cJSON_AddNumberToObject(record, "takeMinutes", mins);
    cJSON_AddItemToArray(root.get(), record);
    write();
  }

  void onClockStop(int nCycles) {
    cJSON* record(cJSON_CreateObject());
    time_t now = time(nullptr);
    cJSON_AddStringToObject(record, "time", ctime(&now));
    cJSON_AddNumberToObject(record, "time_t", now);
    cJSON_AddStringToObject(record, "action", "clockStop");
    cJSON_AddNumberToObject(record, "cycles", nCycles);
    cJSON_AddItemToArray(root.get(), record);
    write();
  }
};


#endif // __FILE_LOGGER_H__
