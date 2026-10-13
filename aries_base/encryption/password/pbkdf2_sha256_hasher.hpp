/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:40
  filename:  aries_base/encryption/password/pbkdf2_sha256_hasher.hpp

  purpose:   Password hasher based on PBKDF2-HMAC-SHA256
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_ENCRYPTION_PASSWORD_PBKDF2_SHA256_HASHER_HPP
#define ARIES_BASE_ENCRYPTION_PASSWORD_PBKDF2_SHA256_HASHER_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>
#include <string>

#include "aries_base/encryption/password/password_hasher.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace password {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Encoded format (lowercase hex for salt and hash):
//   pbkdf2_sha256$<iterations>$<salt_hex>$<hash_hex>
//   e.g. pbkdf2_sha256$600000$<32 hex chars>$<64 hex chars>
class Pbkdf2Sha256Hasher : public IPasswordHasher {
 public:
  static constexpr const char* kAlgorithm = "pbkdf2_sha256";

  // OWASP 2023 recommendation for PBKDF2-HMAC-SHA256.
  static constexpr std::uint32_t kDefaultIterations = 600000;
  static constexpr size_t kDefaultSaltSize = 16;
  static constexpr size_t kDefaultHashSize = 32;

  // Limits applied when parsing a stored string, so a corrupted / tampered
  // value cannot make Verify() run for minutes or allocate huge buffers.
  static constexpr std::uint32_t kMaxIterations = 10000000;
  static constexpr size_t kMinSaltSize = 8;
  static constexpr size_t kMaxSaltSize = 64;
  static constexpr size_t kMinHashSize = 16;
  static constexpr size_t kMaxHashSize = 64;

 public:
  // Parameters used by Hash() and NeedsRehash(). Values outside the limits
  // above are clamped into them.
  explicit Pbkdf2Sha256Hasher(std::uint32_t iterations = kDefaultIterations,
                              size_t salt_size = kDefaultSaltSize,
                              size_t hash_size = kDefaultHashSize);
  ~Pbkdf2Sha256Hasher() override = default;

  std::string Hash(const std::string& password) override;
  bool Verify(const std::string& password, const std::string& encoded) override;
  bool NeedsRehash(const std::string& encoded) const override;

  std::uint32_t iterations() const { return iterations_; }

 private:
  std::uint32_t iterations_;
  size_t salt_size_;
  size_t hash_size_;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace password
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_ENCRYPTION_PASSWORD_PBKDF2_SHA256_HASHER_HPP
// -----------------------------------------------------------------------------
