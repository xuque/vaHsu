#include "include/IVaUtils.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <string>
#include "common/SysTime.h"
#include "common/encrypted/md5.h"

#ifndef __ANDROID__
#include <sys/timeb.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef _WIN32
#define DW_ATOMIC_INCR(p) InterlockedIncrement((volatile long*)p)
#define DW_ATOMIC_DECR(p) InterlockedDecrement((volatile long*)p)
#define DW_ATOMIC_ADD(p, val) InterlockedExchangeAdd((volatile long*)p, val)
#define DW_ATOMIC_SUB(p, val) InterlockedExchangeAdd((volatile long*)p, -val)
#define DW_ATOMIC_COMPARE_EXCHANGE(p, oldval, newval) InterlockedCompareExchange((volatile long*)p, newval, oldval)
#else
#define DW_ATOMIC_INCR(p) __sync_add_and_fetch(p, 1)
#define DW_ATOMIC_DECR(p) __sync_sub_and_fetch(p, 1)
#define DW_ATOMIC_ADD(p, val) __sync_add_and_fetch((volatile long*)p, val)
#define DW_ATOMIC_SUB(p, val) __sync_sub_and_fetch((volatile long*)p, val)
#define DW_ATOMIC_COMPARE_EXCHANGE(p, oldval, newval) __sync_val_compare_and_swap(_me, oldval, newval)
#endif

// CValueAvgStat::CValueAvgStat() : m_min(0x7FFFFFFF), m_max(-1), m_idx(0), m_numAvg(2)
// {
//   m_totalValue = 0;
//   memset(m_Values, 0, sizeof(int) * 256);
// }

// CValueAvgStat::~CValueAvgStat() {}

// void CValueAvgStat::init(int numAvg)
// {
//   m_numAvg = numAvg;
//   if (m_numAvg > 256)
//   {
//     m_numAvg = 256;
//   }
//   if (m_numAvg < 2)
//   {
//     m_numAvg = 2;
//   }

//   m_max = -1;
//   m_min = 0x7FFFFFFF;
//   m_idx = 0;
//   m_totalValue = 0;

//   memset(m_Values, 0, sizeof(int) * 256);
// }

// void CValueAvgStat::AddValue(int value)
// {
//   int realIdx = m_idx % m_numAvg;
//   int oldValue = m_Values[realIdx];
//   m_Values[realIdx] = value;

//   if (m_idx >= m_numAvg && (m_min == oldValue || m_max == oldValue))
//   {
//     m_max = -1;
//     m_min = 0x7FFFFFFF;

//     for (int i = 0; i < m_numAvg; i++)
//     {
//       if (m_Values[i] < m_min) m_min = m_Values[i];
//       if (m_Values[i] > m_max) m_max = m_Values[i];
//     }
//   }
//   else
//   {
//     if (value < m_min) m_min = value;
//     if (value > m_max) m_max = value;
//   }

//   m_totalValue += value - oldValue;
//   m_idx++;
// }

// void CValueAvgStat::GetStat(int& avgValue, int& minValue, int& maxValue)
// {
//   minValue = m_min;
//   maxValue = m_max;

//   if (m_idx == 0)
//   {
//     avgValue = 0;
//   }
//   else if (m_idx > m_numAvg)
//   {
//     avgValue = (int)(m_totalValue / m_numAvg);
//   }
//   else
//   {
//     avgValue = (int)(m_totalValue / m_idx);
//   }
// }

// int ConvertIntArrayToString(char* buf, const int bufsize, const unsigned int* ints, const unsigned int num)
// {
//   buf[0] = 0;
//   if (bufsize < (int)(num * 12) || buf == NULL || ints == NULL || num <= 0) return -1; // 0xFFFFFFFF

//   char* writeptr = buf;
//   for (unsigned int i = 0; i < num; i++)
//   {
//     sprintf(writeptr, " %d", ints[i]);
//     writeptr += strlen(writeptr);
//   }

//   return 0;
// }

// int AtomicInc(int* _me) { return DW_ATOMIC_INCR(_me); }

// int AtomicDec(int* _me) { return DW_ATOMIC_DECR(_me); }

// int AtomicAdd(int* _me, int val) { return (int)DW_ATOMIC_ADD(_me, val); }

// int AtomicSub(int* _me, int val) { return (int)DW_ATOMIC_SUB(_me, val); }

// int AtomicCmpxChg(int* _me, int oldval, int newval) { return DW_ATOMIC_COMPARE_EXCHANGE(_me, oldval, newval); }

// static const uint8_t* avc_find_startcode_internal(const uint8_t* p, const uint8_t* end)
// {
//   const uint8_t* a = p + 4 - ((intptr_t)p & 3);
//   for (end -= 3; p < a && p < end; p++)
//   {
//     if (p[0] == 0 && p[1] == 0 && p[2] == 1) return p;
//   }

//   for (end -= 3; p < end; p += 4)
//   {
//     uint32_t x = *(const uint32_t*)p;
//     if ((x - 0x01010101) & (~x) & 0x80808080)
//     { // generic
//       if (p[1] == 0)
//       {
//         if (p[0] == 0 && p[2] == 1) return p;
//         if (p[2] == 0 && p[3] == 1) return p + 1;
//       }
//       if (p[3] == 0)
//       {
//         if (p[2] == 0 && p[4] == 1) return p + 2;
//         if (p[4] == 0 && p[5] == 1) return p + 3;
//       }
//     }
//   }

//   for (end += 3; p < end; p++)
//   {
//     if (p[0] == 0 && p[1] == 0 && p[2] == 1) return p;
//   }
//   return end + 3;
// }

// const uint8_t* avc_find_startcode(const uint8_t* p, const uint8_t* end)
// {
//   const uint8_t* out = avc_find_startcode_internal(p, end);
//   if (p < out && out < end && !out[-1]) out--;

//   return out;
// }

// int avc_parse_nal_units(uint8_t* buf_out, const uint8_t* buf_in, int size)
// {
//   const uint8_t* p = buf_in;
//   const uint8_t* end = p + size;
//   const uint8_t *nal_start, *nal_end;
//   uint8_t* pb = buf_out;
//   size = 0;
//   nal_start = avc_find_startcode(p, end);

//   if (nal_start < end)
//   {
//     while (!*(nal_start++))
//     {
//     }
//     nal_end = avc_find_startcode(nal_start, end);

//     int nal_size = (int)(nal_end - nal_start);
//     pb[0] = nal_size >> 24;
//     pb[1] = nal_size >> 16;
//     pb[2] = nal_size >> 8;
//     pb[3] = nal_size;
//     memcpy(pb + 4, nal_start, nal_size);

//     size += 4 + nal_size;
//   }

//   return size;
// }

// int avc_copy_nal_units_as_mp4(uint8_t* buf_out, const uint8_t* buf_in, int size)
// {
//   const uint8_t *nal_start, *end, *nal_end;
//   int nCount = 0;
//   end = buf_in + size;
//   nal_start = avc_find_startcode(buf_in, end);
//   while (nal_start < end)
//   {
//     while (!*(nal_start++))
//     {
//     }
//     nal_end = avc_find_startcode(nal_start, end);
//     int nal_size = (int)(nal_end - nal_start);
//     memcpy(buf_out + nCount + 4, nal_start, nal_size);
//     uint8_t* pb = buf_out + nCount;
//     pb[0] = nal_size >> 24;
//     pb[1] = nal_size >> 16;
//     pb[2] = nal_size >> 8;
//     pb[3] = nal_size;
//     nCount += nal_size + 4;
//     nal_start = nal_end;
//   }
//   return nCount;
// }

#ifndef ANDROID

///////////////////// FixedSizeBuffer

FixedSizeBufferPool::FixedSizeBufferPool(int bufNum, int bufSize)
{
  for (int i = 0; i < bufNum; i++)
  {
    void* memPtr = malloc(bufSize + sizeof(FixedSizeBufferHeader*));
    if (!memPtr)
    {
      clearBufferQueue();
      return;
    }
    FixedSizeBufferHeader* preFixed = new FixedSizeBufferHeader;
    *((FixedSizeBufferHeader**)memPtr) = preFixed;

    vaEngine::MutexStackLock guard(m_queueLock);
    m_freeQueue.push_back(memPtr);
  }
  m_totalBufNum = bufNum;
}

FixedSizeBufferPool::~FixedSizeBufferPool()
{
  if (m_totalBufNum != m_freeQueue.size())
  {
    //        NSLog(@"FixedSizeBufferPool::~FixedSizeBufferPool : some buffers not recycled!");
  }

  clearBufferQueue();
}

void FixedSizeBufferPool::clearBufferQueue()
{
  void* memPtr;
  for (;;)
  {
    {
      vaEngine::MutexStackLock guard(m_queueLock);
      if (m_freeQueue.size() <= 0)
      {
        break;
      }
      memPtr = m_freeQueue.front();
      m_freeQueue.pop_front();
    }

    //        if (!memPtr) {
    //            assert(false);
    //            NSLog(@"FixedSizeBufferPool::ClearBufferQueue : why null mem pointer???");
    //        }

    assert(memPtr != NULL);
    FixedSizeBufferHeader* header = *((FixedSizeBufferHeader**)memPtr);
    if (header)
    {
      delete header;
    }
    free(memPtr);
  }
}

void* FixedSizeBufferPool::getBuffer()
{
  void* memPtr;
  {
    vaEngine::MutexStackLock guard(m_queueLock);
    if (m_freeQueue.size() == 0)
    {
      return NULL;
    }
    memPtr = m_freeQueue.front();
    m_freeQueue.pop_front();
  }

  FixedSizeBufferHeader* header = *((FixedSizeBufferHeader**)memPtr);

  vaEngine::MutexStackLock guard(header->lock);
  header->refCount++;
  return ((char*)memPtr + sizeof(FixedSizeBufferHeader*));
}

void* FixedSizeBufferPool::retainBuffer(void* buf) const
{
  if (!buf)
  {
    return NULL;
  }
  void* memPtr = ((char*)buf - sizeof(FixedSizeBufferHeader*));
  FixedSizeBufferHeader* header = *((FixedSizeBufferHeader**)memPtr);

  vaEngine::MutexStackLock guard(header->lock);
  if (header->refCount == 0)
  {
    return NULL;
  }
  header->refCount++;
  return buf;
}

void FixedSizeBufferPool::releaseBuffer(void* buf)
{
  if (!buf)
  {
    return;
  }

  void* memPtr = ((char*)buf - sizeof(FixedSizeBufferHeader*));
  FixedSizeBufferHeader* header = *((FixedSizeBufferHeader**)memPtr);

  bool recycle = false;
  {
    vaEngine::MutexStackLock guard(header->lock);
    header->refCount--;
    if (header->refCount == 0)
    {
      recycle = true;
    }
  }

  {
    vaEngine::MutexStackLock guard(m_queueLock);
    if (recycle)
    {
      m_freeQueue.push_back(memPtr);
    }
  }
}
#endif

#define USEC_PER_SEC 1000000ull
uint32_t IVaUtils::getTickCount()
{
#if defined(__ANDROID__)
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (uint32_t)((uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / USEC_PER_SEC);
#elif defined(_WIN32)
  return timeGetTime();
#elif defined(__APPLE__)
  if (__builtin_available(iOS 10.0, macOS 10.12, *))
  {
    struct timespec tsNowTime;
    clock_gettime(CLOCK_MONOTONIC, &tsNowTime);
    return (uint32_t)((uint64_t)tsNowTime.tv_sec * 1000 + (uint64_t)tsNowTime.tv_nsec / 1000000);
  }
  else
  {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (uint32_t)(((uint64_t)now.tv_sec * 1000000 + now.tv_usec) / 1000);
  }
#else
  #error Unspecified Platform for func(getTickCount)!
#endif
}

uint32_t IVaUtils::getUnixTime()
{
  time_t tm = time(NULL);
  return (static_cast<uint32_t>(tm));
}

uint64_t IVaUtils::getRealTime()
{
#if defined(__ANDROID__)
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return ((uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / USEC_PER_SEC);
#else
  struct timeb now;
  ftime(&now);
  return ((uint64_t)now.time * 1000 + now.millitm);
#endif
}

bool IVaUtils::isBiggerUint32(uint32_t src, uint32_t dest) { return (src != dest && src - dest < 0x7fffffff); }

uint32_t IVaUtils::getMapValue(const std::map<uint32_t, uint32_t>& datas, uint32_t key, uint32_t defaultVal)
{
  if (!datas.empty())
  {
    std::map<uint32_t, uint32_t>::const_iterator it = datas.find(key);
    if (it != datas.end())
    {
      return it->second;
    }
  }

  return defaultVal;
}

std::string IVaUtils::uint2string(uint32_t l)
{
  std::string str;
  char tmp[100];
  snprintf(tmp, sizeof(tmp), "%u", l);
  str = tmp;
  return str;
}

std::string IVaUtils::bigint2string(uint64_t l)
{
  std::string str;
  uint64_t tmp = l;
  while (tmp)
  {
    uint64_t a = tmp % 10;
    str = (char)(a + 48) + str;
    tmp /= 10;
  }
  if (!str.size())
  {
    str = "0";
  }
  return str;
}

std::string IVaUtils::getCurrentDateTimeString()
{
  struct timeval curTime;
  gettimeofday(&curTime, NULL);

  char buf[100];
  strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", localtime((time_t*)&curTime.tv_sec));

  std::ostringstream os;
  os << buf << std::setfill('0') << std::setw(3) << (curTime.tv_usec / 1000);

  return os.str();
}

bool IVaUtils::string2uint(const std::string& buf, uint32_t& num)
{
  const uint64_t kMaxUInt = uint32_t(-1);
  num = 0;

  uint64_t val = 0;
  for (uint32_t i = 0; i < buf.size(); ++i)
  {
    char c = buf[i];
    if (c < '0' || c > '9')
    {
      return false;
    }
    val = val * 10 + (c - '0');
    if (val >= kMaxUInt)
    {
      return false;
    }
  }
  num = uint32_t(val);
  return true;
}

std::string IVaUtils::assembleStreamKey(const std::string& streamName, uint32_t appid)
{
  std::stringstream oss;
  if (!streamName.empty())
  {
    oss << appid << "_" << streamName;
  }
  return oss.str();
}

std::string IVaUtils::getMapValue(const std::map<uint32_t, std::string>& datas, uint32_t key)
{
  std::map<uint32_t, std::string>::const_iterator it = datas.find(key);
  if (it == datas.end())
  {
    std::string defaultVal = "";
    return defaultVal;
  }
  return it->second;
}

uint32_t IVaUtils::getMapIntValue(const std::map<uint32_t, std::string>& datas,
                                         uint32_t key,
                                         uint32_t defaultVal)
{
  std::map<uint32_t, std::string>::const_iterator it = datas.find(key);
  if (it == datas.end() || it->second.empty())
  {
    return defaultVal;
  }
  return (uint32_t)strtoul(it->second.c_str(), NULL, 10);
}

std::string IVaUtils::toString(int value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string IVaUtils::toHexRgbString(int value)
{
  std::stringstream oss;
  if (-1 == value)
  {
    oss << "";
  }
  else
  {
    oss << std::setfill('0') << std::setw(6) << std::hex << value;
  }
  return oss.str();
}

std::string IVaUtils::toString(uint32_t value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string IVaUtils::toString(float value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

bool IVaUtils::isBiggerUint64(uint64_t src, uint64_t dest)
{
  return (src != dest && src - dest < 0x7fffffffffffffff);
};

void IVaUtils::doSleep(uint32_t milliseconds)
{
#ifdef _WIN32
  Sleep(milliseconds);
#else
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = milliseconds * 1000;
  select(0, NULL, NULL, NULL, &tv);
#endif
}
