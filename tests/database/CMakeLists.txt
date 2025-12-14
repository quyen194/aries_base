# CMakeLists.txt for tests/database

# Check if database library is enabled
if(NOT ARIES_DB_ENABLED)
    message(STATUS "Database tests skipped (no database libraries found)")
    return()
endif()

# Include SQLite tests if SQLite support is enabled
if(ARIES_DB_ENABLE_SQLITE AND ARIES_DB_SQLITE_ENABLED)
    add_subdirectory(sqlite)
endif()
