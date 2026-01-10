/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/09 16:29
  filename:  aries_base/encryption/hash/sha384/sha384.hpp

  purpose:   Implementation of SHA-384 hash function
*********************************************************************/

// -----------------------------------------------------------------------------
#include <cstring>
#include <cstdint>
#include <string>
#include <cstdio>  // for snprintf

#include "aries_base/encryption/hash/sha384/sha384.hpp"
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
// SHA-512 round constants (shared with SHA-384)
constexpr std::uint64_t k[80] = {
  0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
  0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
  0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
  0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
  0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
  0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
  0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
  0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
  0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
  0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
  0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
  0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
  0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
  0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
  0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
  0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
  0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
  0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
  0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
  0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
  0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
  0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
  0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
  0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
  0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
  0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
  0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
  0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
  0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
  0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
  0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
  0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
  0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
  0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
  0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
  0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
  0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
  0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
  0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
  0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};
// -----------------------------------------------------------------------------

inline std::uint64_t rotr(std::uint64_t x, std::uint64_t n) {
  return (x >> n) | (x << (64 - n));
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

SHA384::SHA384() {
  Reset();
}
// -----------------------------------------------------------------------------

SHA384::~SHA384() = default;
// -----------------------------------------------------------------------------

void SHA384::Reset() {
  // Initialize SHA384 state (different IV from SHA512)
  state_[0] = 0xcbbb9d5dc1059ed8ULL;
  state_[1] = 0x629a292a367cd507ULL;
  state_[2] = 0x9159015a3070dd17ULL;
  state_[3] = 0x152fecd8f70e5939ULL;
  state_[4] = 0x67332667ffc00b31ULL;
  state_[5] = 0x8eb44a8768581511ULL;
  state_[6] = 0xdb0c2e0d64f98fa7ULL;
  state_[7] = 0x47b5481dbefa4fa4ULL;

  count_ = 0;
  buf_len_ = 0;
  digest_.clear();
}
// -----------------------------------------------------------------------------

void SHA384::Update(const void* data, size_t len) {
  const std::uint8_t* input = static_cast<const std::uint8_t*>(data);
  size_t index = buf_len_;

  count_ += len;

  if (index > 0) {
    size_t to_copy = (len < (128 - index)) ? len : (128 - index);
    std::memcpy(buffer_ + index, input, to_copy);
    input += to_copy;
    len -= to_copy;
    index += to_copy;

    if (index == 128) {
      ProcessBlock(buffer_);
      index = 0;
    }
  }

  while (len >= 128) {
    ProcessBlock(input);
    input += 128;
    len -= 128;
  }

  if (len > 0) {
    std::memcpy(buffer_, input, len);
    index = len;
  }

  buf_len_ = index;
}
// -----------------------------------------------------------------------------

std::string SHA384::Final() {
  if (!digest_.empty()) {
    return digest_;
  }

  std::uint64_t bit_len = count_ * 8;
  size_t index = buf_len_;

  buffer_[index++] = 0x80;

  if (index > 112) {
    std::memset(buffer_ + index, 0, 128 - index);
    ProcessBlock(buffer_);
    index = 0;
  }

  std::memset(buffer_ + index, 0, 112 - index);

  std::uint64_t high_bits = 0;
  std::uint64_t low_bits  = bit_len;
  for (int i = 0; i < 8; ++i) {
    buffer_[112 + i] = static_cast<std::uint8_t>(high_bits >> (56 - 8 * i));
    buffer_[120 + i] = static_cast<std::uint8_t>(low_bits  >> (56 - 8 * i));
  }

  ProcessBlock(buffer_);

  // SHA384 outputs first 48 bytes (6 words)
  digest_.reserve(96);
  for (int i = 0; i < 6; ++i) {
    for (int j = 7; j >= 0; --j) {
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

size_t SHA384::DigestSize() const {
  // SHA384 produces a 48-byte digest
  return 48;
}
// -----------------------------------------------------------------------------

void SHA384::ProcessBlock(const std::uint8_t* block) {
  std::uint64_t w[80];

  for (int i = 0; i < 16; ++i) {
    w[i] = (static_cast<std::uint64_t>(block[i * 8 + 0]) << 56) |
           (static_cast<std::uint64_t>(block[i * 8 + 1]) << 48) |
           (static_cast<std::uint64_t>(block[i * 8 + 2]) << 40) |
           (static_cast<std::uint64_t>(block[i * 8 + 3]) << 32) |
           (static_cast<std::uint64_t>(block[i * 8 + 4]) << 24) |
           (static_cast<std::uint64_t>(block[i * 8 + 5]) << 16) |
           (static_cast<std::uint64_t>(block[i * 8 + 6]) << 8)  |
           (static_cast<std::uint64_t>(block[i * 8 + 7]));
  }

  for (int i = 16; i < 80; ++i) {
    std::uint64_t s0 = rotr(w[i - 15], 1) ^ rotr(w[i - 15], 8) ^ (w[i - 15] >> 7);
    std::uint64_t s1 = rotr(w[i - 2], 19) ^ rotr(w[i - 2], 61) ^ (w[i - 2] >> 6);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }

  std::uint64_t a = state_[0], b = state_[1], c = state_[2], d = state_[3];
  std::uint64_t e = state_[4], f = state_[5], g = state_[6], h = state_[7];

  for (int i = 0; i < 80; ++i) {
    std::uint64_t S1 = rotr(e, 14) ^ rotr(e, 18) ^ rotr(e, 41);
    std::uint64_t ch = (e & f) ^ (~e & g);
    std::uint64_t temp1 = h + S1 + ch + k[i] + w[i];
    std::uint64_t S0 = rotr(a, 28) ^ rotr(a, 34) ^ rotr(a, 39);
    std::uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
    std::uint64_t temp2 = S0 + maj;

    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }

  state_[0] += a;
  state_[1] += b;
  state_[2] += c;
  state_[3] += d;
  state_[4] += e;
  state_[5] += f;
  state_[6] += g;
  state_[7] += h;
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
