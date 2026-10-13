/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 15:40
  filename:  aries_base/encryption/password/password_hasher.hpp

  purpose:   Interface for password storage hashers
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_ENCRYPTION_PASSWORD_PASSWORD_HASHER_HPP
#define ARIES_BASE_ENCRYPTION_PASSWORD_PASSWORD_HASHER_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>
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

// A password hasher produces a self-describing "encoded" string (algorithm,
// parameters, salt and hash) meant to be stored as is, e.g. in a DB column.
// Verify() reads the parameters back from that string, so hashes created with
// older parameters keep verifying after the defaults change.
class IPasswordHasher {
 public:
  virtual ~IPasswordHasher() = default;

  // Hashes `password` with a fresh random salt.
  // Returns an empty string on failure (e.g. the OS RNG is unavailable).
  virtual std::string Hash(const std::string& password) = 0;

  // True if `password` matches `encoded`. Malformed `encoded` -> false.
  virtual bool Verify(const std::string& password,
                      const std::string& encoded) = 0;

  // True if `encoded` was not produced with this hasher's current parameters
  // (or is malformed): re-hash the password after a successful Verify().
  virtual bool NeedsRehash(const std::string& encoded) const = 0;
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
#endif  // ARIES_BASE_ENCRYPTION_PASSWORD_PASSWORD_HASHER_HPP
// -----------------------------------------------------------------------------
