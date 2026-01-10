/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/10 07:35
  filename:  aries_base/tests/encryption/hash/sha384.cpp

  purpose:   Demonstrate SHA-384 hashing using Aries Base HashFactory
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

  auto hasher = HashFactory::Create(HashType::SHA384);
  if (!hasher) {
    return -1;
  }

  const std::vector<std::tuple<std::string, std::string, std::string>> tests = {
      {"",                        "",                   "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b"},
      {"a",                       "a",                  "54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31"},
      {"abc",                     "abc",                "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7"},
      {"message digest",          "message digest",     "473ed35167ec1f5d8e550368a3db39be54639f828868e9454c239fc8b52e3c61dbd0d8b4de1390c256dcbb5d5fd99cd5"},
      {std::string(55, 'a'),      "aaa... (x55)",       "5d91ac7e74e62b5c728904b40f10784d66b7af9cb6302123e48c92f0432ceb8d2a92c02de77dcb29ed75c4b42bde46f4"},
      {std::string(64, 'a'),      "aaa... (x64)",       "2e404b9339da795776e510d96930b3be2904c500395b8cb7413334b82d4dec413b4b8113045a05bbbcff846f027423f6"},
      {std::string(1000000, 'a'), "aaa... (x1000000)",  "9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985"},
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
