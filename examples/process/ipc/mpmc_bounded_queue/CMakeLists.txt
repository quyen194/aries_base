# CMakeLists.txt for examples/process/ipc/mpmc_bounded_queue

set(NAME "mpmc_bounded_queue")
set(NAMESPACE "process_ipc")
set(NAMESPACE_DIR "process/ipc")
set(NAMESPACE_ALIAS "process::ipc")
set(TYPE "example")
set(PROJECT_NAME aries_base_${TYPE}_${NAMESPACE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAMESPACE_ALIAS}::${NAME})
set(ALIAS_GROUP ${NAMESPACE_ALIAS}::${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAMESPACE_DIR}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# Collect example sources (place your example .cpp files here, e.g. main.cpp, demo.cpp)
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT EXAMPLE_SOURCES)
    message(WARNING "No example sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
    # Create individual example executables (one per source)
    aries_base_add_examples()
endif()
