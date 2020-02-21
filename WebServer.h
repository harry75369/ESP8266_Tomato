#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include "Clock.h"
#include "FileLogger.h"
#include "FileSystem.h"
#include "WiFiConfig.h"
#include <ESP8266WebServer.h>
#include <functional>

#define WS_REMOTE_DEBUG

cJSON* getTimeNow() {
  time_t now = time(nullptr);
  cJSON* root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "now", now);
  return root;
}

cJSON* getStatus(TomatoClock* clock) {
  if (!clock) {
    return nullptr;
  }
  cJSON* root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "status", clock->getStatusStr());
  cJSON_AddNumberToObject(root, "cycles", clock->getNCycles());
  cJSON_AddNumberToObject(root, "workMinutes", clock->getWorkMinutes());
  cJSON_AddNumberToObject(root, "restMinutes", clock->getRestMinutes());
  cJSON_AddNumberToObject(root, "timeStamp", clock->getGlobalTimeStamp());
  return root;
}

bool updateClockConfig(TomatoClock* clock, cJSON* root) {
  if (!clock) {
    return false;
  }
  if (!root) {
    return false;
  }
  cJSON* workNode = cJSON_GetObjectItemCaseSensitive(root, "workMinutes");
  cJSON* restNode = cJSON_GetObjectItemCaseSensitive(root, "restMinutes");
  if (workNode && cJSON_IsNumber(workNode)
   && restNode && cJSON_IsNumber(restNode)) {
    return clock->updateConfig(workNode->valuedouble, restNode->valuedouble);
  }
  return false;
}

void updateWiFiConfig(WiFiConfig* wifiConfig, cJSON* root, const std::function<void()>& succeed, const std::function<void()>& fail) {
  if (!wifiConfig || !root) {
    return fail();
  }
  cJSON* ssidNode = cJSON_GetObjectItemCaseSensitive(root, "ssid");
  cJSON* passNode = cJSON_GetObjectItemCaseSensitive(root, "pass");
  if (ssidNode && cJSON_IsString(ssidNode)
   && passNode && cJSON_IsString(passNode)) {
    succeed();
    delay(500);
    wifiConfig->saveStaConfig(ssidNode->valuestring, passNode->valuestring);
    return;
   }
  return fail();
}

class WebServer {
  enum class ServerMode {
    NORMAL,
    CONFIG,
  };
  ESP8266WebServer server;
  ServerMode mode;
  TomatoClock* tomatoClock;
  WiFiConfig* wifiConfig;
  FileLogger* logger;

public:
  WebServer(TomatoClock* c = nullptr, WiFiConfig *w = nullptr, FileLogger* p = nullptr)
    : server(80)
    , mode(ServerMode::NORMAL)
    , tomatoClock(c)
    , wifiConfig(w)
    , logger(p)
  {
    server.on("/clearLogs", HTTP_PUT, [&]() {
      if (logger) {
        logger->reset();
      }
#ifdef WS_REMOTE_DEBUG
      server.sendHeader("Access-Control-Allow-Origin", "*");
#endif
      server.send(200, "text/plain", "ok");
      printRequestLog(200);
    });
    server.on("/getTimeNow", HTTP_GET, [&]() {
      cJSON_ptr root(getTimeNow());
      str_ptr jsonstr(cJSON_PrintUnformatted(root.get()));
#ifdef WS_REMOTE_DEBUG
      server.sendHeader("Access-Control-Allow-Origin", "*");
#endif
      server.send(200, "application/json", jsonstr.get());
      printRequestLog(200);
    });
    server.on("/getStatus", HTTP_GET, [&]() {
      if (!tomatoClock) {
        server.send(500, "text/plain", "Invalid tomato clock pointer.");
        printRequestLog(500);
        return;
      }
      cJSON_ptr root(getStatus(tomatoClock));
      str_ptr jsonstr(cJSON_PrintUnformatted(root.get()));
#ifdef WS_REMOTE_DEBUG
      server.sendHeader("Access-Control-Allow-Origin", "*");
#endif
      server.send(200, "application/json", jsonstr.get());
      printRequestLog(200);
    });
    server.on("/updateClockConfig", HTTP_POST, [&]() {
      cJSON_ptr root(cJSON_Parse(server.arg("plain").c_str()));
      if (updateClockConfig(tomatoClock, root.get())) {
        server.send(200, "text/plain", "ok");
      } else {
        server.send(200, "text/plain", "failed");
      }
      printRequestLog(200);
    });
    server.on("/updateWiFiConfig", HTTP_POST, [&]() {
      cJSON_ptr root(cJSON_Parse(server.arg("plain").c_str()));
      updateWiFiConfig(wifiConfig, root.get(), [&]() {
        server.send(200, "text/plain", "ok");
      }, [&]() {
        server.send(200, "text/plain", "failed");
      });
      printRequestLog(200);
    });
    server.onNotFound([&]() {
      bool ret = onRequest(server.uri());
      if (!ret) {
        server.send(404, "text/plain", "Not Found.");
      }
      printRequestLog(ret ? 200 : 404);
    });
  }

  void setNormalMode() {
    mode = ServerMode::NORMAL;
  }

  void setConfigMode() {
    mode = ServerMode::CONFIG;
  }

  void start() {
    server.begin();
  }

  void handleClient() {
    server.handleClient();
  }

  String getContentType(String filename) {
    if (server.hasArg("download")) {
      return "application/octet-stream";
    } else if (filename.endsWith(".htm")) {
      return "text/html";
    } else if (filename.endsWith(".html")) {
      return "text/html";
    } else if (filename.endsWith(".css")) {
      return "text/css";
    } else if (filename.endsWith(".js")) {
      return "application/javascript";
    } else if (filename.endsWith(".json")) {
      return "application/json";
    } else if (filename.endsWith(".png")) {
      return "image/png";
    } else if (filename.endsWith(".gif")) {
      return "image/gif";
    } else if (filename.endsWith(".jpg")) {
      return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
      return "image/x-icon";
    } else if (filename.endsWith(".xml")) {
      return "text/xml";
    } else if (filename.endsWith(".pdf")) {
      return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
      return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
      return "application/x-gzip";
    }
    return "text/plain";
  }

protected:
  void printRequestLog(int retcode = 200) {
    static const char* METHODS[] = {
      "ANY",
      "GET",
      "HEAD",
      "POST",
      "PUT",
      "PATCH",
      "DELETE",
      "OPTIONS",
    };
    Serial.print(METHODS[server.method()]);
    Serial.print(" ");
    Serial.print(server.uri());
    Serial.print(" ");
    Serial.println(retcode);
  }

  bool onRequest(String path) {
    if (path.endsWith("/")) {
      if (mode == ServerMode::NORMAL) {
        path += "index.html";
      } else {
        path += "config.html";
      }
    }
    String pathWithGz = path + ".gz";
    if (FileSystem.exists(path) || FileSystem.exists(pathWithGz)) {
      File file = FileSystem.exists(pathWithGz)
        ? FileSystem.open(pathWithGz, "r")
        : FileSystem.open(path, "r");
      String contentType = getContentType(path);
#ifdef WS_REMOTE_DEBUG
      server.sendHeader("Access-Control-Allow-Origin", "*");
#endif
      server.streamFile(file, contentType);
      file.close();
      return true;
    }
    return false;
  }
};

#endif // __WEB_SERVER_H__
