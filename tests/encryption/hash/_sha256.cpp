/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/10 07:30
  filename:  aries_base/tests/encryption/hash/sha256.cpp

  purpose:   Demonstrate SHA-256 hashing using Aries Base HashFactory
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

  auto hasher = HashFactory::Create(HashType::SHA256);
  if (!hasher) {
    return -1;
  }

  const std::vector<std::tuple<std::string, std::string, std::string>> tests = {
      {"",                        "",                   "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
      {"a",                       "a",                  "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb"},
      {"abc",                     "abc",                "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
      {"message digest",          "message digest",     "f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650"},
      {std::string(55, 'a'),      "aaa... (x55)",       "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318"},
      {std::string(64, 'a'),      "aaa... (x64)",       "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb"},
      {std::string(1000000, 'a'), "aaa... (x1000000)",  "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"},
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
