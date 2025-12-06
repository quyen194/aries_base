# Database Tests

Run tests with CTest:

```bash
ctest -V -R database
```

Or run individual test executables:

```bash
./aries_base_test_database_basic
./aries_base_test_database_prepared_statement
./aries_base_test_database_transaction
./aries_base_test_database_error_handling
./aries_base_test_database_connection_pooling
```

## Test Coverage

- **basic** - Connection and basic queries
- **prepared_statement** - Parameter binding and statement reuse
- **transaction** - ACID transaction semantics
- **error_handling** - Error scenarios and edge cases
- **connection_pooling** - Resource management and concurrency
