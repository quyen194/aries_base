/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/09 06:57
  filename:  aries_base/examples/encryption/hash/sha256.cpp

  purpose:   Demonstrate SHA-256 hashing using Aries Base HashFactory
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <string>

#include "aries_base/encryption/hash/hash_factory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::encryption::hash;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  auto hasher = HashFactory::Create(HashType::SHA256);
  if (!hasher) {
    return -1;
  }

  const std::string data = "The quick brown fox jumps over the lazy dog";
  hasher->Update(data.data(), data.size());
  std::string digest = hasher->Final();

  // Print the SHA-256 hash in hexadecimal format
  std::cout << "SHA-256" << "(\"" << data << "\") = " << digest << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
