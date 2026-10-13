/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:30
  filename:  aries_base/tests/encryption/kdf/_pbkdf2_hmac_sha256.cpp

  purpose:   Test PBKDF2-HMAC-SHA256 against RFC 7914 / known vectors
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "aries_base/encryption/kdf/pbkdf2_hmac_sha256.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::kdf;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

std::string ToHex(const std::vector<std::uint8_t>& data) {
  static const char kHex[] = "0123456789abcdef";
  std::string out;
  for (std::uint8_t b : data) {
    out += kHex[b >> 4];
    out += kHex[b & 0x0f];
  }
  return out;
}
// -----------------------------------------------------------------------------

struct TestCase {
  std::string name;
  std::string password;
  std::string salt;
  std::uint32_t iterations;
  size_t dk_len;
  std::string expected;
};
// -----------------------------------------------------------------------------

void TestVectors() {
  std::cout << "--- Test vectors ---" << std::endl;

  // Expected values cross-checked with Python hashlib.pbkdf2_hmac("sha256")
  const std::vector<TestCase> cases = {
      {"RFC 7914 #1 (c=1, 2 blocks)", "passwd", "salt", 1, 64,
       "55ac046e56e3089fec1691c22544b605f94185216dde0465e68b9d57c20dacbc"
       "49ca9cccf179b645991664b39d77ef317c71b845b1e30bd509112041d3a19783"},
      {"RFC 7914 #2 (c=80000, 2 blocks)", "Password", "NaCl", 80000, 64,
       "4ddcd8f60b98be21830cee5ef22701f9641a4418d04c0414aeff08876b34ab56"
       "a1d425a1225833549adb841b51c9b3176a272bdebba1d078478f62b397f33c8d"},
      {"c=1", "password", "salt", 1, 32,
       "120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b"},
      {"c=2", "password", "salt", 2, 32,
       "ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43"},
      {"c=4096", "password", "salt", 4096, 32,
       "c5e478d59288c841aa530db6845c4c8d962893a001ce4e11a4963873aa98134a"},
      {"long P/S, dkLen=40 (truncated last block)",
       "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
       4096, 40,
       "348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c4e2a1fb8dd53e1"
       "c635518c7dac47e9"},
      {"embedded NUL, dkLen=16", std::string("pass\0word", 9),
       std::string("sa\0lt", 5), 4096, 16,
       "89b69d0516f829893c696226650a8687"},
  };

  for (const auto& tc : cases) {
    std::vector<std::uint8_t> dk(tc.dk_len);
    bool ok = Pbkdf2HmacSha256(tc.password.data(), tc.password.size(),
                               tc.salt.data(), tc.salt.size(),
                               tc.iterations, dk.data(), dk.size());
    std::string actual = ToHex(dk);
    bool pass = ok && actual == tc.expected;
    std::cout << (pass ? "  PASSED  " : "  FAILED  ") << tc.name << std::endl;
    if (!pass) {
      std::cout << "    actual   " << actual << std::endl;
      std::cout << "    expected " << tc.expected << std::endl;
    }
    assert(pass);
  }
}
// -----------------------------------------------------------------------------

void TestInvalidInput() {
  std::cout << "--- Invalid input is rejected ---" << std::endl;
  std::vector<std::uint8_t> dk(32);

  bool zero_iter = Pbkdf2HmacSha256("p", 1, "s", 1, 0, dk.data(), dk.size());
  assert(!zero_iter);

  bool null_out = Pbkdf2HmacSha256("p", 1, "s", 1, 1, nullptr, 32);
  assert(!null_out);

  bool zero_len = Pbkdf2HmacSha256("p", 1, "s", 1, 1, dk.data(), 0);
  assert(!zero_len);

  // Empty password / salt are valid inputs
  bool empty_ok = Pbkdf2HmacSha256(nullptr, 0, nullptr, 0, 1, dk.data(), dk.size());
  assert(empty_ok);
}
// -----------------------------------------------------------------------------

void TestPrefixConsistency() {
  // Asking for fewer bytes must return a prefix of the longer output.
  std::cout << "--- Shorter dkLen is a prefix of longer dkLen ---" << std::endl;
  std::vector<std::uint8_t> long_dk(100);
  std::vector<std::uint8_t> short_dk(33);
  bool ok1 = Pbkdf2HmacSha256("pw", 2, "salt", 4, 10, long_dk.data(), long_dk.size());
  bool ok2 = Pbkdf2HmacSha256("pw", 2, "salt", 4, 10, short_dk.data(), short_dk.size());
  assert(ok1 && ok2);
  for (size_t i = 0; i < short_dk.size(); ++i) {
    assert(short_dk[i] == long_dk[i]);
  }
}
// -----------------------------------------------------------------------------

void Benchmark(std::uint32_t iterations) {
  // Informational only: pick the production iteration count from this.
  std::vector<std::uint8_t> dk(32);
  auto start = std::chrono::steady_clock::now();
  bool ok = Pbkdf2HmacSha256("benchmark-password", 18, "0123456789abcdef", 16,
                             iterations, dk.data(), dk.size());
  auto end = std::chrono::steady_clock::now();
  assert(ok);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "--- Benchmark: " << iterations << " iterations, dkLen 32: "
            << ms.count() << " ms ---" << std::endl;
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== PBKDF2-HMAC-SHA256 Tests ===" << std::endl << std::endl;

  TestVectors();
  TestInvalidInput();
  TestPrefixConsistency();
  Benchmark(600000);

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
