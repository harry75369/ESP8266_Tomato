#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#define USE_LITTLEFS

#ifdef USE_LITTLEFS
#include <LittleFS.h>
#define FileSystem LittleFS
#else
#include <FS.h>
#define FileSystem SPIFFS
#endif

#endif // __FILE_SYSTEM_H__
