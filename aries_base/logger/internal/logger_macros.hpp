/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/30 17:22
  filename:  aries_base/logger/internal/logger_macros.hpp

  purpose:   Log to both class logger and aggregate logger
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_LOGGER_INTERNAL_LOGGER_MACROS_HPP
#define ARIES_BASE_LOGGER_INTERNAL_LOGGER_MACROS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#define AGGREGATE_LOGGER_NAME "aggregate"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

#define LOG_TRACE(logger, ...) \
  do { \
    if (logger) logger->trace(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->trace(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

#define LOG_DEBUG(logger, ...) \
  do { \
    if (logger) logger->debug(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->debug(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

#define LOG_INFO(logger, ...) \
  do { \
    if (logger) logger->info(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->info(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

#define LOG_WARN(logger, ...) \
  do { \
    if (logger) logger->warn(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->warn(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

#define LOG_ERROR(logger, ...) \
  do { \
    if (logger) logger->error(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->error(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

#define LOG_CRITICAL(logger, ...) \
  do { \
    if (logger) logger->critical(__VA_ARGS__); \
    if (auto agg = spdlog::get(AGGREGATE_LOGGER_NAME)) agg->critical(__VA_ARGS__); \
  } while(0)
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_LOGGER_INTERNAL_LOGGER_MACROS_HPP
// -----------------------------------------------------------------------------
