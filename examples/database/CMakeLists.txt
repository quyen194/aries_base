# CMakeLists.txt for examples/database

# Check if database library is enabled
if(NOT ARIES_DB_ENABLED)
    message(STATUS "Database examples skipped (no database libraries found)")
    return()
endif()

set(NAME "database")
set(TYPE "example")
set(PROJECT_NAME aries_base_${TYPE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAME})
set(ALIAS_GROUP ${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# Collect example sources (place your example .cpp files here, e.g. main.cpp, demo.cpp)
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if(NOT EXAMPLE_SOURCES)
    message(WARNING "No example sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
    # Create individual example executables (one per source)
    aries_base_add_examples()
endif()
