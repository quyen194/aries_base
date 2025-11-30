# CMakeList.txt : CMake project for AriesBase, include source and define

cmake_minimum_required (VERSION 3.20)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("aries_base" LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake")

# Set default options
aries_base_set_option(ARIES_BASE_BUILD_EXAMPLES "Enable build examples" ON)
aries_base_set_option(ARIES_BASE_BUILD_TESTS "Enable build tests" ON)

# Print summary of configuration
aries_base_print_summary()

# FetchContent module for third-party dependencies
include(FetchContent)

# Add spdlog (Logging library)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
    FetchContent_Declare(
        spdlog
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# Add main library
add_subdirectory(aries_base)

# Build examples if enabled
if(ARIES_BASE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build tests if enabled
if(ARIES_BASE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ARIES_BASE_BUILD_EXAMPLES AND ARIES_BASE_BUILD_TESTS)
    add_custom_target(aries_base_all ALL DEPENDS
        aries_base_all_examples
        aries_base_all_tests
    )
endif()
