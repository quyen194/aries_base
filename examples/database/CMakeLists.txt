# CMakeLists.txt for examples/database

# Check if database library is enabled
if(NOT ARIES_DB_ENABLE)
    return()
endif()

# Check if database library is enabled
if(NOT HAVE_ARIES_DB_ENABLE)
    message(STATUS "Database examples skipped (no database libraries found)")
    return()
endif()

# Include MSSQL examples if MSSQL support is enabled
if(HAVE_ARIES_DB_ENABLE_MSSQL)
    add_subdirectory(mssql)
endif()

# Include MySQL examples if MySQL support is enabled
if(HAVE_ARIES_DB_ENABLE_MYSQL)
    add_subdirectory(mysql)
endif()

# Include PostgreSQL examples if PostgreSQL support is enabled
if(HAVE_ARIES_DB_ENABLE_POSTGRES)
    add_subdirectory(postgres)
endif()

# Include SQLite examples if SQLite support is enabled
if(HAVE_ARIES_DB_ENABLE_SQLITE)
    add_subdirectory(sqlite)
endif()
