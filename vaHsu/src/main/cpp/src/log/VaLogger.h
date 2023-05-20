#pragma once

#include "../include/IVaLogger.h"

class VaLogger : public IVaLogger
{
public:
  static VaLogger* instance();

public:
  virtual void putLog(LogLevel level, LogModule module, const std::string& tag, const char* text, bool bAssembled) override;

public:
  static LogLevel s_level;
  static VaLogger* s_instance;

private:
  VaLogger();
  virtual ~VaLogger();
};