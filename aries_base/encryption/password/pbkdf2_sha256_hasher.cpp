/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:40
  filename:  aries_base/encryption/password/pbkdf2_sha256_hasher.cpp

  purpose:   Password hasher based on PBKDF2-HMAC-SHA256
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/encryption/password/pbkdf2_sha256_hasher.hpp"

#include "aries_base/encryption/common/secure_memory.hpp"
#include "aries_base/encryption/kdf/pbkdf2_hmac_sha256.hpp"
#include "aries_base/encryption/random/random.hpp"
#include "aries_base/utils/bytes.hpp"
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
namespace {
// -----------------------------------------------------------------------------

constexpr char kSeparator = '$';
// -----------------------------------------------------------------------------

struct Encoded {
  std::uint32_t iterations = 0;
  utils::bytes salt;
  utils::bytes hash;
};
// -----------------------------------------------------------------------------

template <typename T>
T Clamp(T value, T low, T high) {
  return value < low ? low : (value > high ? high : value);
}
// -----------------------------------------------------------------------------

std::string ToHex(const utils::bytes& data) {
  static const char kHex[] = "0123456789abcdef";
  std::string out;
  out.reserve(data.size() * 2);
  for (std::uint8_t b : data) {
    out += kHex[b >> 4];
    out += kHex[b & 0x0f];
  }
  return out;
}
// -----------------------------------------------------------------------------

int HexValue(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
  return -1;
}
// -----------------------------------------------------------------------------

bool FromHex(const std::string& hex, utils::bytes& out) {
  if (hex.size() % 2 != 0) {
    return false;
  }

  out.assign(hex.size() / 2, 0);
  for (size_t i = 0; i < out.size(); ++i) {
    int hi = HexValue(hex[i * 2]);
    int lo = HexValue(hex[i * 2 + 1]);
    if (hi < 0 || lo < 0) {
      return false;
    }
    out[i] = static_cast<std::uint8_t>((hi << 4) | lo);
  }
  return true;
}
// -----------------------------------------------------------------------------

// Strict decimal: digits only, no sign / spaces, within [1, kMaxIterations].
bool ParseIterations(const std::string& text, std::uint32_t& out) {
  if (text.empty() || text.size() > 10) {
    return false;
  }

  std::uint64_t value = 0;
  for (char c : text) {
    if (c < '0' || c > '9') {
      return false;
    }
    value = value * 10 + static_cast<std::uint64_t>(c - '0');
  }

  if (value < 1 || value > Pbkdf2Sha256Hasher::kMaxIterations) {
    return false;
  }
  out = static_cast<std::uint32_t>(value);
  return true;
}
// -----------------------------------------------------------------------------

bool Parse(const std::string& encoded, Encoded& out) {
  // Exactly 4 fields: algorithm $ iterations $ salt $ hash
  std::string fields[4];
  size_t start = 0;
  for (int i = 0; i < 4; ++i) {
    size_t end = encoded.find(kSeparator, start);
    if (i < 3) {
      if (end == std::string::npos) {
        return false;
      }
      fields[i] = encoded.substr(start, end - start);
      start = end + 1;
    }
    else {
      if (end != std::string::npos) {
        return false;
      }
      fields[i] = encoded.substr(start);
    }
  }

  if (fields[0] != Pbkdf2Sha256Hasher::kAlgorithm) {
    return false;
  }
  if (!ParseIterations(fields[1], out.iterations)) {
    return false;
  }
  if (!FromHex(fields[2], out.salt) || !FromHex(fields[3], out.hash)) {
    return false;
  }
  if (out.salt.size() < Pbkdf2Sha256Hasher::kMinSaltSize ||
      out.salt.size() > Pbkdf2Sha256Hasher::kMaxSaltSize) {
    return false;
  }
  if (out.hash.size() < Pbkdf2Sha256Hasher::kMinHashSize ||
      out.hash.size() > Pbkdf2Sha256Hasher::kMaxHashSize) {
    return false;
  }
  return true;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

Pbkdf2Sha256Hasher::Pbkdf2Sha256Hasher(std::uint32_t iterations,
                                       size_t salt_size,
                                       size_t hash_size)
    : iterations_(Clamp<std::uint32_t>(iterations, 1, kMaxIterations)),
      salt_size_(Clamp<size_t>(salt_size, kMinSaltSize, kMaxSaltSize)),
      hash_size_(Clamp<size_t>(hash_size, kMinHashSize, kMaxHashSize)) {
}
// -----------------------------------------------------------------------------

std::string Pbkdf2Sha256Hasher::Hash(const std::string& password) {
  utils::bytes salt = random::SecureRandomBytes(salt_size_);
  if (salt.size() != salt_size_) {
    return std::string();
  }

  utils::bytes derived(hash_size_);
  bool ok = kdf::Pbkdf2HmacSha256(password.data(), password.size(),
                                  salt.data(), salt.size(),
                                  iterations_,
                                  derived.data(), derived.size());
  if (!ok) {
    return std::string();
  }

  std::string encoded = std::string(kAlgorithm) + kSeparator +
                        std::to_string(iterations_) + kSeparator +
                        ToHex(salt) + kSeparator +
                        ToHex(derived);

  SecureZero(derived.data(), derived.size());
  return encoded;
}
// -----------------------------------------------------------------------------

bool Pbkdf2Sha256Hasher::Verify(const std::string& password,
                                const std::string& encoded) {
  Encoded stored;
  if (!Parse(encoded, stored)) {
    return false;
  }

  // Derive with the stored parameters, not the current defaults.
  utils::bytes derived(stored.hash.size());
  bool ok = kdf::Pbkdf2HmacSha256(password.data(), password.size(),
                                  stored.salt.data(), stored.salt.size(),
                                  stored.iterations,
                                  derived.data(), derived.size());

  bool match = ok && ConstantTimeEqual(derived.data(),
                                       stored.hash.data(),
                                       derived.size());

  SecureZero(derived.data(), derived.size());
  return match;
}
// -----------------------------------------------------------------------------

bool Pbkdf2Sha256Hasher::NeedsRehash(const std::string& encoded) const {
  Encoded stored;
  if (!Parse(encoded, stored)) {
    return true;
  }

  return stored.iterations != iterations_ ||
         stored.salt.size() != salt_size_ ||
         stored.hash.size() != hash_size_;
}
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
