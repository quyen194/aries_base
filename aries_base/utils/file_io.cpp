/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/06 10:29
  filename:  aries_base/utils/file_io.cpp

  purpose:   File I/O utilities implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "aries_base/utils/file_io.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace utils {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

std::string StringFromFile(const std::string& file_path) {
  std::ifstream file(file_path, std::ios::in);
  if (!file.is_open()) {
    return std::string();
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}
// -----------------------------------------------------------------------------

bool StringToFile(const std::string& str, const std::string& file_path) {
  std::ofstream file(file_path, std::ios::out);
  if (!file.is_open()) {
    return false;
  }

  file << str;
  file.close();

  return false;
}
// -----------------------------------------------------------------------------

bytes DataFromFile(const std::string &file_path) {
  std::ifstream file(file_path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    return bytes();
  }

  bytes buffer;
  file.seekg(0, std::ios::end);

  if (file.tellg() == 0) {
    return bytes();
  }

  buffer.resize(file.tellg());

  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
  file.close();

  return ToBytes(buffer.data(), buffer.size());
}
// -----------------------------------------------------------------------------

bool DataToFile(const bytes& data, const std::string& file_path) {
  return DataToFile(data.data(), data.size(), file_path);
}
// -----------------------------------------------------------------------------

bool DataToFile(const void* data, size_t len, const std::string& file_path) {
  std::ofstream file(file_path, std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  file.write(reinterpret_cast<const char*>(data), len);
  file.close();

  return true;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace utils
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
