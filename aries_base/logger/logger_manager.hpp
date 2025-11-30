/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/30 16:16
  filename:  aries_base/logger/logger_manager.hpp

  purpose:   Logger manager to create and manage loggers
             That log to console and rotating files.
             This class uses spdlog library for logging.
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_LOGGER_LOGGER_MANAGER_HPP
#define ARIES_BASE_LOGGER_LOGGER_MANAGER_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "aries_base/definitions/macro.hpp"

#include "aries_base/logger/internal/logger_macros.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace common {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define DEFAULT_LOGS_DIRECTORY "logs"
#define DEFAULT_MAX_FILE_SIZE (1024 * 1024 * 50)  // 50 MB
#define DEFAULT_MAX_FILE_COUNT 10
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class LoggerManager {
 private:
  LoggerManager(std::string logs_directory);
  virtual ~LoggerManager();

 public:
  static void CreateInstance(std::string logs_directory = DEFAULT_LOGS_DIRECTORY);
  static LoggerManager* Instance();

  // Create logger for individual class with its own file
  std::shared_ptr<spdlog::logger> CreateClassLogger(
      const std::string& class_name,
      const std::string& log_file_name = "",
      spdlog::level::level_enum console_level = spdlog::level::info,
      spdlog::level::level_enum file_level = spdlog::level::debug,
      uint32_t max_file_size = DEFAULT_MAX_FILE_SIZE,
      uint32_t max_file_count = DEFAULT_MAX_FILE_COUNT);

  // Create logger with shared file (multiple classes → same file)
  std::shared_ptr<spdlog::logger> CreateLoggerWithSharedFile(
      const std::string& logger_name,
      const std::string& shared_file_name,
      spdlog::level::level_enum console_level = spdlog::level::info,
      spdlog::level::level_enum file_level = spdlog::level::debug,
      uint32_t max_file_size = DEFAULT_MAX_FILE_SIZE,
      uint32_t max_file_count = DEFAULT_MAX_FILE_COUNT);

  // Create aggregate logger for all modules
  void CreateAggregateLogger(
      const std::string& file_name,
      spdlog::level::level_enum file_level = spdlog::level::trace,
      uint32_t max_file_size = DEFAULT_MAX_FILE_SIZE,
      uint32_t max_file_count = DEFAULT_MAX_FILE_COUNT);

 private:
  std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> GetOrCreateSharedFileSink(
      const std::string& file_name,
      spdlog::level::level_enum level,
      uint32_t max_file_size = DEFAULT_MAX_FILE_SIZE,
      uint32_t max_file_count = DEFAULT_MAX_FILE_COUNT);

 private:
  std::string logs_directory_;

  std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>> shared_sinks_;
  std::mutex sinks_mutex_;

 private:
  static LoggerManager *instance_;

 private:
  DISALLOW_COPY_AND_ASSIGN(LoggerManager);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace common
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_LOGGER_LOGGER_MANAGER_HPP
// -----------------------------------------------------------------------------
