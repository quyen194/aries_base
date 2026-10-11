# aries_base/database

Engine-agnostic DB layer, namespace `aries_base::database`. Usage docs: `README.md` in this folder.

## Shape

- `interfaces/` — pure virtual `Database` (Connect/Execute/Prepare/Begin/Commit/Rollback/GetLastInsertId/GetAffectedRows), `Statement` (`Bind*` with **1-based** index, `Execute`, `Query`, `Reset`, `ClearBindings`), `ResultSets` (`Next`, `Get*` by 0-based column index or by name, `IsNull`).
- One folder per backend, each with the same trio: `<engine>_database`, `<engine>_statement`, `<engine>_result`. Sub-namespaces `sqlite`, `postgres`, `mysql`, `mssql`.
- `db_factory.{hpp,cpp}` — `DatabaseFactory::Create(DBType)` + downcasts `ToSQLite/ToPostgres/...` (for Database, Statement and ResultSets). Downcasts **take ownership**: they `release()` the source `unique_ptr` then `dynamic_cast`.

## Compile-time selection

- A backend is compiled only if its CMake option is ON **and** its client lib is found; it then defines `ARIES_DB_SQLITE` / `ARIES_DB_POSTGRES` / `ARIES_DB_MYSQL` / `ARIES_DB_MSSQL` (PUBLIC). Code using a backend must be inside the matching `#ifdef`.
- `aries_base/CMakeLists.txt` excludes `database/` from the main glob; `database/CMakeLists.txt` adds files per backend. A new backend = new folder + new block there + new `DBType` + factory case.

## Backend quirks worth knowing

- SQL is written with `?` placeholders everywhere; the Postgres statement rewrites `?` → `$1, $2…` (skipping quoted strings) before `PQprepare`.
- Connection strings are `key=value;` lists for MySQL/Postgres, a file path or `:memory:` for SQLite, a raw ODBC string for MSSQL (see header comment in `interfaces/database.hpp`).
- Transactions are plain SQL (`BEGIN TRANSACTION` etc.) per backend; there is no RAII transaction guard — callers must Rollback on every error path.
- Tests per backend live in `tests/database/<engine>/` and need a reachable server (except SQLite); settings in `_settings.hpp`.
