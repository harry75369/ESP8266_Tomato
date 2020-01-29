#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include "Clock.h"
#include "FileLogger.h"
#include "FileSystem.h"
#include <ESP8266WebServer.h>

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

class WebServer {
  ESP8266WebServer server;
  TomatoClock* tomatoClock;
  FileLogger* logger;

public:
  WebServer(TomatoClock* c, FileLogger* p)
    : server(80)
    , tomatoClock(c)
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
    server.onNotFound([&]() {
      bool ret = onRequest(server.uri());
      if (!ret) {
        server.send(404, "text/plain", "Not Found.");
      }
      printRequestLog(ret ? 200 : 404);
    });
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
      path += "index.html";
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
