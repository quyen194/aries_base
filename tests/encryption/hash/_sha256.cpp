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
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>

#include "aries_base/encryption/hash/hash_factory.hpp"
#include "aries_base/encryption/hash/sha256/sha256.hpp"
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

std::string ToHex(const SHA256::RawDigest& raw) {
  static const char kHex[] = "0123456789abcdef";
  std::string out;
  for (std::uint8_t b : raw) {
    out += kHex[b >> 4];
    out += kHex[b & 0x0f];
  }
  return out;
}
// -----------------------------------------------------------------------------

const std::string kAbcDigest =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
// -----------------------------------------------------------------------------

void TestFinalRawMatchesVector(const std::string& data,
                               const std::string& expected_digest) {
  SHA256 hasher;
  hasher.Update(data.data(), data.size());
  std::string raw_hex = ToHex(hasher.FinalRaw());
  assert(raw_hex == expected_digest);
}
// -----------------------------------------------------------------------------

void TestFinalRawThenFinal() {
  std::cout << "FinalRaw() then Final() / FinalRaw() twice: no double padding" << std::endl;
  SHA256 hasher;
  hasher.Update("abc", 3);
  SHA256::RawDigest raw1 = hasher.FinalRaw();
  std::string hex = hasher.Final();
  SHA256::RawDigest raw2 = hasher.FinalRaw();
  assert(ToHex(raw1) == kAbcDigest);
  assert(hex == kAbcDigest);
  assert(raw1 == raw2);

  SHA256 hasher2;
  hasher2.Update("abc", 3);
  std::string hex2 = hasher2.Final();
  SHA256::RawDigest raw3 = hasher2.FinalRaw();
  assert(hex2 == kAbcDigest);
  assert(ToHex(raw3) == kAbcDigest);
}
// -----------------------------------------------------------------------------

void TestResetAfterFinal() {
  std::cout << "Reset() after Final() makes the object reusable" << std::endl;
  SHA256 hasher;
  hasher.Update("garbage", 7);
  std::string first = hasher.Final();
  hasher.Reset();
  hasher.Update("abc", 3);
  std::string second = hasher.Final();
  assert(first != kAbcDigest);
  assert(second == kAbcDigest);
}
// -----------------------------------------------------------------------------

void TestCopyMidStream() {
  // HMAC relies on snapshotting a partially-updated state and copying it.
  std::cout << "Copy mid-stream: both copies finish independently" << std::endl;
  SHA256 h1;
  h1.Update("ab", 2);
  SHA256 h2 = h1;
  h1.Update("c", 1);
  h2.Update("c", 1);
  std::string d1 = h1.Final();
  std::string d2 = h2.Final();
  assert(d1 == kAbcDigest);
  assert(d2 == kAbcDigest);

  // Copy across a block boundary (> 64 bytes buffered + processed).
  std::string long_a(100, 'a');
  SHA256 h3;
  h3.Update(long_a.data(), 70);
  SHA256 h4 = h3;
  h4.Update(long_a.data() + 70, 30);
  SHA256 ref;
  ref.Update(long_a.data(), long_a.size());
  std::string d4 = h4.Final();
  std::string dref = ref.Final();
  assert(d4 == dref);
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
    bool pass = execute_test_case(hasher, data, display, expected_digest);
    assert(pass);
    TestFinalRawMatchesVector(data, expected_digest);
  }

  std::cout << "=== SHA256::FinalRaw Tests ===" << std::endl << std::endl;
  TestFinalRawThenFinal();
  TestResetAfterFinal();
  TestCopyMidStream();

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
