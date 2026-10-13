/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:40
  filename:  aries_base/tests/encryption/password/_pbkdf2_sha256_hasher.cpp

  purpose:   Test Pbkdf2Sha256Hasher (encode / verify / rehash)
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "aries_base/encryption/password/pbkdf2_sha256_hasher.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::password;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Low iteration count keeps the test fast; parameters are read back from the
// encoded string anyway.
constexpr std::uint32_t kTestIterations = 1000;
// -----------------------------------------------------------------------------

std::string ToHex(const std::string& data) {
  static const char kHex[] = "0123456789abcdef";
  std::string out;
  for (unsigned char b : data) {
    out += kHex[b >> 4];
    out += kHex[b & 0x0f];
  }
  return out;
}
// -----------------------------------------------------------------------------

std::vector<std::string> Split(const std::string& s, char sep) {
  std::vector<std::string> parts;
  size_t start = 0;
  for (;;) {
    size_t end = s.find(sep, start);
    parts.push_back(s.substr(start, end - start));
    if (end == std::string::npos) {
      return parts;
    }
    start = end + 1;
  }
}
// -----------------------------------------------------------------------------

void TestHashFormat() {
  std::cout << "Hash() produces pbkdf2_sha256$iter$salt$hash" << std::endl;
  Pbkdf2Sha256Hasher hasher(kTestIterations);
  std::string encoded = hasher.Hash("correct horse battery staple");
  std::cout << "  " << encoded << std::endl;

  std::vector<std::string> parts = Split(encoded, '$');
  assert(parts.size() == 4);
  assert(parts[0] == "pbkdf2_sha256");
  assert(parts[1] == std::to_string(kTestIterations));
  assert(parts[2].size() == Pbkdf2Sha256Hasher::kDefaultSaltSize * 2);
  assert(parts[3].size() == Pbkdf2Sha256Hasher::kDefaultHashSize * 2);
}
// -----------------------------------------------------------------------------

void TestVerify() {
  std::cout << "Verify() accepts the right password only" << std::endl;
  Pbkdf2Sha256Hasher hasher(kTestIterations);
  std::string encoded = hasher.Hash("s3cret");
  assert(!encoded.empty());

  bool right = hasher.Verify("s3cret", encoded);
  assert(right);
  bool wrong = hasher.Verify("s3cret ", encoded);
  assert(!wrong);
  bool empty = hasher.Verify("", encoded);
  assert(!empty);

  // Embedded NUL is part of the password, not a terminator
  std::string with_nul("ab\0cd", 5);
  std::string encoded_nul = hasher.Hash(with_nul);
  bool nul_ok = hasher.Verify(with_nul, encoded_nul);
  assert(nul_ok);
  bool nul_prefix = hasher.Verify("ab", encoded_nul);
  assert(!nul_prefix);
}
// -----------------------------------------------------------------------------

void TestSaltIsRandom() {
  std::cout << "Same password hashes differently each time" << std::endl;
  Pbkdf2Sha256Hasher hasher(kTestIterations);
  std::string a = hasher.Hash("same");
  std::string b = hasher.Hash("same");
  assert(a != b);
  bool va = hasher.Verify("same", a);
  bool vb = hasher.Verify("same", b);
  assert(va && vb);
}
// -----------------------------------------------------------------------------

void TestKnownVector() {
  // PBKDF2-HMAC-SHA256 known answer, wrapped in the encoded format:
  // P="passwordPASSWORDpassword", S="saltSALTsaltSALTsaltSALTsaltSALTsalt",
  // c=4096, dkLen=40.
  std::cout << "Verify() uses the stored parameters (known vector)" << std::endl;
  std::string encoded =
      "pbkdf2_sha256$4096$" + ToHex("saltSALTsaltSALTsaltSALTsaltSALTsalt") +
      "$348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c4e2a1fb8dd53e1"
      "c635518c7dac47e9";

  // Hasher defaults (600000 / 16 / 32) differ from the stored ones.
  Pbkdf2Sha256Hasher hasher;
  bool ok = hasher.Verify("passwordPASSWORDpassword", encoded);
  assert(ok);

  // Uppercase hex in storage is accepted too
  std::string upper = encoded;
  for (char& c : upper) {
    if (c >= 'a' && c <= 'f') {
      c = static_cast<char>(c - 'a' + 'A');
    }
  }
  upper.replace(0, 13, "pbkdf2_sha256");  // algorithm name stays lowercase
  bool ok_upper = hasher.Verify("passwordPASSWORDpassword", upper);
  assert(ok_upper);

  bool needs = hasher.NeedsRehash(encoded);
  assert(needs);
}
// -----------------------------------------------------------------------------

void TestMalformed() {
  std::cout << "Malformed encoded strings are rejected" << std::endl;
  Pbkdf2Sha256Hasher hasher(kTestIterations);
  std::string good = hasher.Hash("pw");
  std::vector<std::string> p = Split(good, '$');

  const std::vector<std::string> bad = {
      "",
      "pbkdf2_sha256",
      "pbkdf2_sha256$$$",
      "pbkdf2_sha1$" + p[1] + "$" + p[2] + "$" + p[3],      // algorithm
      p[0] + "$" + p[1] + "$" + p[2],                       // 3 fields
      good + "$extra",                                       // 5 fields
      p[0] + "$0$" + p[2] + "$" + p[3],                     // iterations 0
      p[0] + "$-1$" + p[2] + "$" + p[3],                    // sign
      p[0] + "$+1000$" + p[2] + "$" + p[3],                 // sign
      p[0] + "$ 1000$" + p[2] + "$" + p[3],                 // space
      p[0] + "$1e3$" + p[2] + "$" + p[3],                   // not decimal
      p[0] + "$99999999999$" + p[2] + "$" + p[3],           // overflow
      p[0] + "$10000001$" + p[2] + "$" + p[3],              // > kMaxIterations
      p[0] + "$" + p[1] + "$" + p[2] + "0$" + p[3],         // odd hex
      p[0] + "$" + p[1] + "$" + p[2] + "$" + p[3] + "zz",   // non-hex
      p[0] + "$" + p[1] + "$abcd$" + p[3],                  // salt too short
      p[0] + "$" + p[1] + "$" + p[2] + "$abcd",             // hash too short
      p[0] + "$" + p[1] + "$" + p[2] + "$" + std::string(130, 'a'),  // too long
  };

  for (const auto& encoded : bad) {
    bool ok = hasher.Verify("pw", encoded);
    assert(!ok);
    bool needs = hasher.NeedsRehash(encoded);
    assert(needs);
  }

  bool good_ok = hasher.Verify("pw", good);
  assert(good_ok);
}
// -----------------------------------------------------------------------------

void TestNeedsRehash() {
  std::cout << "NeedsRehash() follows the current parameters" << std::endl;
  Pbkdf2Sha256Hasher old_hasher(kTestIterations);
  Pbkdf2Sha256Hasher new_hasher(kTestIterations * 2);
  std::string encoded = old_hasher.Hash("pw");

  bool old_needs = old_hasher.NeedsRehash(encoded);
  assert(!old_needs);
  bool new_needs = new_hasher.NeedsRehash(encoded);
  assert(new_needs);

  // Old hashes still verify after the iteration count is raised
  bool still_ok = new_hasher.Verify("pw", encoded);
  assert(still_ok);

  Pbkdf2Sha256Hasher bigger_salt(kTestIterations, 32);
  bool salt_needs = bigger_salt.NeedsRehash(encoded);
  assert(salt_needs);
}
// -----------------------------------------------------------------------------

void TestClamp() {
  std::cout << "Constructor clamps parameters into limits" << std::endl;
  Pbkdf2Sha256Hasher hasher(0, 1, 1000);
  assert(hasher.iterations() == 1);
  std::vector<std::string> parts = Split(hasher.Hash("pw"), '$');
  assert(parts.size() == 4);
  assert(parts[2].size() == Pbkdf2Sha256Hasher::kMinSaltSize * 2);
  assert(parts[3].size() == Pbkdf2Sha256Hasher::kMaxHashSize * 2);
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Pbkdf2Sha256Hasher Tests ===" << std::endl << std::endl;

  TestHashFormat();
  TestVerify();
  TestSaltIsRandom();
  TestKnownVector();
  TestMalformed();
  TestNeedsRehash();
  TestClamp();

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
