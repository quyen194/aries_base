/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/10 07:20
  filename:  aries_base/tests/encryption/hash/sha1.cpp

  purpose:   Demonstrate SHA-1 hashing using Aries Base HashFactory
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <string>
#include <tuple>

#include "aries_base/encryption/hash/hash_factory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::hash;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

bool execute_test_case(std::unique_ptr<IHash>& hasher,
                       const std::string& data,
                       const std::string& display,
                       const std::string& expected_digest) {
  hasher->Reset();
  hasher->Update(data.data(), data.size());
  std::string digest = hasher->Final();

  bool pass = digest == expected_digest;

  // Print the MD5 hash in hexadecimal format
  std::cout << "ACTUAL   MD5" << "(\"" << display << "\") = " << digest << std::endl;
  std::cout << "EXPECTED MD5" << "(\"" << display << "\") = " << expected_digest << std::endl;
  std::cout << (pass ? "====> ✓✓✓ PASSED ✓✓✓ <====" : "====> ✗✗✗ FAILED ✗✗✗ <====") << std::endl;
  std::cout << std::endl;

  return pass;
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== MD5 Hashing Tests ===" << std::endl << std::endl;

  auto hasher = HashFactory::Create(HashType::SHA1);
  if (!hasher) {
    return -1;
  }

  const std::vector<std::tuple<std::string, std::string, std::string>> tests = {
      {"",                        "",                   "da39a3ee5e6b4b0d3255bfef95601890afd80709"},
      {"a",                       "a",                  "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"},
      {"abc",                     "abc",                "a9993e364706816aba3e25717850c26c9cd0d89d"},
      {"message digest",          "message digest",     "c12252ceda8be8994d5fa0290a47231c1d16aae3"},
      {std::string(55, 'a'),      "aaa... (x55)",       "c1c8bbdc22796e28c0e15163d20899b65621d65a"},
      {std::string(64, 'a'),      "aaa... (x64)",       "0098ba824b5c16427bd7a1122a5a442a25ec644d"},
      {std::string(1000000, 'a'), "aaa... (x1000000)",  "34aa973cd4c4daa4f61eeb2bdbad27316534016f"},
  };

  int i = 1;
  for (const auto& [data, display, expected_digest] : tests) {
    std::cout << "Execute testcase " << i++ << std::endl;
    assert(execute_test_case(hasher, data, display, expected_digest));
  }

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
