#include "JniHelper.h"

#define TAG "VaVideoEngine"

int initNative(JNIEnv* env);
int unInitNative(JNIEnv* env);

JNIEXPORT jint JNI_OnLoad(JavaVM* jvm, void* reserved)
{
    LOGI(TAG, "JNI_OnLoad libvaHsu.so");
    JNIEnv* env = nullptr;
    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }

    setJVM(jvm);

    initNative(env);
    return JNI_VERSION_1_6;
}


JNIEXPORT void JNI_OnUnload(JavaVM* jvm, void* reserved)
{
    LOGI(TAG, "JNI_OnUnload libVaHsu.so");
    JNIEnv* env = nullptr;
    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK)
    {
        unInitNative(env);
    }

    setJVM(nullptr);
}

int initNative(JNIEnv* env)
{
    LOGI(TAG, "initNative success");
    return 0;
}

int unInitNative(JNIEnv* env)
{
//    detachJvm();

    LOGI(TAG, "unInitNative success");
    return 0;
}

#include "../ffmpegWrapper/ffmpegH264DecoderWrapper.h"

static hsu::IVaVideoDecoder* s_decoder = nullptr;

extern "C" JNIEXPORT jint JNICALL
Java_com_vaHsu_decoder_ffmpeg_FFmpegDecoder_initFFmpegDecoder(JNIEnv* env, jclass type, jint width, jint height)
{
    if (s_decoder != nullptr) 
    {
        return -1;
    }

    s_decoder = new hsu::ffmpegH264DecoderWrapper();

    hsu::DecoderConfig config;
    config.width = width;
    config.height = height;
    s_decoder->init(config);

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_vaHsu_decoder_ffmpeg_FFmpegDecoder_deInitFFmpegDecoder(JNIEnv* env, jclass type)
{
    if (s_decoder == nullptr)
    {
        return -1;
    }

    s_decoder->deInit();

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_vaHsu_decoder_ffmpeg_FFmpegDecoder_processBuffer(JNIEnv* env, jclass type, jbyteArray buff, jint buffLen)
{
    
    return -1;
}