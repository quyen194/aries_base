/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/10 07:05
  filename:  aries_base/tests/encryption/hash/md5.cpp

  purpose:   Demonstrate MD5 hashing using Aries Base HashFactory
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

  auto hasher = HashFactory::Create(HashType::MD5);
  if (!hasher) {
    return -1;
  }

  const std::vector<std::tuple<std::string, std::string, std::string>> tests = {
      {"",                        "",                   "d41d8cd98f00b204e9800998ecf8427e"},
      {"a",                       "a",                  "0cc175b9c0f1b6a831c399e269772661"},
      {"abc",                     "abc",                "900150983cd24fb0d6963f7d28e17f72"},
      {"message digest",          "message digest",     "f96b697d7cb7938d525a2f31aaf161d0"},
      {std::string(55, 'a'),      "aaa... (x55)",       "ef1772b6dff9a122358552954ad0df65"},
      {std::string(64, 'a'),      "aaa... (x64)",       "014842d480b571495a4a0363793f7367"},
      {std::string(1000000, 'a'), "aaa... (x1000000)",  "7707d6ae4e027c70eea2a935c2296f21"},
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
