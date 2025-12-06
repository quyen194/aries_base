# CMakeLists.txt for tests/database

set(NAME "database")
set(TYPE "test")
set(PROJECT_NAME aries_base_${TYPE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAME})
set(ALIAS_GROUP ${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# Check if database library is enabled
if(NOT ARIES_DB_ENABLED)
    message(STATUS "Database tests skipped (no database libraries found)")
    return()
endif()

# Collect test sources
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT TEST_SOURCES)
    message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
    # Create individual test executables (one per source)
    aries_base_add_tests()
endif()
