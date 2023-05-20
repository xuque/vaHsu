#include "VaLogger.h"

#include <stdarg.h>
#include <inttypes.h>


static const uint32_t kMaxBufferSize = 2048;

LogLevel VaLogger::s_level = kLogTrace;
VaLogger* VaLogger::s_instance = nullptr;

#ifdef ANDROID
#include <android/log.h>
#include <jni.h>

static const int s_androidLogLevel[] = {ANDROID_LOG_VERBOSE,
                                        ANDROID_LOG_DEBUG,
                                        ANDROID_LOG_INFO,
                                        ANDROID_LOG_WARN,
                                        ANDROID_LOG_ERROR,
                                        ANDROID_LOG_FATAL,
                                        ANDROID_LOG_FATAL,
                                        ANDROID_LOG_FATAL,
                                        ANDROID_LOG_FATAL,
                                        ANDROID_LOG_FATAL,
                                        ANDROID_LOG_FATAL};

#endif

void PlatLog(LogLevel level, LogModule module, const char* tag, const char* format, ...)
{
 if (VaLogger::s_level > level)
 {
   return;
 }

  char buf[kMaxBufferSize] = {0};
  va_list args;

  va_start(args, format);
  vsnprintf((char*)buf, kMaxBufferSize - 1, format, args);
  va_end(args);

  buf[kMaxBufferSize - 1] = 0;

  IVaLogger::instance()->putLog(level, module, tag, buf, false);
}

IVaLogger* IVaLogger::instance()
{
  return VaLogger::instance();
}

VaLogger* VaLogger::instance()
{
  if (s_instance == nullptr)
  {
    s_instance = new VaLogger();
  }
  return s_instance;
}

VaLogger::VaLogger()
{
  
}

VaLogger::~VaLogger()
{
  
}

void VaLogger::putLog(LogLevel level, LogModule module, const std::string& tag, const char* text, bool bAssembled)
{
#if defined(ANDROID)
  __android_log_print(s_androidLogLevel[level], tag.c_str(), "%s", text);
#elif defined(__APPLE__) && defined(DEBUG)
  printf("[%s] %s\n", tag, buf);
#endif    
}
