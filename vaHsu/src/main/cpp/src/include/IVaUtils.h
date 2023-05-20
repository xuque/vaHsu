#pragma once

#include "include/IVaLock.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

#ifdef ANDROID
#include <jni.h>
#endif

template <typename T> inline T MaxValue(T v1, T v2) { return v1 > v2 ? v1 : v2; }

template <typename T> inline T MinValue(T v1, T v2) { return v1 > v2 ? v2 : v1; }

#define SafeDelete(ptr) \
  do                    \
  {                     \
    if (ptr)            \
    {                   \
      delete (ptr);     \
      (ptr) = NULL;     \
    }                   \
  } while (0)

#define IsFlagSet(e, f) (((e) & (f)) != 0)
#define SetFlag(e, f) \
  do                  \
  {                   \
    (e) |= (f);       \
  } while (0)
#define ClearFlag(e, f) \
  do                    \
  {                     \
    (e) &= ~(f);        \
  } while (0)

#define FilterFlag(e, m) ((e) & (m))
// class CValueAvgStat
// {
//  public:
//   CValueAvgStat();
//   virtual ~CValueAvgStat();

//   void init(int numAvg);
//   void AddValue(int value);
//   void GetStat(int& avg, int& minV, int& maxV);

//  private:
//   int m_min;
//   int m_max;
//   int m_idx;
//   int m_numAvg;
//   int m_Values[256];
//   long long m_totalValue;
// };

// int ConvertIntArrayToString(char* buf, const int bufsize, const unsigned int* ints, const unsigned int num);

// int AtomicInc(int* _me);
// int AtomicDec(int* _me);
// int AtomicAdd(int* _me, int val);
// int AtomicSub(int* _me, int val);

// const uint8_t* avc_find_startcode(const uint8_t* p, const uint8_t* end);
// int avc_parse_nal_units(uint8_t* buf_out, const uint8_t* buf_in, int size);
// int avc_copy_nal_units_as_mp4(uint8_t* buf_out, const uint8_t* buf_in, int size);

// return *_me
// int AtomicCmpxChg(int* _me, int oldval, int newval);

#ifndef ANDROID

/////////////////////////////// FixedSizeBuffer

class FixedSizeBufferHeader
{
 public:
  FixedSizeBufferHeader() { refCount = 0; }

  vaEngine::MediaMutex lock;
  int refCount;
};

#include <queue>

class FixedSizeBufferPool
{
 public:
  explicit FixedSizeBufferPool(int bufNum, int bufSize);
  virtual ~FixedSizeBufferPool();

  void* getBuffer();
  void* retainBuffer(void* buf) const;
  void releaseBuffer(void* buf);

 private:
  void clearBufferQueue();

  vaEngine::MediaMutex m_queueLock;
  int m_totalBufNum;
  std::deque<void*> m_freeQueue;
};

#endif

class IVaUtils
{
 public:
  static uint32_t getTickCount();
  static uint32_t getUnixTime();
  static uint64_t getRealTime();
  static bool isBiggerUint32(uint32_t src, uint32_t dest);
  static uint32_t getMapValue(const std::map<uint32_t, uint32_t>& datas, uint32_t key, uint32_t defaultVal);
  static std::string uint2string(uint32_t l);
  static std::string bigint2string(uint64_t l);
  static std::string getCurrentDateTimeString();
  static bool string2uint(const std::string& buf, uint32_t& num);
  static std::string assembleStreamKey(const std::string& streamName, uint32_t appid);
  static std::string getMapValue(const std::map<uint32_t, std::string>& datas, uint32_t key);
  static uint32_t getMapIntValue(const std::map<uint32_t, std::string>& datas, uint32_t key, uint32_t defaultVal);
  static std::string toString(int value);
  static std::string toHexRgbString(int value);
  static std::string toString(uint32_t value);
  static std::string toString(float value);
  static std::string md5String(const std::string& inStr);
  static bool isBiggerUint64(uint64_t src, uint64_t dest);
  static void doSleep(uint32_t milliseconds);
};
