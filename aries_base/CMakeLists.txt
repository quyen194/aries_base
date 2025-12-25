# CMakeList.txt : CMake project for aries_base, include source and define

# Set project name and language
project("aries_base" LANGUAGES C CXX)

# Create the library target
add_library(${PROJECT_NAME} STATIC)

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# Collect all header files
file(GLOB_RECURSE HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Add source and header files to the library target
target_sources(${PROJECT_NAME} PRIVATE
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Link pthreads if needed (required for std::thread)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

# Link realtime library if available (for shm_open/mmap on some systems)
if(NOT WIN32 AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC rt)
endif()

# Link spdlog
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
