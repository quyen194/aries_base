/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 13:40
  filename:  aries_base/encryption/mac/hmac_sha256.cpp

  purpose:   HMAC-SHA256 (RFC 2104 / FIPS 198-1)
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstdint>
#include <cstring>

#include "aries_base/encryption/mac/hmac_sha256.hpp"
#include "aries_base/encryption/common/secure_memory.hpp"
#include "aries_base/encryption/hash/sha256/sha256.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mac {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace {
// -----------------------------------------------------------------------------

constexpr std::uint8_t kIpad = 0x36;
constexpr std::uint8_t kOpad = 0x5c;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

HmacSha256::HmacSha256(const void* key, size_t key_len) {
  // K': key hashed if longer than a block, then zero-padded to a block
  std::uint8_t k_prime[kBlockSize] = {0};
  if (key_len <= kBlockSize) {
    if (key_len) {
      memcpy(k_prime, key, key_len);
    }
  }
  else {
    hash::SHA256 hasher;
    hasher.Update(key, key_len);
    auto sign = hasher.FinalRaw();
    memcpy(k_prime, sign.data(), sign.size());
    SecureZero(sign.data(), sign.size());
  }

  std::uint8_t ipad[kBlockSize];
  std::uint8_t opad[kBlockSize];
  for (size_t i = 0; i < kBlockSize; ++i) {
    ipad[i] = k_prime[i] ^ kIpad;
    opad[i] = k_prime[i] ^ kOpad;
  }

  inner_key_state_.Update(ipad, kBlockSize);
  outer_key_state_.Update(opad, kBlockSize);
  Reset();

  SecureZero(k_prime, sizeof(k_prime));
  SecureZero(ipad, sizeof(ipad));
  SecureZero(opad, sizeof(opad));
}
// -----------------------------------------------------------------------------

void HmacSha256::Reset() {
  inner_ = inner_key_state_;
}
// -----------------------------------------------------------------------------

void HmacSha256::Update(const void* data, size_t len) {
  inner_.Update(data, len);
}
// -----------------------------------------------------------------------------

HmacSha256::Mac HmacSha256::Final() {
  Mac inner_hash = inner_.FinalRaw();

  // Work on a copy: outer_key_state_ must stay reusable for the next message.
  hash::SHA256 outer = outer_key_state_;
  outer.Update(inner_hash.data(), inner_hash.size());
  return outer.FinalRaw();
}
// -----------------------------------------------------------------------------

HmacSha256::Mac HmacSha256::Compute(const void* key,
                                    size_t key_len,
                                    const void* data,
                                    size_t len) {
  HmacSha256 hmac(key, key_len);
  hmac.Update(data, len);
  return hmac.Final();
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mac
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
