#include "testDecoder.h"

#define INBUF_SIZE 4096 


TestDecoder::TestDecoder()
 : m_decoder(nullptr)
 , m_inbufOffset(0)
 , m_totalDataLen(0)
{
    m_inbuf = new uint8_t[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
}

TestDecoder::~TestDecoder()
{

}

void TestDecoder::onThreadRun(uint32_t now)
{
    if (m_decoder == nullptr)
    {
        m_decoder = new ffmpegH264DecoderWrapper();
        m_decoder->init()
    }

    if (m_parser == nullptr)
    {
        const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
        m_parser = av_parser_init(codec->id);
    }  

    if (fRead == nullptr)
    {
        FILE* fRead = fopen("/sdcard/test.h264", "rb");
        size_t data_size = fread(m_inbuf, 1, INBUF_SIZE, f);
        if (ferror(f))
        {
            break;
        }
        eof = !data_size;
        m_totalDataLen = data_size;
    }

    uint8_t* data = nullptr;
    uint32_t dataLen = 0;
    int ret = av_parser_parse2(m_parser, m_decoder->m_ctx, &data, &dataLen, m_inbuf, m_inbuf + m_inbufOffset, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    if (ret > 0) 
    {
        m_inbufOffset += ret;
        m_decoder->processDecode(data, dataLen);
    }

    if (m_inbufOffset == m_totalDataLen)
    {
        m_totalDataLen = 0;
        stopThread();
    }
}

void TestDecoder::test()
{    
    startThread(); 
}
