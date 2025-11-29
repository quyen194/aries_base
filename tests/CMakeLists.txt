# Tests CMakeLists.txt

# enable testing for CTest
enable_testing()

message(STATUS "")
message(STATUS "Build all aries_base tests:")

# initialize a global property to collect all test targets
set_property(GLOBAL PROPERTY ALL_TEST_TARGETS "")

# Include all test subdirectories
add_subdirectory(process/thread_pool)

# Retrieve the list of all test targets
get_property(ALL_TEST_TARGETS GLOBAL PROPERTY ALL_TEST_TARGETS)

message(STATUS "")

# Provide a convenience target that builds all tests in this folder.
# This allows users to run: `cmake --build <builddir> --target tests_all`.
set(TARGET_NAME aries_base_all_tests)
add_custom_target(${TARGET_NAME} ALL DEPENDS ${ALL_TEST_TARGETS})
