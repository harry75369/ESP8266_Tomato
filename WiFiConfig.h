#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

#include "JsonFile.h"

class WiFiConfig : public JsonFile {
public:
  WiFiConfig(const std::string& fn) : JsonFile(fn) {}

  virtual void reset() {
    root.reset(cJSON_CreateObject());
    write();
  }

  void saveStaConfig(const char* ssid, const char* pass) {
    root.reset(cJSON_CreateObject());
    cJSON_AddStringToObject(root.get(), "ssid", ssid);
    cJSON_AddStringToObject(root.get(), "pass", pass);
    write();
  }

  void getStaConfig(char** pSsid, char** pPass) {
    if (root) {
      cJSON* ssidNode = cJSON_GetObjectItemCaseSensitive(root.get(), "ssid");
      cJSON* passNode = cJSON_GetObjectItemCaseSensitive(root.get(), "pass");
      if (ssidNode && cJSON_IsString(ssidNode)
       && passNode && cJSON_IsString(passNode)) {
        *pSsid = ssidNode->valuestring;
        *pPass = passNode->valuestring;
        return;
      }
    }
    *pSsid = nullptr;
    *pPass = nullptr;
  }
};

#endif // __WIFI_CONFIG_H__
