#include "ffmpegH264DecoderWrapper.h"

#include "include/IVaLogger.h"
// #include "jni/JniHelper.h"

//#include "libavcodec/avcodec.h"

#include <cassert>


#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

//#define H264_FILE_TEST

namespace hsu
{
ffmpegH264DecoderWrapper::ffmpegH264DecoderWrapper()
 : m_codec(nullptr)
 , m_ctx(nullptr)
// , m_frameNum(0)
{

}

ffmpegH264DecoderWrapper::~ffmpegH264DecoderWrapper()
{

}

int ffmpegH264DecoderWrapper::init(DecoderConfig config)
{
  /*find the MPEG-1 video decoder*/
  const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
  if (!codec) 
  {
    LOGE(kLogTagVideo, "codec not found");
    exit(1);
  }

  AVCodecContext* ctx = avcodec_alloc_context3(codec);
  if (!ctx) 
  {
    LOGE(kLogTagVideo, "could not allocate video codec context");
    exit(1);
  }
 
  // if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
  // {
  //   c->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames
  // }
 
  /*For some codecs, such as msmpeg4 and mpeg4, width and height
   MUST be initialized there because this information is not
   available in the bitstream.*/
 
  /*open it*/
  if (avcodec_open2(ctx, codec, nullptr) < 0) 
  {
    LOGE(kLogTagVideo, "could not open codec");
    exit(1);
  }
 
  m_codec = codec;
  m_ctx = ctx;

  return 0;
}

int ffmpegH264DecoderWrapper::deInit()
{
  avcodec_free_context(&m_ctx);
  m_codec = nullptr;

  return 0;
}

void ffmpegH264DecoderWrapper::processDecode(uint8_t* buffer, int bufferLen)
{
#ifdef H264_FILE_TEST
  char* filename = "";
  char* outfilename = "";
  LOGI(kLogVideo, "process decode video file %s to %s", filename, outfilename);

  FILE* f = fopen(filename, "rb");
  if (!f)
  {
    LOGE(kLogTagVideo, "Could not open %s", filename);
    exit(1);
  }

  uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
  /*set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams)*/
  memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

  avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
  if (avpkt.size == 0)
  {
    break;
  }
#endif

  assert(buffer != nullptr && bufferLen > 0);

  AVPacket* avpkt = av_packet_alloc();
  assert(avpkt != nullptr);

  AVFrame* frame = av_frame_alloc();
  assert(frame != nullptr);
 
  avpkt->size = bufferLen;
  avpkt->data = buffer;

  int ret = avcodec_send_packet(m_ctx, avpkt);
  if (ret < 0) 
  {
      LOGE(kLogTagVideo, "Error sending a packet for decoding");
      exit(1);
  }

  while (ret >= 0) 
  {
    ret = avcodec_receive_frame(m_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
    {
      return;
    }
    else if (ret < 0) 
    {
      LOGE(kLogTagVideo, "Error during decoding");
      exit(1);
    }

//    printf("saving frame %3" PRId64"\n", m_ctx->frame_num);
    fflush(stdout);

    // /* the picture is allocated by the decoder. no need to free it */
    // snprintf(buf, sizeof(buf), "%s-%"PRId64, filename, dec_ctx->frame_num);
    // pgm_save(frame->data[0], frame->linesize[0],
    //          frame->width, frame->height, buf);
  }

  static FILE* outputFile = fopen("/sdcard/output.yuv", "ab");
  if (outputFile == nullptr)
  {
    return ;
  }

  if (frame != nullptr)
  {
    for (int i = 0; i < frame->height; ++i)
    {
      fwrite(frame->data[0] + i * frame->linesize[0], 1, frame->width, outputFile);
    }
  }

  av_frame_free(&frame);
  av_packet_free(&avpkt);
}

} // namespace hsu
