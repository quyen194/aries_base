/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 13:40
  filename:  aries_base/encryption/mac/hmac_sha256.hpp

  purpose:   HMAC-SHA256 (RFC 2104 / FIPS 198-1)
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_ENCRYPTION_MAC_HMAC_SHA256_HPP
#define ARIES_BASE_ENCRYPTION_MAC_HMAC_SHA256_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstddef>

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

// HMAC(K, m) = H((K' ^ opad) || H((K' ^ ipad) || m))
//   K' = H(K) if |K| > block size, else K; then zero-padded to block size.
//
// The key is absorbed once in the constructor: inner_key_state_ / outer_key_state_
// hold SHA256 states after hashing (K' ^ ipad) / (K' ^ opad). Every message
// then only copies those states, so computing many MACs with the same key
// (PBKDF2) costs 2 SHA256 compressions per short message instead of 4.
//
// Usage:
//   HmacSha256 hmac(key, key_len);
//   hmac.Update(part1, len1);
//   hmac.Update(part2, len2);
//   HmacSha256::Mac tag = hmac.Final();
//   hmac.Reset();                       // next message, same key
//
// Copyable on purpose (it is just three SHA256 states).
class HmacSha256 {
 public:
  static constexpr size_t kMacSize = hash::SHA256::kDigestSize;
  static constexpr size_t kBlockSize = hash::SHA256::kBlockSize;

  typedef hash::SHA256::RawDigest Mac;

 public:
  // `key` may be nullptr when `key_len` is 0.
  HmacSha256(const void* key, size_t key_len);

  // Start a new message with the same key.
  void Reset();
  // Must not be called after Final() until Reset().
  void Update(const void* data, size_t len);
  // Returns the MAC of everything passed to Update() since the last Reset().
  Mac Final();

  // One-shot helper.
  static Mac Compute(const void* key,
                     size_t key_len,
                     const void* data,
                     size_t len);

 private:
  hash::SHA256 inner_key_state_;  // after Update(K' ^ ipad), never finalized
  hash::SHA256 outer_key_state_;  // after Update(K' ^ opad), never finalized
  hash::SHA256 inner_;            // working copy for the current message
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_ENCRYPTION_MAC_HMAC_SHA256_HPP
// -----------------------------------------------------------------------------
