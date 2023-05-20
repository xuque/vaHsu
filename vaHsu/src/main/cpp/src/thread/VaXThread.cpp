#include "thread/VaXThread.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include "include/IVaUtils.h"
#include "log/VaLogger.h"

#ifdef _WIN32
#include <process.h>
// #include <timeapi.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

VaThread::VaThread(const char* name, uint32_t interval, bool urgent)
  : m_interval(interval)
  , m_uncondLoop(false)
  , m_lastTimeoutStamp(0)
  , m_lastWakeupStamp(0)
  , m_wakeupTimes(0)
  , m_callWakeUpCount(0)
  , m_callResetWakeUpCount(0)
  , m_maxSleepInterval(0)
  , m_bQuit(true)
  , m_urgent(urgent)
  , m_hThread(0)
#ifdef _WIN32
  , m_event(NULL)
  , m_exitMaxWaitTime(INFINITE)
#endif
{
  ::strncpy(m_name, name, kThreadNameSize - 1);
  ::memset(m_runUseArray, 0, sizeof(m_runUseArray));

#ifdef _WIN32
  // 注: m_interval=0在移动端是一直阻塞，对应WIN的是INFINITE
  if (m_interval == 0)
  {
    m_interval = INFINITE;
  }
#endif

  LOGI(kLogTagThread, "thread(%s) %u %u construct", m_name, interval, urgent);
}

VaThread::~VaThread()
{
  stopThread();
}

#ifdef _WIN32
unsigned __stdcall VaThread::threadFunc(void* pParam)
{
  VaThread* pThread = static_cast<VaThread*>(pParam);
  if (pThread)
  {
    pThread->loop();
  }
  return 0;
}

void _stdcall VaThread::timerRoutine(PVOID lpParameter, BOOLEAN timerOrWaitFired)
{
  VaThread* pThread = (VaThread*)lpParameter;
  if (NULL != pThread)
  {
    pThread->onTimerRoutine();
  }
}
#else
void* VaThread::threadFunc(void* pParam)
{
  VaThread* pThread = static_cast<VaThread*>(pParam);
  if (pThread)
  {
    char threadName[kThreadNameSize] = { 0 };
    pThread->getThreadName(threadName);
#if defined(__ANDROID__)
    pthread_setname_np(pthread_self(), threadName);
#elif defined(__APPLE__)
    pthread_setname_np(threadName);
#endif

    pThread->loop();
  }
  return pParam;
}
#endif

void VaThread::loop()
{
  if (m_urgent)
  {
    // #ifdef ANDROID
    //    SetAndroidThreadPriority(m_name, THREAD_PRIORITY_URGENT_AUDIO);
    // #endif
  }

  showThreadPriority();

  onCreate();
  if (m_uncondLoop)
  {
    onUnconditionalLoop();
  }
  else if (m_urgent)
  {
    onUrgentLoop();
  }
  else
  {
    onTimerLoop();
  }
  onStop();
  resetWakeUpEvent();
}

void VaThread::onUnconditionalLoop()
{
  while (!m_bQuit)
  {
    uint32_t now = IVaUtils::getTickCount();
    onThreadRun(now);
  }
}

void VaThread::onUrgentLoop()
{
#ifdef _WIN32
  onTimerLoop();
#else
  for (;;)
  {
    if (m_interval == 0)
    {
      m_semaphore.wait();
    }
    else
    {
      m_semaphore.wait_for(m_interval);
    }

    if (m_bQuit)
    {
      break;
    }

    uint32_t now = IVaUtils::getTickCount();
    checkPerformance(now);
    onThreadRun(now);

    m_lastTimeoutStamp = IVaUtils::getTickCount();
    monitorWakeupTimes(m_lastTimeoutStamp, m_lastTimeoutStamp - now);

    if (m_bQuit)
    {
      break;
    }
  }
#endif
}

void VaThread::onTimerLoop()
{
#ifdef _WIN32
  HANDLE hTimer = NULL;
  MMRESULT periodRet = timeBeginPeriod(1);
  uint32_t minInterval = (uint32_t)m_interval;
  uint32_t maxInterval = (uint32_t)(m_interval << 1);
  if (!CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)timerRoutine, this, minInterval, minInterval, 0))
  {
    LOGW(kLogTagThread, "%s CreateTimerQueueTimer failed %u", m_name, GetLastError());
  }
#endif

  for (; ; )
  {
#ifdef _WIN32
    if (minInterval != m_interval)
    {
      if (hTimer != NULL)
      {
        DeleteTimerQueueTimer(NULL, hTimer, NULL);
      }
      minInterval = (uint32_t)m_interval;
      maxInterval = (uint32_t)(m_interval << 1);
      if (!CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)timerRoutine, this, minInterval, minInterval, 0))
      {
        LOGW(kLogTagThread, "%s CreateTimerQueueTimer failed %u", m_name, GetLastError());
      }
    }
    WaitForSingleObject(m_event, maxInterval);
#else
    if (m_interval == 0)
    {
      m_semaphore.wait();
    }
    else
    {
      m_semaphore.wait_for(m_interval);
    }
#endif
    if (m_bQuit)
    {
      break;
    }

    uint32_t now = IVaUtils::getTickCount();
    if (m_interval != 0)
    {
      checkPerformance(now);
    }

    onThreadRun(now);

    m_lastTimeoutStamp = IVaUtils::getTickCount();
    monitorWakeupTimes(m_lastTimeoutStamp, m_lastTimeoutStamp - now);

    if (m_bQuit)
    {
      break;
    }
  }

#ifdef _WIN32
  if (NULL != hTimer)
  {
    DeleteTimerQueueTimer(NULL, hTimer, NULL);
    hTimer = NULL;
  }
  if (TIMERR_NOERROR == periodRet)
  {
    timeEndPeriod(1);
  }
#endif
}

void VaThread::stopThread()
{
  if (m_bQuit)
  {
    // LOGT(kLogTagThread, "faild to stop thread, thread(%s) has been stopped", m_name);
    return;
  }

  m_bQuit = true;

#ifdef _WIN32
  if (m_event == 0)
  {
    LOGT(kLogTagThread, "!!!bug %s thread event is null when stop", m_name);
    return;
  }

  if (m_hThread == 0)
  {
    LOGT(kLogTagThread, "!!!bug %s thread handle is null when stop", m_name);
    CloseHandle(m_event);
    m_event = 0;
    return;
  }

  LOGT(kLogTagThread, "stop %s thread", m_name);
  SetEvent(m_event);

  DWORD ret = WaitForSingleObject(m_hThread, m_exitMaxWaitTime);
  if (WAIT_TIMEOUT == ret)
  {
    TerminateThread(m_hThread, -1);
  }

  CloseHandle(m_hThread);
  m_hThread = 0;
  CloseHandle(m_event);
  m_event = 0;
  m_lastTimeoutStamp = 0;
#else
  if (m_hThread == 0)
  {
    LOGT(kLogTagThread, "!!!bug %s thread handle is null when stop", m_name);
    return;
  }

  LOGT(kLogTagThread, "stop %s thread", m_name);
  wakeUp();

  pthread_join(m_hThread, NULL);
  m_hThread = 0;
#endif
  LOGI(kLogTagThread, "thread(%s) stop successfully", m_name);
}

void VaThread::startThread()
{
  if (!m_bQuit)
  {
    LOGW(kLogTagThread, "failed to start thread, thread(%s) has been started", m_name);
    return;
  }

  m_bQuit = false;
#ifdef _WIN32
  m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (m_event == 0)
  {
    m_bQuit = true;
    LOGI(kLogTagThread, "!!!bug %s thread event is null when start", m_name);
    return;
  }

  unsigned threadID;
  m_hThread = (HANDLE)_beginthreadex(NULL, 0, VaThread::threadFunc, this, 0, &threadID);
  if (m_hThread == 0)
  {
    LOGI(kLogTagThread, "!!!bug %s thread handle is null when start, error %u", m_name, GetLastError());
    CloseHandle(m_event);
    m_event = 0;
    m_bQuit = true;
    return;
  }

  SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
#else
  int err = 0;
  if (m_urgent)
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (pthread_attr_init(&attr) != 0)
    {
      LOGW(kLogTagThread, "failed to call pthread_attr_init in thread(%s)", m_name);
    }

    // Set real-time round-robin policy.
    if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0)
    {
      LOGW(kLogTagThread, "failed to call pthread_attr_setschedpolicy in thread(%s)", m_name);
    }

    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = 6; // 6 = HIGH
    if (pthread_attr_setschedparam(&attr, &param) != 0)
    {
      LOGW(kLogTagThread, "failed to call pthread_attr_setschedparam in thread(%s)", m_name);
    }

    err = pthread_create(&m_hThread, &attr, &threadFunc, this);
  }
  else
  {
    err = pthread_create(&m_hThread, NULL, &threadFunc, this);
  }

  if (err != 0)
  {
    m_hThread = 0;
    m_bQuit = true;
    LOGE(kLogTagThread, "failed to createEngine thread(%s) %u", m_name, err);
    return;
  }
#endif
  LOGI(kLogTagThread, "start thread(%s) %s interval %u", m_name, m_urgent ? "urgent" : "unurgent", m_interval);
}

void VaThread::wakeUp()
{
#ifdef _WIN32
  if (m_event != 0)
  {
    SetEvent(m_event);
  }
#else
  m_semaphore.signal();
#endif
  ++m_callWakeUpCount;
  // LOGI(kLogTagThread, "thread(%s) wakeup now %u", m_name, YYUtils::getTickCount());
}

void VaThread::resetWakeUpEvent()
{
#ifndef _WIN32
  m_semaphore.reset();
#endif
  ++m_callResetWakeUpCount;
}

bool VaThread::isQuit() const { return m_bQuit; }

void VaThread::onCreate() { LOGI(kLogTagThread, "thread(%s) created %u", m_name, m_interval); }

void VaThread::onStop() { LOGI(kLogTagThread, "exit thread(%s) %u", m_name, m_interval); }

void VaThread::checkPerformance(uint32_t now)
{
  if (m_lastTimeoutStamp == 0)
  {
    return;
  }

  if (IVaUtils::isBiggerUint32(m_lastTimeoutStamp, now))
  {
    LOGI(kLogTagPerf,
         "thread(%s) system time has been modified, last %u cur %u diff %u",
         m_name,
         m_lastTimeoutStamp,
         now,
         m_lastTimeoutStamp - now);
    return;
  }

  const uint32_t kMaxInterval = 100;
  uint32_t deltaT = now - m_lastTimeoutStamp;
  m_maxSleepInterval = std::max(deltaT, m_maxSleepInterval);
  if (deltaT > kMaxInterval + m_interval)
  {
    LOGW(kLogTagPerf, "thread(%s) process spend too long %u %u", m_name, m_interval, deltaT);
  }
}

void VaThread::monitorWakeupTimes(uint32_t now, uint32_t runUse)
{
  ++m_wakeupTimes;
  for (uint32_t i = 0; i < kMaxRunUseArraySize; ++i)
  {
    if (runUse <= kRunUseThreshold[i])
    {
      ++m_runUseArray[i];
      break;
    }
  }

  if (m_lastWakeupStamp == 0)
  {
    m_lastWakeupStamp = now;
    return;
  }

  const uint32_t kTimeout = 32 * 1000;
  if (IVaUtils::isBiggerUint32(m_lastWakeupStamp + kTimeout, now))
  {
    return;
  }

  std::ostringstream oss;
  for (uint32_t i = 0; i < kMaxRunUseArraySize; ++i)
  {
    oss << " " << (int)kRunUseThreshold[i] << ":" << m_runUseArray[i];
  }

  LOGI(kLogTagThread,
       "thread(%s) in past %u ms, wakeup %u times callWakeupCount %u "
       "callResetWakeUpCount %u maxSleep %u runUse(%s)",
       m_name,
       now - m_lastWakeupStamp,
       m_wakeupTimes,
       m_callWakeUpCount,
       m_callResetWakeUpCount,
       m_maxSleepInterval,
       oss.str().c_str());

  m_lastWakeupStamp = now;
  m_wakeupTimes = 0;
  m_callWakeUpCount = 0;
  m_callResetWakeUpCount = 0;
  m_maxSleepInterval = 0;
  memset(m_runUseArray, 0, sizeof(m_runUseArray));
}

void VaThread::showThreadPriority()
{
#ifndef _WIN32
  pthread_attr_t attr;
  int ret = pthread_attr_init(&attr);
  if (ret != 0)
  {
    LOGW(kLogTagThread, "failed to call pthread_attr_init in thread(%s)", m_name);
    return;
  }

  int policy = 0;
  ret = pthread_attr_getschedpolicy(&attr, &policy);
  if (ret != 0)
  {
    LOGW(kLogTagThread, "failed to call pthread_attr_getschedpolicy in thread(%s)", m_name);
    return;
  }

  struct sched_param param;
  memset(&param, 0, sizeof(param));
  ret = pthread_attr_getschedparam(&attr, &param);
  if (ret != 0)
  {
    LOGW(kLogTagThread, "failed to call pthread_attr_getschedparam in thread(%s)", m_name);
    return;
  }

  int maxPriority = sched_get_priority_max(policy);
  int minPriority = sched_get_priority_min(policy);

  LOGI(kLogTagThread,
       "thread priority in thread(%s), policy %d minPriority %d maxPriority %d "
       "curPriority %u",
       m_name,
       policy,
       minPriority,
       maxPriority,
       param.sched_priority);
#endif
}

void VaThread::resetInterval(uint32_t interval) { m_interval = interval; }

const char* VaThread::getThreadName(char* threadName) const
{
  if (nullptr == threadName)
  {
    return nullptr;
  }
  
  ::strncpy(threadName, m_name, kThreadNameSize - 1);
  return threadName;
}

void VaThread::onTimerRoutine()
{
#ifdef _WIN32
  SetEvent(m_event);
#endif
}

XTaskThread::XTaskThread(const char* name, uint32_t interval, ITask* task) 
 : VaThread(name, interval)
 , m_task(task)
{
  m_uncondLoop = (interval == 0);
}

void XTaskThread::onThreadRun(uint32_t now)
{
  if (!m_task)
  {
    LOGE(kLogTagThread, "error!!!!!!!!");
    return;
  }

  m_task->proc();
}
