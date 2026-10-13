/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/10/13 10:42
  filename:  aries_base/encryption/random/random.cpp

  purpose:   Cryptographically secure random bytes from the OS
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#include <climits>
#elif defined(__APPLE__) || defined(__EMSCRIPTEN__)
// unistd.h first: Emscripten's compat <sys/random.h> needs size_t declared.
#include <unistd.h>
#include <sys/random.h>
#elif defined(__ANDROID__) || defined(__linux__)
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#if !defined(__ANDROID__)
#include <sys/random.h>
#endif
#endif

#include "aries_base/encryption/random/random.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace encryption {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace random {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace {
// -----------------------------------------------------------------------------

#if defined(_WIN32)

// BCryptGenRandom takes a 32-bit ULONG length, size_t is 64-bit on x64.
bool OsRandomBytes(std::uint8_t* out, size_t len) {
  while (len > 0) {
    ULONG chunk = static_cast<ULONG>(len < ULONG_MAX ? len : ULONG_MAX);
    NTSTATUS status = BCryptGenRandom(
        nullptr, out, chunk, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status)) {
      return false;
    }
    out += chunk;
    len -= chunk;
  }
  return true;
}

#elif defined(__APPLE__) || defined(__EMSCRIPTEN__)

// getentropy() fails with EIO for requests larger than 256 bytes.
constexpr size_t kGetEntropyMaxChunk = 256;

bool OsRandomBytes(std::uint8_t* out, size_t len) {
  while (len > 0) {
    size_t chunk = len < kGetEntropyMaxChunk ? len : kGetEntropyMaxChunk;
    if (getentropy(out, chunk) != 0) {
      return false;
    }
    out += chunk;
    len -= chunk;
  }
  return true;
}

#elif defined(__ANDROID__) || defined(__linux__)

// Reads exactly `len` bytes from /dev/urandom, retrying short reads and EINTR.
bool ReadDevUrandom(std::uint8_t* out, size_t len) {
  int fd;
  do {
    fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
  } while (fd < 0 && errno == EINTR);

  if (fd < 0) {
    return false;
  }

  bool ok = true;
  while (len > 0) {
    ssize_t n = read(fd, out, len);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      ok = false;
      break;
    }
    if (n == 0) {
      // EOF on a random device should never happen.
      ok = false;
      break;
    }
    out += n;
    len -= static_cast<size_t>(n);
  }

  close(fd);
  return ok;
}

#if defined(__ANDROID__)

// Bionic only exposes getrandom() from API 28; the build targets API 21.
bool OsRandomBytes(std::uint8_t* out, size_t len) {
  return ReadDevUrandom(out, len);
}

#else

bool OsRandomBytes(std::uint8_t* out, size_t len) {
  while (len > 0) {
    ssize_t n = getrandom(out, len, 0);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      if (errno == ENOSYS) {
        // Kernel < 3.17 has no getrandom syscall.
        return ReadDevUrandom(out, len);
      }
      return false;
    }
    // getrandom may return fewer bytes than requested (> 256 bytes + signal).
    out += n;
    len -= static_cast<size_t>(n);
  }
  return true;
}

#endif  // __ANDROID__

#else
#error "SecureRandomBytes: unsupported platform"
#endif
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // anonymous namespace
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

bool SecureRandomBytes(void* buf, size_t len) {
  if (len == 0) {
    return true;
  }
  if (!buf) {
    return false;
  }

  return OsRandomBytes(static_cast<std::uint8_t*>(buf), len);
}
// -----------------------------------------------------------------------------

utils::bytes SecureRandomBytes(size_t len) {
  if (len == 0) {
    return utils::bytes();
  }

  utils::bytes out(len);
  if (!SecureRandomBytes(out.data(), out.size())) {
    return utils::bytes();
  }
  return out;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace random
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace encryption
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
