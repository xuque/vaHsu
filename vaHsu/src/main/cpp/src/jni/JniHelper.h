#pragma once
#ifndef _JNIHelper_h_
#define _JNIHelper_h_

#include <jni.h>
#include <string>
#include <android/log.h>

#define LOGI(TAG, ...)  __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(TAG, ...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

namespace JniHelper
{

std::string jbyteArray2str(JNIEnv* env, jbyteArray& jstr);

jbyteArray str2jbyteArray(JNIEnv* env, const std::string& str);

jbyteArray buf2jbyteArray(JNIEnv* env, const void* buf, uint32_t bufSize);

jstring str2jstring(JNIEnv* env, const std::string& str);

void deleteGlobalRef(void* object);

JNIEnv* attachJVM();

void detachJVM();
} // namespace JniHelper

struct JavaMethodTable
{
  jmethodID& method;
  const char* name;
  const char* sign;
};

void setJVM(JavaVM* vm);

JavaVM* getJVM();

#define ATTACH_JVM(jni_env)                                            \
  JNIEnv* g_env = NULL;                                                \
  int env_status = getJVM()->GetEnv((void**)&g_env, JNI_VERSION_1_6);  \
  bool bShouldDetach = false;                                          \
  (void) bShouldDetach;                                                \
  if (env_status == JNI_EDETACHED)                                     \
  {                                                                    \
    jint attachResult = getJVM()->AttachCurrentThread(&jni_env, NULL); \
    if (attachResult >= 0)                                             \
      bShouldDetach = true;                                            \
    else                                                               \
      jni_env = NULL;                                                  \
  }                                                                    \
  else if (JNI_OK == env_status)                                       \
  {                                                                    \
    jni_env = g_env;                                                   \
  }                                                                    \
  else                                                                 \
  {                                                                    \
    jni_env = NULL;                                                    \
  }

#define DETACH_JVM(jni_env)            \
  {                                    \
    if (bShouldDetach)                 \
    {                                  \
      getJVM()->DetachCurrentThread(); \
    }                                  \
  }

#endif
