/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/22 06:09
  filename:  aries_base/utils/times.cpp

  purpose:   Source file for time utilities
*********************************************************************/


// -----------------------------------------------------------------------------
#include <string>
#include <chrono>
#include <ctime>

#include "aries_base/utils/bytes.hpp"
#include "aries_base/utils/times.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace utils {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define SECONDS(x) ((x) * (1000UL))
#define MINUTES(x) ((x) * (SECONDS(60UL)))
#define HOURS(x) ((x) * MINUTES(60UL))
#define DAYS(x) ((x) * HOURS(24UL))
#define WEEKS(x) ((x) * DAYS(7UL))
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

uint32_t EpocTime() {
  system_clock::time_point now = system_clock::now();
  system_clock::duration elapsed = now.time_since_epoch();
  uint32_t seconds_since_epoch = static_cast<time_t>(elapsed / seconds(1));
  return seconds_since_epoch;
}
// -----------------------------------------------------------------------------

uint64_t EpocTimeMs() {
  system_clock::time_point now = system_clock::now();
  system_clock::duration elapsed = now.time_since_epoch();
  uint64_t milliseconds_since_epoch = elapsed / milliseconds(1);
  return milliseconds_since_epoch;
}
// -----------------------------------------------------------------------------

uint64_t EpocTimeUs() {
  system_clock::time_point now = system_clock::now();
  system_clock::duration elapsed = now.time_since_epoch();
  uint64_t microseconds_since_epoch = elapsed / microseconds(1);
  return microseconds_since_epoch;
}
// -----------------------------------------------------------------------------

int8_t TimeZone() {
  // 1992/19/04 00:00:00 UTC
  time_t utc_time = 703641600UL;

  struct tm local_timeinfo = {0};
#ifdef _WIN32
    localtime_s(&local_timeinfo, &utc_time);
#else
    localtime_r(&utc_time, &local_timeinfo);
#endif
  int32_t local_hours = local_timeinfo.tm_mday * 24L + local_timeinfo.tm_hour;

  struct tm gm_timeinfo = {0};
#ifdef _WIN32
    gmtime_s(&gm_timeinfo, &utc_time);
#else
    gmtime_r(&utc_time, &gm_timeinfo);
#endif
  int32_t gm_hours = gm_timeinfo.tm_mday * 24L + gm_timeinfo.tm_hour;

  return static_cast<int8_t>(local_hours - gm_hours);
}
// -----------------------------------------------------------------------------

time_t AdjustTime(time_t time, int32_t duration, TimeUnit unit) {
  time_t utime = time;

  if (utime < 0) {
    return time;
  }

  switch (unit) {
    case TimeUnit::kMiliseconds:
    case TimeUnit::kSeconds:
    case TimeUnit::kMinutes:
    case TimeUnit::kHours:
    case TimeUnit::kDays:
    case TimeUnit::kWeeks: {
      return time + static_cast<int32_t>(FromMiliseconds(
                        ToMiliseconds(duration, unit), TimeUnit::kSeconds));
    }

    case TimeUnit::kMonths: {
      struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif
      int months = static_cast<int32_t>(
          FromMiliseconds(ToMiliseconds(duration, unit), TimeUnit::kMonths));
      months += timeinfo.tm_mon;
      timeinfo.tm_mon = months % 12;
      timeinfo.tm_year += months / 12;
      return mktime(&timeinfo);
    }

    case TimeUnit::kYears: {
      struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif
      int years = static_cast<int32_t>(
          FromMiliseconds(ToMiliseconds(duration, unit), TimeUnit::kYears));
      timeinfo.tm_year += years;
      return mktime(&timeinfo);
    }
  }

  // unexpected error
  return time;
}
// -----------------------------------------------------------------------------

time_t YearBegin(uint32_t time) {
  time_t utime = time;

  if (utime < 0) {
    return time;
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  timeinfo.tm_sec = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_hour = 0;
  timeinfo.tm_mday = 1;
  timeinfo.tm_mon = 0;

  return mktime(&timeinfo);
}
// -----------------------------------------------------------------------------

time_t MonthBegin(uint32_t time) {
  time_t utime = time;

  if (utime < 0) {
    return time;
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  timeinfo.tm_sec = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_hour = 0;
  timeinfo.tm_mday = 1;

  return mktime(&timeinfo);
}
// -----------------------------------------------------------------------------

time_t DayBegin(uint32_t time) {
  time_t utime = time;

  if (utime < 0) {
    return time;
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  timeinfo.tm_sec = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_hour = 0;

  return mktime(&timeinfo);
}
// -----------------------------------------------------------------------------

time_t WeekBegin(uint32_t time) {
  time_t utime = time;

  if (utime < 0) {
    return time;
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  timeinfo.tm_sec = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_hour = 0;

  return mktime(&timeinfo) -
         static_cast<int32_t>(
             FromMiliseconds(ToMiliseconds(timeinfo.tm_wday, TimeUnit::kDays),
                             TimeUnit::kSeconds));
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace time {
// -----------------------------------------------------------------------------

std::string ToString(time_t datetime, const std::string format) {
  time_t utime = datetime;

  if (utime < 0) {
    return std::string("invalid time");
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  utils::bytes buffer(64);
  while (!strftime(reinterpret_cast<char*>(&buffer[0]),
                   buffer.size(),
                   format.c_str(),
                   &timeinfo)) {
  }

  return reinterpret_cast<char*>(&buffer[0]);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace time
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace gm_time {
// -----------------------------------------------------------------------------

std::string ToString(time_t datetime, const std::string format) {
  time_t utime = datetime;

  if (utime < 0) {
    return std::string("invalid time");
  }

  struct tm timeinfo = {0};
#ifdef _WIN32
      localtime_s(&timeinfo, &utime);
#else
      localtime_r(&utime, &timeinfo);
#endif

  utils::bytes buffer(64);
  while (!strftime(reinterpret_cast<char*>(&buffer[0]),
                   buffer.size(),
                   format.c_str(),
                   &timeinfo)) {
  }

  return reinterpret_cast<char*>(&buffer[0]);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace gm_time
// -----------------------------------------------------------------------------

int64_t ToMiliseconds(int64_t duration, TimeUnit unit) {
  switch (unit) {
    case TimeUnit::kMiliseconds:
      break;
    case TimeUnit::kSeconds:
      duration *= SECONDS(1);
      break;
    case TimeUnit::kMinutes:
      duration *= MINUTES(1);
      break;
    case TimeUnit::kHours:
      duration *= HOURS(1);
      break;
    case TimeUnit::kDays:
      duration *= DAYS(1);
      break;
    case TimeUnit::kWeeks:
      duration *= WEEKS(1);
      break;
  }

  return duration;
}
// -----------------------------------------------------------------------------

int64_t FromMiliseconds(int64_t duration, TimeUnit unit) {
  switch (unit) {
    case TimeUnit::kMiliseconds:
      break;
    case TimeUnit::kSeconds:
      duration /= SECONDS(1);
      break;
    case TimeUnit::kMinutes:
      duration /= MINUTES(1);
      break;
    case TimeUnit::kHours:
      duration /= HOURS(1);
      break;
    case TimeUnit::kDays:
      duration /= DAYS(1);
      break;
    case TimeUnit::kWeeks:
      duration /= WEEKS(1);
      break;
  }

  return duration;
}
// -----------------------------------------------------------------------------

uint64_t CreateTimer(uint64_t duration, TimeUnit unit) {
  return EpocTimeMs() + ToMiliseconds(duration, unit);
}
// -----------------------------------------------------------------------------

uint64_t TimeLeft(uint64_t timer, TimeUnit unit) {
  if (IsTimeout(timer)) {
    return 0;
  }

  return FromMiliseconds(timer - EpocTimeMs(), unit);
}
// -----------------------------------------------------------------------------

uint64_t TimePassed(uint64_t timer, TimeUnit unit) {
  if (!IsTimeout(timer)) {
    return 0;
  }

  return FromMiliseconds(EpocTimeMs() - timer, unit);
}
// -----------------------------------------------------------------------------

bool IsTimeout(uint64_t timer) {
  return timer < EpocTimeMs();
}
// -----------------------------------------------------------------------------

bool IsTimeout(uint64_t based_time, uint64_t passed_time, TimeUnit unit) {
  return IsTimeout(based_time + ToMiliseconds(passed_time, unit));
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace utils
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
