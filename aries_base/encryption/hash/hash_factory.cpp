/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/09 06:46
  filename:  aries_base/encryption/hash/hash_factory.cpp

  purpose:   Factory for creating hash implementations
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/encryption/hash/md5/md5.hpp"
#include "aries_base/encryption/hash/sha1/sha1.hpp"
#include "aries_base/encryption/hash/sha256/sha256.hpp"
#include "aries_base/encryption/hash/sha384/sha384.hpp"
#include "aries_base/encryption/hash/sha512/sha512.hpp"
#include "aries_base/encryption/hash/hash_factory.hpp"
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

std::unique_ptr<IHash> HashFactory::Create(HashType type) {
  switch (type) {
    case HashType::MD5:
      return std::make_unique<MD5>();
    case HashType::SHA1:
      return std::make_unique<SHA1>();
    case HashType::SHA384:
      return std::make_unique<SHA384>();
    case HashType::SHA256:
      return std::make_unique<SHA256>();
    case HashType::SHA512:
      return std::make_unique<SHA512>();
    default:
      return nullptr;
  }
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
