/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 11:10
  filename:  aries_base/tests/encryption/random/_secure_random.cpp

  purpose:   Test SecureRandomBytes (OS CSPRNG wrapper)
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "aries_base/encryption/random/random.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::random;
using aries_base::utils::bytes;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void TestZeroLength() {
  std::cout << "Zero length (nullptr allowed)" << std::endl;
  bool ok = SecureRandomBytes(nullptr, 0);
  assert(ok);
  bytes empty = SecureRandomBytes(0);
  assert(empty.empty());
}
// -----------------------------------------------------------------------------

void TestNullBuffer() {
  std::cout << "Null buffer with non-zero length fails" << std::endl;
  bool ok = SecureRandomBytes(nullptr, 16);
  assert(!ok);
}
// -----------------------------------------------------------------------------

void TestTwoCallsDiffer() {
  std::cout << "Two 32-byte draws differ and are not all zero" << std::endl;
  bytes a = SecureRandomBytes(32);
  bytes b = SecureRandomBytes(32);
  assert(a.size() == 32);
  assert(b.size() == 32);
  assert(a != b);
  assert(a != bytes(32, 0));
}
// -----------------------------------------------------------------------------

void TestWholeBufferWritten(size_t len) {
  // Pre-fill with a sentinel; after the call only ~len/256 bytes should still
  // equal it by chance. A chunking / short-read bug leaves a large tail intact.
  std::cout << "Whole buffer written (" << len << " bytes)" << std::endl;
  const std::uint8_t kSentinel = 0xAA;
  bytes buf(len, kSentinel);
  bool ok = SecureRandomBytes(buf.data(), buf.size());
  assert(ok);

  size_t untouched = 0;
  for (std::uint8_t b : buf) {
    if (b == kSentinel) {
      ++untouched;
    }
  }
  std::cout << "  bytes still equal to sentinel: " << untouched << std::endl;
  assert(untouched < len / 256 * 4 + 8);

  // Tail must not be the sentinel block a truncated write would leave behind.
  size_t tail_len = len < 32 ? len : 32;
  assert(!std::equal(buf.end() - tail_len, buf.end(), bytes(tail_len, kSentinel).begin()));
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== SecureRandomBytes Tests ===" << std::endl << std::endl;

  TestZeroLength();
  TestNullBuffer();
  TestTwoCallsDiffer();
  TestWholeBufferWritten(257);    // just over the getentropy 256-byte chunk
  TestWholeBufferWritten(1000);
  TestWholeBufferWritten(65536);

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
