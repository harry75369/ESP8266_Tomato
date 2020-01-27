#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include "FileLogger.h"
#include <ESP8266WebServer.h>

class WebServer {
  ESP8266WebServer server;
  FileLogger* logger;

public:
  WebServer(FileLogger* p)
    : server(80)
    , logger(p)
  {
    server.on("/clearLogs", HTTP_PUT, [&]() {
      if (logger) {
        logger->reset();
      }
      server.send(200, "text/plain", "ok");
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
    if (SPIFFS.exists(path) || SPIFFS.exists(pathWithGz)) {
      File file = SPIFFS.exists(pathWithGz)
        ? SPIFFS.open(pathWithGz, "r")
        : SPIFFS.open(path, "r");
      String contentType = getContentType(path);
      server.streamFile(file, contentType);
      file.close();
      return true;
    }
    return false;
  }
};

#endif // __WEB_SERVER_H__
