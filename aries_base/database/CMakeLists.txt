# Database Library CMakeLists.txt

# Find and configure database libraries
set(DATABASE_SOURCES "")
set(DATABASE_HEADERS "")
set(DATABASE_LIBS "")
set(DATABASE_DEFINITIONS "")

# ============================================================================
# SQLite
# ============================================================================
if(ARIES_DB_ENABLE_SQLITE)
    find_package(SQLite3)
    if(SQLite3_FOUND)
        message(STATUS "SQLite found: ${SQLite3_VERSION}")
        file(GLOB_RECURSE SQLITE_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.c"
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.cc"
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.cpp"
        )
        file(GLOB_RECURSE SQLITE_HEADER_FILES
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.h"
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.hh"
            "${CMAKE_CURRENT_LIST_DIR}/sqlite/*.hpp"
        )
        list(APPEND DATABASE_SOURCES ${SQLITE_SOURCE_FILES})
        list(APPEND DATABASE_HEADERS ${SQLITE_HEADER_FILES})
        list(APPEND DATABASE_LIBS SQLite::SQLite3)
        list(APPEND DATABASE_DEFINITIONS ARIES_DB_SQLITE)
    else()
        message(WARNING "SQLite not found. SQLite support will be disabled.")
    endif()
endif()

# ============================================================================
# MySQL
# ============================================================================
if(ARIES_DB_ENABLE_MYSQL)
    if(WIN32)
        # Windows specific find package for mysql connector cpp
        find_package(unofficial-libmysql)
        if(unofficial-libmysql_FOUND)
            message(STATUS "MySQL found")
            file(GLOB_RECURSE MYSQL_SOURCE_FILES
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.cc"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.cpp"
            )
            file(GLOB_RECURSE MYSQL_HEADER_FILES
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.h"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.hh"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.hpp"
            )
            list(APPEND DATABASE_SOURCES ${MYSQL_SOURCE_FILES})
            list(APPEND DATABASE_HEADERS ${MYSQL_HEADER_FILES})
            list(APPEND DATABASE_LIBS unofficial::libmysql::libmysql)
            list(APPEND DATABASE_DEFINITIONS ARIES_DB_MYSQL)
        else()
            message(WARNING "MySQL not found. MySQL support will be disabled.")
        endif()
    else()
        # Linux/Mac specific find package for mysql
        find_package(MySQL)
        if(MYSQL_FOUND)
            message(STATUS "MySQL found: ${MYSQL_VERSION}")
            file(GLOB_RECURSE MYSQL_SOURCE_FILES
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.cc"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.cpp"
            )
            file(GLOB_RECURSE MYSQL_HEADER_FILES
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.h"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.hh"
                "${CMAKE_CURRENT_LIST_DIR}/mysql/*.hpp"
            )
            list(APPEND DATABASE_SOURCES ${MYSQL_SOURCE_FILES})
            list(APPEND DATABASE_HEADERS ${MYSQL_HEADER_FILES})
            list(APPEND DATABASE_LIBS ${MYSQL_LIBRARIES})
            list(APPEND DATABASE_DEFINITIONS ARIES_DB_MYSQL)
        else()
            message(WARNING "MySQL not found. MySQL support will be disabled.")
        endif()
    endif()
endif()

# ============================================================================
# PostgreSQL
# ============================================================================
if(ARIES_DB_ENABLE_POSTGRES)
    find_package(PostgreSQL)
    if(PostgreSQL_FOUND)
        message(STATUS "PostgreSQL found: ${PostgreSQL_VERSION_STRING}")
        file(GLOB_RECURSE POSTGRES_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.c"
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.cc"
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.cpp"
        )
        file(GLOB_RECURSE POSTGRES_HEADER_FILES
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.h"
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.hh"
            "${CMAKE_CURRENT_LIST_DIR}/postgres/*.hpp"
        )
        list(APPEND DATABASE_SOURCES ${POSTGRES_SOURCE_FILES})
        list(APPEND DATABASE_HEADERS ${POSTGRES_HEADER_FILES})
        list(APPEND DATABASE_LIBS PostgreSQL::PostgreSQL)
        list(APPEND DATABASE_DEFINITIONS ARIES_DB_POSTGRES)
    else()
        message(WARNING "PostgreSQL not found. PostgreSQL support will be disabled.")
    endif()
endif()

# ============================================================================
# MSSQL (ODBC)
# ============================================================================
if(ARIES_DB_ENABLE_MSSQL)
    find_package(ODBC)
    if(ODBC_FOUND)
        message(STATUS "ODBC found")
        file(GLOB_RECURSE MSSQL_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.c"
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.cc"
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.cpp"
        )
        file(GLOB_RECURSE MSSQL_HEADER_FILES
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.h"
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.hh"
            "${CMAKE_CURRENT_LIST_DIR}/mssql/*.hpp"
        )
        list(APPEND DATABASE_SOURCES ${MSSQL_SOURCE_FILES})
        list(APPEND DATABASE_HEADERS ${MSSQL_HEADER_FILES})
        if(WIN32)
            list(APPEND DATABASE_LIBS odbc32 odbccp32)
        else()
            list(APPEND DATABASE_LIBS ODBC::ODBC)
        endif()
        list(APPEND DATABASE_DEFINITIONS ARIES_DB_MSSQL)
    else()
        message(WARNING "ODBC not found. MSSQL support will be disabled.")
    endif()
endif()

# ============================================================================
# Create database library target
# ============================================================================
if(DATABASE_SOURCES)
    # Database source files
    list(APPEND DATABASE_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/db_factory.cpp"
    )

    list(APPEND DATABASE_HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/db_factory.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/interfaces/database.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/interfaces/statement.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/interfaces/result_sets.hpp"
    )

    # Add database sources to aries_base library
    target_sources(${PROJECT_NAME} PRIVATE
        ${DATABASE_SOURCES}
        ${DATABASE_HEADERS}
    )

    # Add database definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${DATABASE_DEFINITIONS})

    # Link database libraries
    if(DATABASE_LIBS)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${DATABASE_LIBS})
    endif()

    # Add include directories
    if(DATABASE_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${DATABASE_INCLUDE_DIRS})
    endif()

    message(STATUS "Database library enabled with: ${DATABASE_DEFINITIONS}")

    # Set global variable for examples
    set(ARIES_DB_ENABLED TRUE CACHE BOOL "Enable Aries DB")
else()
    message(WARNING "No database libraries found. Database functionality will be unavailable.")
    set(ARIES_DB_ENABLED FALSE CACHE BOOL "Disable Aries DB")
endif()
