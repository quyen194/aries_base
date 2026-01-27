/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/22 06:03
  filename:  aries_base/utils/times.hpp

  purpose:   Header file for time utilities
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_UTILS_TIMES_HPP
#define ARIES_BASE_UTILS_TIMES_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
enum class TimeUnit : uint8_t {
  kMiliseconds,
  kSeconds,
  kMinutes,
  kHours,
  kDays,

  kWeeks,
  kMonths,
  kYears,
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace utils {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
uint32_t EpocTime();
uint64_t EpocTimeMs();
uint64_t EpocTimeUs();
int8_t TimeZone();

time_t AdjustTime(time_t time,
                  int32_t duration,
                  TimeUnit unit = TimeUnit::kSeconds);
time_t YearBegin(time_t time);
time_t MonthBegin(time_t time);
time_t DayBegin(time_t time);
time_t WeekBegin(time_t time);

namespace time {
std::string ToString(time_t datetime, const std::string format = "%F %T");
} // namespace time

namespace gm_time {
std::string ToString(time_t datetime, const std::string format = "%F %T");
} // namespace gm_time

int64_t ToMiliseconds(int64_t duration, TimeUnit unit);
int64_t FromMiliseconds(int64_t duration, TimeUnit unit);
uint64_t CreateTimer(uint64_t duration, TimeUnit unit = TimeUnit::kMiliseconds);
uint64_t TimeLeft(uint64_t timer, TimeUnit unit = TimeUnit::kMiliseconds);
uint64_t TimePassed(uint64_t timer, TimeUnit unit = TimeUnit::kMiliseconds);
bool IsTimeout(uint64_t timer);
bool IsTimeout(uint64_t based_time,
               uint64_t passed_time,
               TimeUnit unit = TimeUnit::kMiliseconds);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace utils
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_UTILS_TIMES_HPP
// -----------------------------------------------------------------------------
