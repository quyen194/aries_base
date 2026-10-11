# aries_base

Reusable C++17 base library (static lib `aries_base`, own repo `quyen194/aries_base`, used as a submodule by ElefantBlaster). Mentor mode applies (see workspace `CLAUDE.md` if present): explain/review, don't edit unless asked, answer in Vietnamese.

codebase-memory project: `K-git-ElefantBlaster-libs-aries_base`. Ignore `third_party/` (vendored spdlog) and `out/`.

## Layout → namespace

Include root is the repo root, so includes are `#include "aries_base/<module>/<file>.hpp"`.

| Dir | Namespace | What |
|-----|-----------|------|
| `definitions/macro.hpp` | (global macros) | `HOPE_*`/`EXPECT_*`/`CHECK*`/`DCHECK*` guards, `DISALLOW_COPY_AND_ASSIGN`, `LO_/HI_/MAKE_WORD/DWORD/QWORD`, `PACK_ON/OFF` |
| `process/event/` | `aries_base::process` | `Event`: Win32-style event on mutex+condvar. Single-state (`Set/Reset/Wait`) or multi-state by name/id (`AddName`, `WaitAnyName`, `WaitAll`), bitset storage, manual/auto reset |
| `process/thread_pool/` | `aries_base::process` | `ThreadPool` singleton: static `PostTask` / `PostDelayedTask`, optional `Event*` signalled on task end, idle/max worker counts, `internal/` = workers + pending/delay task queues |
| `process/ipc/mpmc_bounded_queue/` | `...::process::ipc::mpmc_bounded_queue` | `IpcServer` (consumer; subclass and override `OnMessageReceived(id, data, len)`) + `IpcClient::Enqueue(msg_id, data, len)`. In-process or cross-process (shared memory, `/name`). `IpcType` e.g. `kMPSC` |
| `process/utils/` | `...::process::utils` | `SingleProcess` (lock file), `ExitProcessHandle` (run callback on SIGINT/SIGTERM or Windows console Ctrl events), `system` |
| `logger/` | `aries_base::common` | `LoggerManager` singleton over spdlog: per-class console + rotating file loggers, shared file sinks, "aggregate" logger; `LOG_INFO(logger, ...)` macros also write to aggregate |
| `database/` | `aries_base::database` | Unified DB API (see `aries_base/database/CLAUDE.md`) |
| `encryption/hash/` | `aries_base::encryption::hash` | `HashFactory::Create(HashType::SHA256)` → `Update()/Final()`; MD5, SHA1/256/384/512 implemented by hand |
| `container/linked_list.hpp` | | Intrusive Chromium-style `LinkNode<T>`/`LinkedList<T>`, used by thread pool queues |
| `utils/` | `aries_base::utils` | `bytes` (byte vector + `ToBytes`), `file_io`, `times`, `GetDefault(map, key, def)`, `scope_cleanup` |

## Build & test

```bash
cmake -S . -B build -DARIES_BASE_BUILD_TESTS=ON -DARIES_BASE_BUILD_EXAMPLES=ON
cmake --build build --target aries_base_all_tests -j
ctest --test-dir build -C Debug --output-on-failure
```

- Options: `ARIES_BASE_BUILD_EXAMPLES`, `ARIES_BASE_BUILD_TESTS`, `ARIES_DB_ENABLE` + `ARIES_DB_ENABLE_{SQLITE,MYSQL,POSTGRES,MSSQL}`. ElefantBlaster turns examples/tests off and disables MSSQL/MySQL/Postgres (and all DB on Emscripten).
- Sources are collected with `GLOB_RECURSE` — new files are picked up on re-configure, no CMake edit needed (except `database/`, which is per-backend).
- Tests are **plain executables using `assert`** (no gtest), one per `_*.cpp` file under `tests/<module>/`, registered by `aries_base_add_tests()` in `cmake/macros.cmake`. Examples mirror the same layout under `examples/`.
- File naming in tests/examples: leading underscore (`_single.cpp`, `_basic.cpp`).

## Things to keep in mind when reviewing

- Platform split is done with `#if defined(_WIN32)` inside the class (different members per OS) — check both branches when reviewing IPC / SingleProcess.
- `ThreadPool` and `LoggerManager` are process-wide singletons; consumers must `CreateInstance()` before use and `DestroyInstance()` at shutdown.
- Library is consumed via public include dir + `PUBLIC` links (spdlog, Threads, rt, DB libs) — changes to headers ripple to every ElefantBlaster project.
