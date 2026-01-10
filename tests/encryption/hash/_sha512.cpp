/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/10 07:41
  filename:  aries_base/tests/encryption/hash/sha512.cpp

  purpose:   Demonstrate SHA-512 hashing using Aries Base HashFactory
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

  auto hasher = HashFactory::Create(HashType::SHA512);
  if (!hasher) {
    return -1;
  }

  const std::vector<std::tuple<std::string, std::string, std::string>> tests = {
      {"",                        "",                   "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"},
      {"a",                       "a",                  "1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75"},
      {"abc",                     "abc",                "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"},
      {"message digest",          "message digest",     "107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c"},
      {std::string(55, 'a'),      "aaa... (x55)",       "b0220c772cbf6c1822e2cb38a437d0e1d58772417a4bbb21c961364f8b6143e05aa6316dca8d1d7b19e16448419076395f6086cb55101fbd6d5497b148e1745f"},
      {std::string(64, 'a'),      "aaa... (x64)",       "01d35c10c6c38c2dcf48f7eebb3235fb5ad74a65ec4cd016e2354c637a8fb49b695ef3c1d6f7ae4cd74d78cc9c9bcac9d4f23a73019998a7f73038a5c9b2dbde"},
      {std::string(1000000, 'a'), "aaa... (x1000000)",  "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b"},
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
