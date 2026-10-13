/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:30
  filename:  aries_base/encryption/common/secure_memory.hpp

  purpose:   Memory helpers for secret data (wipe, constant-time compare)
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_ENCRYPTION_COMMON_SECURE_MEMORY_HPP
#define ARIES_BASE_ENCRYPTION_COMMON_SECURE_MEMORY_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstddef>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Overwrites `len` bytes with zeros. Unlike a plain memset right before the
// buffer goes out of scope, the stores cannot be removed as dead by the
// optimizer.
void SecureZero(void* buf, size_t len);

// Compares two buffers in time that depends only on `len`, not on where they
// first differ. Use it for MACs / password hashes. `len` itself is not secret.
bool ConstantTimeEqual(const void* a, const void* b, size_t len);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_ENCRYPTION_COMMON_SECURE_MEMORY_HPP
// -----------------------------------------------------------------------------
