#ifndef __JSON_FILE_H__
#define __JSON_FILE_H__

#include "cJSON.h"
#include <FS.h>
#include <memory>

class JsonFile {
protected:
  bool ready;
  std::string fileName;
  std::unique_ptr<cJSON> root;

public:
  JsonFile(const std::string& fn)
    : ready(false)
    , fileName(fn)
  {}

  inline bool isReady() {
    return ready;
  }

  void init() {
    if (ready) {
      return;
    }
    if (SPIFFS.exists(fileName.c_str())) {
      Serial.print("Init JsonFile from: ");
      Serial.println(fileName.c_str());
      File f = SPIFFS.open(fileName.c_str(), "r");
      root.reset(cJSON_Parse(f.readString().c_str()));
      f.close();
    } else {
      Serial.print("New JsonFile: ");
      Serial.println(fileName.c_str());
      root.reset(cJSON_CreateArray());
    }
    ready = true;
  }

  void write() {
    if (!ready) {
      return;
    }
    File f = SPIFFS.open(fileName.c_str(), "w");
    std::unique_ptr<char> str(cJSON_PrintUnformatted(root.get()));
    f.print(str.get());
    f.close();
  }
};

#endif // __JSON_FILE_H__
