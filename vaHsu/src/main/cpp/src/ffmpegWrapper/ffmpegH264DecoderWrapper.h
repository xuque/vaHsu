#pragma once

#include <IVaVideoDecoder.h>

#ifdef __cplusplus
extern "C" {
#endif
//#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
//#include "libavutil/channel_layout.h"
//#include "libavutil/common.h"
//#include "libavutil/imgutils.h"
//#include "libavutil/mathematics.h"
//#include "libavutil/samplefmt.h"
#ifdef __cplusplus
}
#endif

namespace hsu
{

class ffmpegH264DecoderWrapper : public IVaVideoDecoder
{
public:
  ffmpegH264DecoderWrapper();
  virtual ~ffmpegH264DecoderWrapper();

public:
  virtual int init(DecoderConfig config) override;
  virtual int deInit() override;
  virtual void processDecode(uint8_t* buffer, int bufferLen) override;

// private:
public:
  const AVCodec* m_codec;
  AVCodecContext* m_ctx;
//  uint32_t m_frameNum;
};
} // namespace hsu
