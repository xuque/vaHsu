#include "JniHelper.h"
#include <stdlib.h>

namespace JniHelper
{
std::string jbyteArray2str(JNIEnv* env, jbyteArray& jarray)
{
  jsize alen = env->GetArrayLength(jarray);
  jbyte* ba = env->GetByteArrayElements(jarray, JNI_FALSE);
  std::string stemp;
  stemp.assign((char*)ba, alen);
  env->ReleaseByteArrayElements(jarray, ba, 0);
  return stemp;
}

jbyteArray str2jbyteArray(JNIEnv* env, const std::string& str)
{
  jbyte* by = (jbyte*)str.c_str();
  jbyteArray jarray = env->NewByteArray(str.length()); // no release
  env->SetByteArrayRegion(jarray, 0, str.length(), by);

  return jarray;
}

jbyteArray buf2jbyteArray(JNIEnv* env, const void* buf, uint32_t bufSize)
{
  jbyte* by = (jbyte*)buf;
  jbyteArray jarray = env->NewByteArray(bufSize); // no release
  env->SetByteArrayRegion(jarray, 0, bufSize, by);

  return jarray;
}

jstring str2jstring(JNIEnv* env, const std::string& str)
{
  jsize len = str.size();

  jclass clsstring = env->FindClass("java/lang/String");
  jstring strencode = env->NewStringUTF("GB2312");

  jmethodID mid = env->GetMethodID(clsstring, "<init>", "([BLjava/lang/String;)V");
  jbyteArray barr = env->NewByteArray(len);

  env->SetByteArrayRegion(barr, 0, len, (jbyte*)str.c_str());

  jstring string = (jstring)env->NewObject(clsstring, mid, barr, strencode);

  env->DeleteLocalRef(strencode);
  env->DeleteLocalRef(barr);

  return string;
}

void deleteGlobalRef(void* object)
{
  if (object == nullptr)
  {
    return;
  }
  JNIEnv* env = nullptr;
  ATTACH_JVM(env);
  if (env == nullptr)
  {
    return;
  }
  env->DeleteGlobalRef((jobject)object);
  DETACH_JVM(env);
}

JNIEnv* attachJVM()
{
  JNIEnv* g_env = NULL;
  JNIEnv* jni_env = NULL;
  int env_status = getJVM()->GetEnv((void**)&g_env, JNI_VERSION_1_6);
  if (env_status == JNI_EDETACHED)
  {
    jint attachResult = getJVM()->AttachCurrentThread(&jni_env, NULL);
    if (attachResult < 0) jni_env = NULL;
  }
  else if (JNI_OK == env_status)
  {
    jni_env = g_env;
  }
  else
  {
    jni_env = NULL;
  }
  return jni_env;
}

void detachJVM()
{
  JNIEnv* g_env = NULL;
  int env_status = getJVM()->GetEnv((void**)&g_env, JNI_VERSION_1_6);
  if (JNI_OK == env_status)
  {
    getJVM()->DetachCurrentThread();
  }
}

} // namespace JniHelper

static JavaVM* g_JavaVM = nullptr;

void setJVM(JavaVM* vm) { g_JavaVM = vm; }

JavaVM* getJVM() { return g_JavaVM; }
