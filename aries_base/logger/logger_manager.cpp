/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/30 16:17
  filename:  aries_base/logger/logger_manager.cpp

  purpose:   Logger manager implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <filesystem>

#include "aries_base/logger/logger_manager.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace common {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
LoggerManager* LoggerManager::instance_ = nullptr;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

LoggerManager::LoggerManager(std::string logs_directory)
    : logs_directory_(logs_directory) {
  // Create logs directory
  std::filesystem::create_directories(logs_directory_);
}
// -----------------------------------------------------------------------------

LoggerManager::~LoggerManager() {
  spdlog::shutdown();
}
// -----------------------------------------------------------------------------

void LoggerManager::CreateInstance(std::string logs_directory) {
  if (instance_ == nullptr) {
    instance_ = new LoggerManager(logs_directory);
  }
}
// -----------------------------------------------------------------------------

void LoggerManager::DestroyInstance() {
  if (instance_) {
    delete instance_;
    instance_ = nullptr;
  }
}
// -----------------------------------------------------------------------------

LoggerManager* LoggerManager::Instance() {
  if (instance_ == nullptr) {
    CreateInstance(DEFAULT_LOGS_DIRECTORY);
  }
  return instance_;
}
// -----------------------------------------------------------------------------

std::shared_ptr<spdlog::logger> LoggerManager::CreateClassLogger(
    const std::string& class_name,
    const std::string& log_file_name,
    spdlog::level::level_enum console_level,
    spdlog::level::level_enum file_level,
    uint32_t max_file_size,
    uint32_t max_file_count) {

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(console_level);
  console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

  std::string file_name = log_file_name.empty() ? class_name : log_file_name;
  std::string log_file = fmt::format("{}/{}", logs_directory_, file_name);
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      log_file, max_file_size, max_file_count);
  file_sink->set_level(file_level);
  file_sink->set_pattern(
    R"({"ts":"%Y-%m-%dT%H:%M:%S.%eZ","class":"%n","lvl":"%l","tid":%t,"src":"%s:%#","func":"%!","msg":"%v"})");

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  auto logger = std::make_shared<spdlog::logger>(class_name, sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::trace);

  spdlog::register_logger(logger);
  return logger;
}
// -----------------------------------------------------------------------------

std::shared_ptr<spdlog::logger> LoggerManager::CreateLoggerWithSharedFile(
    const std::string& logger_name,
    const std::string& shared_file_name,
    spdlog::level::level_enum console_level,
    spdlog::level::level_enum file_level,
    uint32_t max_file_size,
    uint32_t max_file_count) {

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(console_level);
  console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

  auto file_sink = GetOrCreateSharedFileSink(shared_file_name, file_level);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  auto logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::trace);

  spdlog::register_logger(logger);
  return logger;
}
// -----------------------------------------------------------------------------

void LoggerManager::CreateAggregateLogger(
    const std::string& file_name,
    spdlog::level::level_enum file_level,
    uint32_t max_file_size,
    uint32_t max_file_count) {

  std::string log_file = fmt::format("{}/{}", logs_directory_, file_name);
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      log_file, max_file_size, max_file_count);
  file_sink->set_level(file_level);
  file_sink->set_pattern(
      R"({"ts":"%Y-%m-%dT%H:%M:%S.%eZ","logger":"%n","lvl":"%l","tid":%t,"src":"%s:%#","func":"%!","msg":"%v"})");

  auto aggregate_logger = std::make_shared<spdlog::logger>(AGGREGATE_LOGGER_NAME, file_sink);
  aggregate_logger->set_level(spdlog::level::trace);
  spdlog::register_logger(aggregate_logger);
}
// -----------------------------------------------------------------------------

std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> LoggerManager::GetOrCreateSharedFileSink(
    const std::string& file_name,
    spdlog::level::level_enum level,
    uint32_t max_file_size,
    uint32_t max_file_count) {

  std::lock_guard<std::mutex> lock(sinks_mutex_);

  auto it = shared_sinks_.find(file_name);
  if (it != shared_sinks_.end()) {
    return it->second;
  }

  std::string log_file = fmt::format("{}/{}", logs_directory_, file_name);
  auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
    log_file, max_file_size, max_file_count);
  sink->set_level(level);
  sink->set_pattern(
    R"({"ts":"%Y-%m-%dT%H:%M:%S.%eZ","logger":"%n","lvl":"%l","tid":%t,"src":"%s:%#","func":"%!","msg":"%v"})");

  shared_sinks_[file_name] = sink;
  return sink;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace common
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------
