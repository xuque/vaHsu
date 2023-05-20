#pragma once
#ifdef __linux__
#include <sys/time.h>
#endif
#ifdef __APPLE__
#include <sys/time.h>
#endif
#ifdef __ANDROID__
#include <ctime>
#endif
#ifdef _WIN32
#include <time.h>
#endif
#ifdef _WIN64
#include <time.h>
#endif