# AriesBase

Cross-platform C++ base library providing lightweight concurrency primitives and helpers:

- **Process Events**: multi/single state events, manual/auto reset, wait-all/any with priorities
- **IPC**: bounded MPMC queue for high-throughput inter-thread/process messaging
- **Thread Pool**: task posting, delayed tasks, graceful shutdown, concurrency tuning
- **Containers/Helpers**: utility helpers and minimal container abstractions

This project uses `spdlog` (MIT) for logging.

## Features
- **Portable**: Windows, Linux, macOS (CMake-based build)
- **Modern C++**: C++17, noexcept-friendly APIs, header-first design where sensible
- **Fast**: lock-free/low-contention where appropriate, careful allocation behavior
- **Tested**: rich unit tests and runnable examples

## Getting Started

### Prerequisites
- CMake >= 3.20
- A C++17 compiler
	- Windows: MSVC (Visual Studio 2022) or clang-cl
	- Linux: GCC 10+ or Clang 12+
	- macOS: Apple Clang (Xcode 13+)

### Clone
```
git clone https://github.com/quyen194/aries_base.git
cd aries_base
```

### Configure & Build (CMake Presets)
Use your platform’s preset or pick one from `CMakePresets.json`.

Windows (PowerShell):
```
cmake -S . -B out/build/x64-Debug --preset x64-Debug
cmake --build out/build/x64-Debug -j
```

Linux/macOS:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### Using AriesBase in Your Project
Add as a subdirectory or install:

Subdirectory:
```
add_subdirectory(path/to/aries_base)
target_link_libraries(your_target PRIVATE aries_base)
```

Find package (if you install and export configs):
```
find_package(aries_base CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE aries_base)
```

### Logging (spdlog)
AriesBase depends on `spdlog`. If vendored `third_party/spdlog` exists, it will be used; otherwise CMake will fetch upstream via `FetchContent`. You can also provide your own `spdlog_SOURCE_DIR` at top-level to control the source.

## Examples
Examples live under `examples/` and cover events, thread pools, and IPC.

Build examples:
```
cmake -S . -B build -DARIES_BASE_BUILD_EXAMPLES=ON
cmake --build build --target aries_base_examples -j
```

Run a sample (paths may differ per generator):
```
./build/examples/process/event/_single_state
```

## Tests
See `TEST.md` for detailed guidance. Quick start:
```
cmake -S . -B build -DARIES_BASE_BUILD_TESTS=ON
cmake --build build --target aries_base_tests -j
ctest --test-dir build -C Debug --output-on-failure
```

## Project Structure
```
aries_base/
	container/            # Minimal containers (e.g., linked list)
	definitions/          # Macros and platform abstractions
	helper/               # Process helpers
	process/              # Event primitives (single/multi state)
	thread_pool/          # Thread pool implementation
cmake/                  # CMake helper macros
examples/               # Runnable examples
tests/                  # Unit tests
third_party/spdlog/     # Optional vendored spdlog
```

## Contributing
We welcome issues and PRs. Please read `CONTRIBUTING.md` and follow coding style and testing guidelines.

## Code of Conduct
See `CODE_OF_CONDUCT.md`.

## Security
Please report vulnerabilities responsibly. See `SECURITY.md`.

## License
AriesBase is open source. Dependencies retain their original licenses (e.g., spdlog under MIT).