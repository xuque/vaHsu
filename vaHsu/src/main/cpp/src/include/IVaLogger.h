#pragma once

#include <string>

enum LogLevel
{
  kLogTrace = 0,
  kLogDebug = 1,
  kLogInfo = 2,
  kLogWarning = 3,
  kLogError = 4,
  kLogAssert = 5,
  kLogRelease = 10
};

enum LogModule
{
  kLogUnknown = 0,
  kLogPlatform = 100,
  kLogAudio = 101,
  kLogVideo = 102,
};

#define kLogTagCallBack "ycallback"
#define kLogTagThread "thread"
#define kLogTagPerf "perf"
#define kLogTagAudio "audio"
#define kLogTagVideo "video"
#define kLogTagTrans "trans"

#define Log(level, tag, format, ...)                          \
  do                                                          \
  {                                                           \
    PlatLog(level, kLogPlatform, tag, format, ##__VA_ARGS__); \
  } while (0)

#define LOGD(tag, format, ...) Log(kLogDebug, tag, format, ##__VA_ARGS__)
#define LOGI(tag, format, ...) Log(kLogInfo, tag, format, ##__VA_ARGS__)
#define LOGE(tag, format, ...) Log(kLogError, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) Log(kLogWarning, tag, format, ##__VA_ARGS__)
#define LOGT(tag, format, ...) Log(kLogTrace, tag, format, ##__VA_ARGS__)
#define LOGR(tag, format, ...) Log(kLogRelease, tag, format, ##__VA_ARGS__)

void PlatLog(LogLevel level, LogModule module, const char* tag, const char* format, ...);

typedef void (*LogCallback)(LogLevel level, LogModule module, const char* text);

class IVaLogger
{
public:
  static IVaLogger* instance();

public:
  virtual void putLog(LogLevel level, LogModule module, const std::string& tag, const char* text, bool bAssembled) = 0;

protected:
  IVaLogger() = default;
  ~IVaLogger() = default;
};