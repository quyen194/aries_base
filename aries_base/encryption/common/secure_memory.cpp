/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:30
  filename:  aries_base/encryption/common/secure_memory.cpp

  purpose:   Memory helpers for secret data (wipe, constant-time compare)
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstdint>

#include "aries_base/encryption/common/secure_memory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void SecureZero(void* buf, size_t len) {
  if (!buf) {
    return;
  }

  // Volatile stores are observable behavior, so they are never elided.
  volatile std::uint8_t* p = static_cast<volatile std::uint8_t*>(buf);
  while (len--) {
    *p++ = 0;
  }
}
// -----------------------------------------------------------------------------

bool ConstantTimeEqual(const void* a, const void* b, size_t len) {
  if (len == 0) {
    return true;
  }
  if (!a || !b) {
    return false;
  }

  // Volatile reads keep the compiler from turning the loop into an
  // early-exit memcmp.
  const volatile std::uint8_t* pa = static_cast<const volatile std::uint8_t*>(a);
  const volatile std::uint8_t* pb = static_cast<const volatile std::uint8_t*>(b);

  std::uint8_t diff = 0;
  for (size_t i = 0; i < len; ++i) {
    diff |= static_cast<std::uint8_t>(pa[i] ^ pb[i]);
  }
  return diff == 0;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
