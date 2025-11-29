# CMakeList.txt : CMake project for AriesBase, include source and define

cmake_minimum_required (VERSION 3.20)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake")

# Set default options
aries_base_set_option(ARIES_BASE_BUILD_EXAMPLES "Enable build examples" ON)
aries_base_set_option(ARIES_BASE_BUILD_TESTS "Enable build tests" ON)

# Print summary of configuration
aries_base_print_summary()

# Add main library
add_subdirectory(aries_base)

# Build examples if enabled
if(ARIES_BASE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build tests if enabled
if(ARIES_BASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
