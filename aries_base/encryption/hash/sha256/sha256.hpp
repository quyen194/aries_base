/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/09 06:49
  filename:  aries_base/encryption/hash/sha256/sha256.hpp

  purpose:   Implementation of SHA-256 hash function
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_ENCRYPTION_HASH_SHA256_SHA256_HPP
#define ARIES_BASE_ENCRYPTION_HASH_SHA256_SHA256_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <array>
#include <cstdint>

#include "aries_base/encryption/hash/interfaces/hash.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace hash {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Copyable on purpose: HMAC snapshots the state after hashing the padded key
// and copies it for every message. Do not add DISALLOW_COPY_AND_ASSIGN.
class SHA256 : public IHash {
 public:
  static constexpr size_t kDigestSize = 32;
  static constexpr size_t kBlockSize = 64;

  typedef std::array<std::uint8_t, kDigestSize> RawDigest;

 public:
  SHA256();
  ~SHA256() override;

  void Reset() override;
  // Must not be called after Final()/FinalRaw() until Reset().
  void Update(const void* data, size_t len) override;
  // Lowercase hex digest. Final() and FinalRaw() may be called any number of
  // times, in any order, and return the same digest.
  std::string Final() override;
  RawDigest FinalRaw();

  size_t DigestSize() const override;

 private:
  void ProcessBlock(const std::uint8_t* block);
  // Applies padding exactly once and stores the digest in raw_.
  void Finalize();

 private:
  // SHA256 state variables
  std::uint32_t state_[8];
  std::uint64_t count_;
  std::uint8_t buffer_[kBlockSize];
  size_t buf_len_;
  bool finalized_;
  RawDigest raw_;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace hash
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_ENCRYPTION_HASH_SHA256_SHA256_HPP
// -----------------------------------------------------------------------------
