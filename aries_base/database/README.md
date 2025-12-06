# Database Module

Cross-platform database abstraction library with unified API for multiple database engines.

## Supported Databases

- SQLite (native embedded)
- MySQL / MariaDB (libmysqlclient)
- PostgreSQL (libpq)
- Microsoft SQL Server (ODBC)

## Key Features

- **Unified API** - Same code works with any database engine
- **Prepared Statements** - SQL injection protection & performance
- **Transactions** - Full ACID support (begin/commit/rollback)
- **Type Safe** - Strong typing for parameters and results
- **Cross-Platform** - Windows, Linux, macOS

## Quick Example

```cpp
#include "aries_base/database/db_factory.hpp"
using namespace aries_base::database;

// Create connection
auto db = DatabaseFactory::Create(DBType::SQLite);
db->Connect(":memory:");

// Execute query
auto result = db->Execute("SELECT id, name FROM users");
while (result->Next()) {
    std::cout << result->GetInt(0) << " | " << result->GetString(1) << "\n";
}

// Prepared statement
auto stmt = db->Prepare("INSERT INTO users(name) VALUES (?)");
stmt->BindString(1, "John");
stmt->Execute();

// Transaction
db->Begin();
db->Execute("UPDATE users SET active = 1");
db->Commit();
```

## Building

Install dependencies (see ../BUILD.md), then:

```bash
cmake -DARIES_DB_ENABLE_SQLITE=ON \
      -DARIES_DB_ENABLE_POSTGRES=ON \
      -DARIES_DB_ENABLE_MYSQL=ON \
      -DARIES_DB_ENABLE_MSSQL=ON ..
cmake --build .
```

## API Overview

### Database Interface
- `Connect(connectionString)` - Establish database connection
- `Execute(sql)` - Execute SQL query/command
- `Prepare(sql)` - Create prepared statement
- `Begin() / Commit() / Rollback()` - Transaction control

### Statement Interface
- `BindInt() / BindInt64() / BindDouble() / BindString() / BindNull()` - Parameter binding
- `Execute()` - Execute non-query
- `Query()` - Execute with result set

### ResultSets Interface
- `Next()` - Move to next row
- `GetInt() / GetInt64() / GetDouble() / GetString()` - Retrieve values
- `IsNull()` - Check NULL value

## Connection Strings

**SQLite:**
```
:memory:                    # In-memory
database.db                 # File-based
```

**PostgreSQL:**
```
host=localhost;port=5432;user=postgres;password=secret;dbname=mydb
```

**MySQL:**
```
host=localhost;port=3306;user=root;password=secret;database=mydb
```

**MSSQL:**
```
Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=mydb;UID=sa;PWD=pass
```

## Examples

See `../../examples/database/` directory:
- `_basic.cpp` - Simple queries
- `_prepare.cpp` - Prepared statements
- `_transaction.cpp` - Transaction management
- `_advanced.cpp` - Complex real-world scenario
- `_multi_db.cpp` - Database abstraction demonstration

## Testing

Run tests with CTest:

```bash
ctest -V -R database
```

Test files in `../../tests/database/`:
- `_basic.cpp` - Basic operations
- `_prepared_statement.cpp` - Parameter binding
- `_transaction.cpp` - Transaction semantics
- `_error_handling.cpp` - Error scenarios
- `_connection_pooling.cpp` - Resource management

## Dependencies

See `../../BUILD.md` for OS-specific installation instructions.
