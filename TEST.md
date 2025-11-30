# Testing AriesBase

This guide explains how to configure, build, and run tests for AriesBase across Windows, Linux, and macOS.

## Prerequisites
- CMake >= 3.20
- C++17-capable compiler
- Optional: `ctest` (bundled with CMake)

## Configure

Windows (PowerShell):
```pwsh
cmake -S . -B out/build/x64-Debug -DARIES_BASE_BUILD_TESTS=ON --preset x64-Debug
```
If you don't use presets:
```pwsh
cmake -S . -B out/build/x64-Debug -DARIES_BASE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
```

Linux/macOS:
```bash
cmake -S . -B build -DARIES_BASE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
```

## Build

Windows:
```pwsh
cmake --build out/build/x64-Debug --target aries_base_tests -j
```
Linux/macOS:
```bash
cmake --build build --target aries_base_tests -j
```

## Run
Use ctest from the build directory.

Windows:
```pwsh
ctest --test-dir out/build/x64-Debug -C Debug --output-on-failure
```
Linux/macOS:
```bash
ctest --test-dir build --output-on-failure
```

## Test Layout
- Tests are under `tests/` and mirror library modules
  - `process/event/` covers event states, waits, timeouts
  - `thread_pool/` covers posting, ordering, cancellation, capacity
  - `ipc/mpmc_bounded_queue/` covers concurrency and throughput

## Common Options
- `-DARIES_BASE_BUILD_TESTS=ON` to enable tests
- `-DARIES_BASE_BUILD_EXAMPLES=ON` to build examples alongside tests
- Generators:
  - MSVC: `-G "Visual Studio 17 2022" -A x64`
  - Ninja: `-G Ninja`

## Troubleshooting
- spdlog fetch vs vendored
  - If `third_party/spdlog` exists, it is used.
  - Otherwise `FetchContent` pulls upstream/fork into `<build>/_deps/spdlog-src`.
  - Clean stale deps if you see CMake version policy errors:
    - Windows: remove `out\build\x64-Debug\_deps\spdlog-*` and reconfigure.
- Compiler version
  - Ensure your compiler supports C++17 and thread primitives required by the library.
- Parallel build
  - Use `-j` to speed up; reduce if memory constrained.

## Coverage (optional)
If you maintain a coverage setup, configure your compiler with coverage flags and run `ctest`. Coverage collection is not enabled by default in this repo.
