#pragma once

#include <stdint.h>

namespace hsu 
{
typedef struct DecoderConfig
{
  uint32_t width;
  uint32_t height;  
} DecoderConfig;


class IVaVideoDecoder
{
public:
  IVaVideoDecoder() {};
  virtual ~IVaVideoDecoder() {};

public:
  virtual int init(DecoderConfig config) = 0;
  virtual int deInit() = 0;
  virtual void processDecode(uint8_t* buffer, int bufferLen) = 0;
};
}