/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:30
  filename:  aries_base/tests/encryption/common/_secure_memory.cpp

  purpose:   Test SecureZero and ConstantTimeEqual
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "aries_base/encryption/common/secure_memory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void TestSecureZero() {
  std::cout << "SecureZero wipes the whole buffer" << std::endl;
  std::vector<std::uint8_t> buf(100, 0xAB);
  SecureZero(buf.data(), buf.size());
  for (std::uint8_t b : buf) {
    assert(b == 0);
  }

  // Partial wipe leaves the rest intact
  std::vector<std::uint8_t> part(10, 0xCD);
  SecureZero(part.data(), 4);
  assert(part[3] == 0);
  assert(part[4] == 0xCD);

  // No-ops
  SecureZero(nullptr, 10);
  SecureZero(buf.data(), 0);
}
// -----------------------------------------------------------------------------

void TestConstantTimeEqual() {
  std::cout << "ConstantTimeEqual" << std::endl;
  std::vector<std::uint8_t> a(32);
  for (size_t i = 0; i < a.size(); ++i) {
    a[i] = static_cast<std::uint8_t>(i * 7 + 1);
  }

  std::vector<std::uint8_t> b = a;
  bool equal = ConstantTimeEqual(a.data(), b.data(), a.size());
  assert(equal);

  // A single differing bit anywhere must be detected
  for (size_t pos : {size_t(0), size_t(15), size_t(31)}) {
    std::vector<std::uint8_t> c = a;
    c[pos] ^= 0x01;
    bool same = ConstantTimeEqual(a.data(), c.data(), a.size());
    assert(!same);
  }

  // Only the first `len` bytes are compared
  std::vector<std::uint8_t> d = a;
  d[31] ^= 0xFF;
  bool prefix_equal = ConstantTimeEqual(a.data(), d.data(), 31);
  assert(prefix_equal);

  bool empty_equal = ConstantTimeEqual(nullptr, nullptr, 0);
  assert(empty_equal);
  bool null_vs_data = ConstantTimeEqual(nullptr, a.data(), a.size());
  assert(!null_vs_data);
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Secure Memory Tests ===" << std::endl << std::endl;

  TestSecureZero();
  TestConstantTimeEqual();

  std::cout << std::endl << "=== All tests passed! ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
