# Examples CMakeLists.txt

message(STATUS "")
message(STATUS "Build all aries_base examples:")

# initialize a global property to collect all example targets
set_property(GLOBAL PROPERTY ALL_EXAMPLE_TARGETS "")

# Include all example subdirectories
add_subdirectory(database)
add_subdirectory(process/event)
add_subdirectory(process/ipc/mpmc_bounded_queue)
add_subdirectory(process/thread_pool)

# Retrieve the list of all example targets
get_property(ALL_EXAMPLE_TARGETS GLOBAL PROPERTY ALL_EXAMPLE_TARGETS)

message(STATUS "")

# Provide a convenience target that builds all examples in this folder.
# This allows users to run: `cmake --build <builddir> --target examples_all`.
set(TARGET_NAME aries_base_all_examples)
add_custom_target(${TARGET_NAME} ALL DEPENDS ${ALL_EXAMPLE_TARGETS})
