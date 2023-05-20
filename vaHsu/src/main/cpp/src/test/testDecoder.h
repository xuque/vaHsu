#pragma once

#include "../ffmpegWrapper/ffmpegH264DecoderWrapper.h"
#include "../thread/VaXThread.h"

namespace hsu 
{
class TestDecoder : public VaThread
{
public:
  TestDecoder();
  virtual ~TestDecoder();

public:
 virtual void onThreadRun(uint32_t now) override;

public:
  void test();

private:
  IVaVideoDecoder* m_decoder;
  AVCodecContext* m_ctx;
  AVCodecParserContext* m_parser;

  uint8_t* m_inbuf;
  uint32_t m_inbufOffset;

  uint32_t m_totalDataLen;
};
}

