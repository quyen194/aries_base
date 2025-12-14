# CMakeLists.txt for tests/database

# Check if database library is enabled
if(NOT ARIES_DB_ENABLED)
    message(STATUS "Database tests skipped (no database libraries found)")
    return()
endif()

# Include MySQL tests if MySQL support is enabled
if(ARIES_DB_ENABLE_MYSQL AND ARIES_DB_MYSQL_ENABLED)
    add_subdirectory(mysql)
endif()


# Include SQLite tests if SQLite support is enabled
if(ARIES_DB_ENABLE_SQLITE AND ARIES_DB_SQLITE_ENABLED)
    add_subdirectory(sqlite)
endif()
