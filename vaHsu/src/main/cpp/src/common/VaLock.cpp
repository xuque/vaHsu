
#include "include/IVaLock.h"
#include <stdlib.h>
#include "common/SysTime.h"

using namespace vaEngine;

#ifdef _WIN32

MediaMutexLock::MediaMutexLock() : iLockCnt(0) { ::InitializeCriticalSection(&iHandle); }

MediaMutexLock::~MediaMutexLock() { ::DeleteCriticalSection(&iHandle); }

MediaMutexLock::MediaMutexLock(const MediaMutexLock& other) { m_lastLockId = -1; }

MediaMutexLock& MediaMutexLock::operator=(const MediaMutexLock& other) { return *this; }

void MediaMutexLock::Lock(int lastlockid)
{
  ::EnterCriticalSection(&iHandle);
  m_lastLockId = (m_lastLockId << 8) + lastlockid;
  ++iLockCnt;
}

void MediaMutexLock::Unlock()
{
  --iLockCnt;
  m_lastLockId = -1;
  ::LeaveCriticalSection(&iHandle);
}

int MediaMutexLock::GetLockCnt() const { return iLockCnt; }

MediaMutex::MediaMutex() { ::InitializeCriticalSection(&m_mutex); }

MediaMutex::~MediaMutex()
{
  //    PlatAssert(iLockCnt == 0, "no released");
  ::DeleteCriticalSection(&m_mutex);
}

MediaMutex::MediaMutex(const MediaMutex& other)
{
  //  PlatAssert(false, "no implementation");
}

MediaMutex& MediaMutex::operator=(const MediaMutex& other)
{
  //   PlatAssert(false, "no implementation");
  return *this;
}

void MediaMutex::Lock() { ::EnterCriticalSection(&m_mutex); }

void MediaMutex::Unlock() { ::LeaveCriticalSection(&m_mutex); }

/////////// MediaEvent
MediaEvent::MediaEvent() : m_event(NULL)
{
  m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  ::InitializeCriticalSection(&m_mute);
}

MediaEvent::~MediaEvent()
{
  ::DeleteCriticalSection(&m_mute);
  ::CloseHandle(m_event);
  m_event = NULL;
}

MediaEvent::MediaEvent(const MediaEvent& other)
{
  //   PlatAssert(false, "no implementation");
}

MediaEvent& MediaEvent::operator=(const MediaEvent& other)
{
  //    PlatAssert(false, "no implementation");
  return *this;
}

bool MediaEvent::Wait(unsigned int timeoutMS)
{
  ::EnterCriticalSection(&m_mute);
  ::WaitForSingleObject(m_event, timeoutMS);
  ::LeaveCriticalSection(&m_mute);
  return true;
}

bool MediaEvent::Wait() { return Wait(INFINITE); }

void MediaEvent::Signal() { ::ResetEvent(m_event); }

/////////////////// MutexStackLock
MutexStackLock::MutexStackLock(MediaMutex& mutex) : iMutex(mutex) { iMutex.Lock(); }

MutexStackLock::~MutexStackLock() { iMutex.Unlock(); }

MutexStackLock& MutexStackLock::operator=(const MutexStackLock& other)
{
  //  LibAssert(false, "no impl");
  return *this;
}

MutexStackLock::MutexStackLock(const MutexStackLock& other) : iMutex(other.iMutex)
{
  // LibAssert(false, "no impl");
}

RWLock::RWLock()
{
  m_writeSem = ::CreateSemaphore(NULL, 1, 1, NULL);
  m_readSem = ::CreateSemaphore(NULL, MAX_READ_NUM, MAX_READ_NUM, NULL);
}

RWLock::~RWLock()
{
  ::CloseHandle(m_writeSem);
  ::CloseHandle(m_readSem);
}

void RWLock::acquireLockExclusive()
{
  WaitForSingleObject(m_writeSem, INFINITE);
  for (int i = 0; i < MAX_READ_NUM; ++i)
  {
    WaitForSingleObject(m_readSem, INFINITE);
  }
}

void RWLock::releaseLockExclusive()
{
  ReleaseSemaphore(m_readSem, MAX_READ_NUM, NULL);
  ReleaseSemaphore(m_writeSem, 1, NULL);
}

void RWLock::rcquireLockShared()
{
  WaitForSingleObject(m_writeSem, INFINITE);
  ReleaseSemaphore(m_writeSem, 1, NULL);
  WaitForSingleObject(m_readSem, INFINITE);
}

void RWLock::releaseLockShared() { ReleaseSemaphore(m_readSem, 1, NULL); }

#else
MediaMutexLock::MediaMutexLock() : iHandle(malloc(sizeof(pthread_mutex_t))), iLockCnt(0)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init((pthread_mutex_t*)iHandle, &attr);
  pthread_mutexattr_destroy(&attr);
}

MediaMutexLock::~MediaMutexLock()
{
  pthread_mutex_destroy((pthread_mutex_t*)iHandle);
  free(iHandle);
  iHandle = NULL;
}

MediaMutexLock::MediaMutexLock(const MediaMutexLock& other) { m_lastLockId = -1; }

MediaMutexLock& MediaMutexLock::operator=(const MediaMutexLock& other) { return *this; }

void MediaMutexLock::Lock(int lastlockid)
{
  pthread_mutex_lock((pthread_mutex_t*)iHandle);
  m_lastLockId = (m_lastLockId << 8) + lastlockid;
  ++iLockCnt;
}

void MediaMutexLock::Unlock()
{
  --iLockCnt;
  m_lastLockId = -1;
  pthread_mutex_unlock((pthread_mutex_t*)iHandle);
}

int MediaMutexLock::GetLockCnt() const { return iLockCnt; }

MediaMutex::MediaMutex()
{
  pthread_mutexattr_t mutexAttr;
  pthread_mutexattr_init(&mutexAttr);
  pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE); // 同线程可多次进入
  pthread_mutex_init(&m_mutex, &mutexAttr);
  pthread_mutexattr_destroy(&mutexAttr);
}

MediaMutex::~MediaMutex()
{
  //    PlatAssert(iLockCnt == 0, "no released");
  pthread_mutex_destroy(&m_mutex);
}

MediaMutex::MediaMutex(const MediaMutex& other)
{
  //  PlatAssert(false, "no implementation");
}

MediaMutex& MediaMutex::operator=(const MediaMutex& other)
{
  //   PlatAssert(false, "no implementation");
  return *this;
}

void MediaMutex::Lock() { pthread_mutex_lock(&m_mutex); }

void MediaMutex::Unlock() { pthread_mutex_unlock(&m_mutex); }

/////////// MediaEvent
MediaEvent::MediaEvent() : iHandle(malloc(sizeof(pthread_mutex_t) + sizeof(pthread_cond_t)))
{
  pthread_mutex_t* mutex = (pthread_mutex_t*)iHandle;
  pthread_cond_t* cond = (pthread_cond_t*)((char*)iHandle + sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);
  pthread_cond_init(cond, NULL);
}

MediaEvent::~MediaEvent()
{
  pthread_mutex_t* mutex = (pthread_mutex_t*)iHandle;
  pthread_cond_t* cond = (pthread_cond_t*)((char*)iHandle + sizeof(pthread_mutex_t));

  pthread_cond_destroy(cond);
  pthread_mutex_destroy(mutex);

  free(iHandle);
  iHandle = NULL;
}

MediaEvent::MediaEvent(const MediaEvent& other)
{
  //   PlatAssert(false, "no implementation");
}

MediaEvent& MediaEvent::operator=(const MediaEvent& other)
{
  //    PlatAssert(false, "no implementation");
  return *this;
}

bool MediaEvent::Wait(unsigned int timeoutMS)
{
  pthread_mutex_t* mutex = (pthread_mutex_t*)iHandle;
  pthread_cond_t* cond = (pthread_cond_t*)((char*)iHandle + sizeof(pthread_mutex_t));

  pthread_mutex_lock(mutex);
  struct timespec ts;
  timeval value;
  struct timezone time_zone;
  time_zone.tz_minuteswest = 0;
  time_zone.tz_dsttime = 0;
  gettimeofday(&value, &time_zone);

#if defined(__APPLE__)
  TIMEVAL_TO_TIMESPEC(&value, &ts);
#else
  ts.tv_sec = value.tv_sec;
  ts.tv_nsec = value.tv_usec * 1000;
#endif

  ts.tv_sec += timeoutMS / 1000;
  ts.tv_nsec += (timeoutMS - (timeoutMS / 1000) * 1000) * 1000000;
  pthread_cond_timedwait(cond, mutex, &ts);
  pthread_mutex_unlock(mutex);
  return true;
}

bool MediaEvent::Wait()
{
  pthread_mutex_t* mutex = (pthread_mutex_t*)iHandle;
  pthread_cond_t* cond = (pthread_cond_t*)((char*)iHandle + sizeof(pthread_mutex_t));

  pthread_mutex_lock(mutex);
  pthread_cond_wait(cond, mutex);
  pthread_mutex_unlock(mutex);

  return true;
}

void MediaEvent::Signal()
{
  pthread_cond_t* cond = (pthread_cond_t*)((char*)iHandle + sizeof(pthread_mutex_t));
  pthread_cond_broadcast(cond);
}

/////////////////// MutexStackLock
MutexStackLock::MutexStackLock(MediaMutex& mutex) : iMutex(mutex) { iMutex.Lock(); }

MutexStackLock::~MutexStackLock() { iMutex.Unlock(); }

MutexStackLock& MutexStackLock::operator=(const MutexStackLock& other)
{
  //  LibAssert(false, "no impl");
  return *this;
}

MutexStackLock::MutexStackLock(const MutexStackLock& other) : iMutex(other.iMutex)
{
  // LibAssert(false, "no impl");
}

RWLock::RWLock() : m_lock() { pthread_rwlock_init(&m_lock, 0); }

RWLock::~RWLock() { pthread_rwlock_destroy(&m_lock); }

void RWLock::acquireLockExclusive() { pthread_rwlock_wrlock(&m_lock); }

void RWLock::releaseLockExclusive() { pthread_rwlock_unlock(&m_lock); }

void RWLock::rcquireLockShared() { pthread_rwlock_rdlock(&m_lock); }

void RWLock::releaseLockShared() { pthread_rwlock_unlock(&m_lock); }

#endif

ReferenceCalc::ReferenceCalc(MediaMutex& mutex, uint32_t& count)
: m_mutex(mutex)
, m_count(count)
{
  m_mutex.Lock();
  ++m_count;
  m_mutex.Unlock();
}

ReferenceCalc::~ReferenceCalc()
{
  m_mutex.Lock();
  --m_count;
  m_mutex.Unlock();
}
