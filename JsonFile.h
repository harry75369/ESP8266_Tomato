#ifndef __JSON_FILE_H__
#define __JSON_FILE_H__

#include "FileSystem.h"
#include "cJSON.h"
#include <memory>

#define MAX_FILE_SIZE (1024 * 1024)

struct CJsonDeleter {
  void operator()(cJSON* json) {
    if (json) {
      cJSON_Delete(json);
    }
  }
};

struct StrDeleter {
  void operator()(char* str) {
    if (str) {
      free(str);
    }
  }
};

typedef std::unique_ptr<cJSON, CJsonDeleter> cJSON_ptr;
typedef std::unique_ptr<char, StrDeleter> str_ptr;

class JsonFile {
protected:
  bool ready;
  std::string fileName;
  cJSON_ptr root;

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
    if (FileSystem.exists(fileName.c_str())) {
      Serial.print("[INFO] Initializing json from file: ");
      Serial.println(fileName.c_str());
      File f = FileSystem.open(fileName.c_str(), "r");
      if (f.size() > 0 && f.size() < MAX_FILE_SIZE) {
        String content = f.readString();
        cJSON* p = cJSON_Parse(content.c_str());
        if (p) {
          root.reset(p);
          ready = true;
        } else {
          Serial.println("[ERROR] Failed to parse this file as json.");
        }
      } else {
        Serial.println("[WARN] Empty file or exceeds max file size.");
      }
      f.close();
    }
    if (!ready) {
      Serial.print("[INFO] Create new json file: ");
      Serial.println(fileName.c_str());
      ready = true;
      reset();
    }
  }

  virtual void reset() {
    root.reset(cJSON_CreateArray());
    write();
  }

  void write() {
    if (!ready) {
      return;
    }
    File f = FileSystem.open(fileName.c_str(), "w");
    str_ptr str(cJSON_PrintUnformatted(root.get()));
    f.print(str.get());
    f.close();
  }
};

#endif // __JSON_FILE_H__
