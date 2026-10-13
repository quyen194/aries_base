/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 13:40
  filename:  aries_base/tests/encryption/mac/_hmac_sha256.cpp

  purpose:   Test HMAC-SHA256 against RFC 4231 test vectors
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "aries_base/encryption/mac/hmac_sha256.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::mac;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

std::string ToHex(const HmacSha256::Mac& mac) {
  static const char kHex[] = "0123456789abcdef";
  std::string out;
  for (std::uint8_t b : mac) {
    out += kHex[b >> 4];
    out += kHex[b & 0x0f];
  }
  return out;
}
// -----------------------------------------------------------------------------

struct TestCase {
  std::string name;
  std::string key;
  std::string data;
  std::string expected;
};
// -----------------------------------------------------------------------------

std::string Bytes(std::initializer_list<int> values) {
  std::string out;
  for (int v : values) {
    out += static_cast<char>(v);
  }
  return out;
}
// -----------------------------------------------------------------------------

std::string Range(int first, int last) {
  std::string out;
  for (int v = first; v <= last; ++v) {
    out += static_cast<char>(v);
  }
  return out;
}
// -----------------------------------------------------------------------------

const std::vector<TestCase>& Cases() {
  static const std::vector<TestCase> cases = {
      // RFC 4231 section 4
      {"RFC 4231 #1", std::string(20, '\x0b'), "Hi There",
       "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7"},
      {"RFC 4231 #2", "Jefe", "what do ya want for nothing?",
       "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843"},
      {"RFC 4231 #3", std::string(20, '\xaa'), std::string(50, '\xdd'),
       "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe"},
      {"RFC 4231 #4", Range(0x01, 0x19), std::string(50, '\xcd'),
       "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b"},
      // RFC lists only the first 128 bits; full value cross-checked with Python
      {"RFC 4231 #5", std::string(20, '\x0c'), "Test With Truncation",
       "a3b6167473100ee06e0c796c2955552bfa6f7c0a6a8aef8b93f860aab0cd20c5"},
      {"RFC 4231 #6 (key > block size)", std::string(131, '\xaa'),
       "Test Using Larger Than Block-Size Key - Hash Key First",
       "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54"},
      {"RFC 4231 #7 (key and data > block size)", std::string(131, '\xaa'),
       "This is a test using a larger than block-size key and a larger than "
       "block-size data. The key needs to be hashed before being used by the "
       "HMAC algorithm.",
       "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2"},
      // Edge cases, expected values from Python hmac/hashlib
      {"key == block size (not hashed)", std::string(64, '\xaa'),
       "exact block-size key",
       "a7bc82a329db3e78bc3e4b3fa7a350dd28caf8525ccbe80d38307091653c7250"},
      {"empty key, empty data", "", "",
       "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad"},
  };
  return cases;
}
// -----------------------------------------------------------------------------

void TestVectors() {
  std::cout << "--- Test vectors (one-shot Compute) ---" << std::endl;
  for (const auto& tc : Cases()) {
    HmacSha256::Mac mac = HmacSha256::Compute(
        tc.key.data(), tc.key.size(), tc.data.data(), tc.data.size());
    std::string actual = ToHex(mac);
    bool pass = actual == tc.expected;
    std::cout << (pass ? "  PASSED  " : "  FAILED  ") << tc.name << std::endl;
    if (!pass) {
      std::cout << "    actual   " << actual << std::endl;
      std::cout << "    expected " << tc.expected << std::endl;
    }
    assert(pass);
  }
}
// -----------------------------------------------------------------------------

void TestStreamingMatchesOneShot() {
  std::cout << "--- Streaming Update() in pieces == one-shot ---" << std::endl;
  for (const auto& tc : Cases()) {
    HmacSha256 hmac(tc.key.data(), tc.key.size());
    // Feed 1, 2, 3, ... bytes at a time to cross block boundaries unevenly.
    size_t pos = 0;
    size_t step = 1;
    while (pos < tc.data.size()) {
      size_t n = std::min(step++, tc.data.size() - pos);
      hmac.Update(tc.data.data() + pos, n);
      pos += n;
    }
    std::string actual = ToHex(hmac.Final());
    assert(actual == tc.expected);
  }
}
// -----------------------------------------------------------------------------

void TestResetReusesKey() {
  std::cout << "--- Reset() starts a new message with the same key ---" << std::endl;
  const TestCase& tc = Cases()[1];  // "Jefe"
  HmacSha256 hmac(tc.key.data(), tc.key.size());

  hmac.Update("garbage", 7);
  std::string first = ToHex(hmac.Final());
  assert(first != tc.expected);

  for (int round = 0; round < 3; ++round) {
    hmac.Reset();
    hmac.Update(tc.data.data(), tc.data.size());
    std::string actual = ToHex(hmac.Final());
    assert(actual == tc.expected);
  }
}
// -----------------------------------------------------------------------------

void TestFinalTwice() {
  std::cout << "--- Final() twice returns the same MAC ---" << std::endl;
  const TestCase& tc = Cases()[0];
  HmacSha256 hmac(tc.key.data(), tc.key.size());
  hmac.Update(tc.data.data(), tc.data.size());
  HmacSha256::Mac a = hmac.Final();
  HmacSha256::Mac b = hmac.Final();
  assert(a == b);
  assert(ToHex(a) == tc.expected);
}
// -----------------------------------------------------------------------------

void TestCopy() {
  std::cout << "--- Copy mid-message finishes independently ---" << std::endl;
  const TestCase& tc = Cases()[6];  // long key + long data
  size_t half = tc.data.size() / 2;

  HmacSha256 h1(tc.key.data(), tc.key.size());
  h1.Update(tc.data.data(), half);
  HmacSha256 h2 = h1;
  h1.Update(tc.data.data() + half, tc.data.size() - half);
  h2.Update(tc.data.data() + half, tc.data.size() - half);
  std::string d1 = ToHex(h1.Final());
  std::string d2 = ToHex(h2.Final());
  assert(d1 == tc.expected);
  assert(d2 == tc.expected);
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== HMAC-SHA256 Tests ===" << std::endl << std::endl;

  TestVectors();
  TestStreamingMatchesOneShot();
  TestResetReusesKey();
  TestFinalTwice();
  TestCopy();

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
