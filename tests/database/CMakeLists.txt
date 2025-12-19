# CMakeLists.txt for tests/database

# Check if database library is enabled
if(NOT ARIES_DB_ENABLE)
    return()
endif()

# Check if database library is enabled
if(NOT HAVE_ARIES_DB_ENABLE)
    message(STATUS "Database tests skipped (no database libraries found)")
    return()
endif()

# Include MSSQL tests if MSSQL support is enabled
if(HAVE_ARIES_DB_ENABLE_MSSQL)
    add_subdirectory(mssql)
endif()

# Include MySQL tests if MySQL support is enabled
if(HAVE_ARIES_DB_ENABLE_MYSQL)
    add_subdirectory(mysql)
endif()

# Include PostgreSQL tests if PostgreSQL support is enabled
if(HAVE_ARIES_DB_ENABLE_POSTGRES)
    add_subdirectory(postgres)
endif()

# Include SQLite tests if SQLite support is enabled
if(HAVE_ARIES_DB_ENABLE_SQLITE)
    add_subdirectory(sqlite)
endif()
