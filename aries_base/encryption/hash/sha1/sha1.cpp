/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/09 15:35
  filename:  aries_base/encryption/hash/sha1/sha1.hpp

  purpose:   Implementation of SHA-1 hash function
*********************************************************************/

// -----------------------------------------------------------------------------
#include <cstring>
#include <cstdint>
#include <string>
#include <cstdio>  // for snprintf

#include "aries_base/encryption/hash/sha1/sha1.hpp"
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
namespace {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// SHA-1 round constants
constexpr std::uint32_t K0 = 0x5a827999; // 0..19
constexpr std::uint32_t K1 = 0x6ed9eba1; // 20..39
constexpr std::uint32_t K2 = 0x8f1bbcdc; // 40..59
constexpr std::uint32_t K3 = 0xca62c1d6; // 60..79
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

inline std::uint32_t rotl(std::uint32_t x, std::uint32_t n) {
  return (x << n) | (x >> (32 - n));
}
// -----------------------------------------------------------------------------

inline std::uint32_t f0(std::uint32_t b, std::uint32_t c, std::uint32_t d) {
  return (b & c) | (~b & d);
}
// -----------------------------------------------------------------------------

inline std::uint32_t f1(std::uint32_t b, std::uint32_t c, std::uint32_t d) {
  return b ^ c ^ d;
}
// -----------------------------------------------------------------------------

inline std::uint32_t f2(std::uint32_t b, std::uint32_t c, std::uint32_t d) {
  return (b & c) | (b & d) | (c & d);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

SHA1::SHA1() {
  // init
  Reset();
}
// -----------------------------------------------------------------------------

SHA1::~SHA1() = default;
// -----------------------------------------------------------------------------

void SHA1::Reset() {
  // Initial SHA-1 state (H0..H4)
  state_[0] = 0x67452301;
  state_[1] = 0xefcdab89;
  state_[2] = 0x98badcfe;
  state_[3] = 0x10325476;
  state_[4] = 0xc3d2e1f0;

  count_ = 0;
  buf_len_ = 0;
  digest_.clear();
}
// -----------------------------------------------------------------------------

void SHA1::Update(const void* data, size_t len) {
  const std::uint8_t* input = static_cast<const std::uint8_t*>(data);
  size_t index = buf_len_;

  count_ += len;

  // Fill remaining buffer if any
  if (index > 0) {
    size_t to_copy = (len < (64 - index)) ? len : (64 - index);
    std::memcpy(buffer_ + index, input, to_copy);
    input += to_copy;
    len -= to_copy;
    index += to_copy;

    if (index == 64) {
      ProcessBlock(buffer_);
      index = 0;
    }
  }

  // Process full 64-byte blocks
  while (len >= 64) {
    ProcessBlock(input);
    input += 64;
    len -= 64;
  }

  // Save remaining data
  if (len > 0) {
    std::memcpy(buffer_, input, len);
    index = len;
  }

  buf_len_ = index;
}
// -----------------------------------------------------------------------------

std::string SHA1::Final() {
  if (!digest_.empty()) {
    return digest_;
  }

  std::uint64_t bit_len = count_ * 8;

  size_t index = buf_len_;

  // Append 0x80
  buffer_[index++] = 0x80;

  // If not enough space for length (need 8 bytes), pad current block and start
  // new
  if (index > 56) {
    std::memset(buffer_ + index, 0, 64 - index);
    ProcessBlock(buffer_);
    index = 0;
  }

  // Pad zeros up to byte 55
  std::memset(buffer_ + index, 0, 56 - index);

  // Append length as big-endian 64-bit
  for (int i = 0; i < 8; ++i) {
    buffer_[56 + i] = static_cast<std::uint8_t>(bit_len >> (56 - 8 * i));
  }

  ProcessBlock(buffer_);

  // Convert state to lowercase hex string (big-endian per word)
  digest_.reserve(40);
  for (int i = 0; i < 5; ++i) {
    for (int j = 3; j >= 0; --j) {
      char hex[3];
      std::snprintf(hex,
                    sizeof(hex),
                    "%02x",
                    static_cast<unsigned int>((state_[i] >> (j * 8)) & 0xff));
      digest_ += hex;
    }
  }

  return digest_;
}
// -----------------------------------------------------------------------------

size_t SHA1::DigestSize() const {
  // SHA1 produces a 20-byte digest
  return 20;
}
// -----------------------------------------------------------------------------

void SHA1::ProcessBlock(const std::uint8_t* block) {
  std::uint32_t w[80];

  // Load 16 words (big-endian)
  for (int i = 0; i < 16; ++i) {
    w[i] = (static_cast<std::uint32_t>(block[i * 4 + 0]) << 24) |
           (static_cast<std::uint32_t>(block[i * 4 + 1]) << 16) |
           (static_cast<std::uint32_t>(block[i * 4 + 2]) << 8)  |
           (static_cast<std::uint32_t>(block[i * 4 + 3]));
  }

  // Extend to 80 words:
  // w[i] = ROTL1(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16])
  for (int i = 16; i < 80; ++i) {
    w[i] = rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
  }

  // Initialize working variables
  std::uint32_t a = state_[0];
  std::uint32_t b = state_[1];
  std::uint32_t c = state_[2];
  std::uint32_t d = state_[3];
  std::uint32_t e = state_[4];

  for (int i = 0; i < 80; ++i) {
    std::uint32_t f, k;
    if (i < 20) {
      f = f0(b, c, d);
      k = K0;
    } else if (i < 40) {
      f = f1(b, c, d);
      k = K1;
    } else if (i < 60) {
      f = f2(b, c, d);
      k = K2;
    } else {
      f = f1(b, c, d);
      k = K3;
    }

    std::uint32_t temp = rotl(a, 5) + f + e + k + w[i];
    e = d;
    d = c;
    c = rotl(b, 30);
    b = a;
    a = temp;
  }

  // Add the working vars back into state
  state_[0] += a;
  state_[1] += b;
  state_[2] += c;
  state_[3] += d;
  state_[4] += e;
}
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
