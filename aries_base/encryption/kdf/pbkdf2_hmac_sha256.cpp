/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:30
  filename:  aries_base/encryption/kdf/pbkdf2_hmac_sha256.cpp

  purpose:   PBKDF2 with HMAC-SHA256 as PRF (RFC 8018 section 5.2)
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstring>

#include "aries_base/encryption/kdf/pbkdf2_hmac_sha256.hpp"
#include "aries_base/encryption/common/secure_memory.hpp"
#include "aries_base/encryption/mac/hmac_sha256.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace kdf {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace {
// -----------------------------------------------------------------------------

// RFC 8018: dkLen > (2^32 - 1) * hLen -> "derived key too long"
constexpr std::uint64_t kMaxBlocks = 0xFFFFFFFFull;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

bool Pbkdf2HmacSha256(const void* password,
                      size_t password_len,
                      const void* salt,
                      size_t salt_len,
                      std::uint32_t iterations,
                      std::uint8_t* out,
                      size_t out_len) {
  typedef mac::HmacSha256::Mac Block;

  if (iterations == 0 || !out || out_len == 0) {
    return false;
  }
  if (static_cast<std::uint64_t>(out_len) >
      kMaxBlocks * mac::HmacSha256::kMacSize) {
    return false;
  }

  // The password is the HMAC key for every U_j: absorb it once, then each
  // iteration only Reset()s to the precomputed key state.
  mac::HmacSha256 hmac(password, password_len);

  Block u;
  Block t;
  std::uint32_t block_index = 1;
  size_t written = 0;

  while (written < out_len) {
    std::uint8_t counter[4] = {
        static_cast<std::uint8_t>(block_index >> 24),
        static_cast<std::uint8_t>(block_index >> 16),
        static_cast<std::uint8_t>(block_index >> 8),
        static_cast<std::uint8_t>(block_index),
    };

    // U_1 = HMAC(P, S || INT32_BE(i))
    hmac.Reset();
    hmac.Update(salt, salt_len);
    hmac.Update(counter, sizeof(counter));
    u = hmac.Final();
    t = u;

    // U_j = HMAC(P, U_{j-1}),  T_i ^= U_j
    for (std::uint32_t j = 1; j < iterations; ++j) {
      hmac.Reset();
      hmac.Update(u.data(), u.size());
      u = hmac.Final();
      for (size_t k = 0; k < t.size(); ++k) {
        t[k] ^= u[k];
      }
    }

    size_t remaining = out_len - written;
    size_t n = remaining < t.size() ? remaining : t.size();
    memcpy(out + written, t.data(), n);
    written += n;
    ++block_index;
  }

  SecureZero(u.data(), u.size());
  SecureZero(t.data(), t.size());

  return true;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace kdf
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
