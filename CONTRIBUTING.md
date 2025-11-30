# Contributing to AriesBase

Thanks for your interest in contributing! This document outlines how to propose changes and our development conventions.

## Ways to Contribute
- Report bugs and request features via Issues
- Submit pull requests for bug fixes, tests, docs, or new modules
- Improve examples and developer experience

## Development Setup
- CMake >= 3.20
- C++17 compiler (MSVC/Clang/GCC)
- Optional: Ninja, clang-format

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```
Windows (PowerShell):
```pwsh
cmake -S . -B out/build/x64-Debug --preset x64-Debug
cmake --build out/build/x64-Debug -j
```

### Tests
```bash
cmake -S . -B build -DARIES_BASE_BUILD_TESTS=ON
cmake --build build --target aries_base_tests
ctest --test-dir build --output-on-failure
```

## Coding Guidelines
- **Standard**: C++17
- **Style**: Prefer consistent naming and minimal headers; follow existing patterns in `aries_base/`
- **Safety**: Favor RAII, avoid raw ownership unless necessary
- **Concurrency**: Be mindful of races; document memory ordering assumptions
- **Logging**: Use `spdlog` where needed; keep logs minimal in hot paths

## CMake Conventions
- Use `FetchContent` for third-party deps when not vendored
- Provide options:
  - `ARIES_BASE_BUILD_EXAMPLES` (default ON)
  - `ARIES_BASE_BUILD_TESTS` (default ON)
- Keep targets private unless intended for public consumption

## Branching & Commits
- Branch from `dev`
- Use concise commit messages; group related changes
- Include tests for bug fixes and new features

## Pull Request Checklist
- Code builds on Windows/Linux/macOS
- Tests added/updated and passing locally
- Documentation updated (`README.md`, `TEST.md`, examples if relevant)
- CI green (if configured)

## Issue Reporting
- Provide environment details (OS, compiler, CMake version)
- Steps to reproduce, expected vs actual behavior
- Logs or minimal repro snippets when possible

## License and Dependencies
- Respect third-party licenses (e.g., `spdlog` MIT)
- Do not add new dependencies without discussion

## Contact
Open an Issue for questions or to propose larger changes.
