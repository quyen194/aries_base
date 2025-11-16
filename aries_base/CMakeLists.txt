# CMakeList.txt : CMake project for AriesBase, include source and define

cmake_minimum_required (VERSION 3.20)

project("aries_base" LANGUAGES C CXX)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/**/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/**/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/**/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/**/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/**/*.hh"
    "${CMAKE_CURRENT_LIST_DIR}/**/*.cc"
)

# Create the library target
add_library(${PROJECT_NAME} STATIC
    ${SOURCE_FILES}
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
