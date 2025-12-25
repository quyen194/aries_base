# CMakeLists.txt for tests/event

set(NAME "event")
set(NAMESPACE "process")
set(NAMESPACE_DIR "process")
set(NAMESPACE_ALIAS "process")
set(TYPE "test")
set(PROJECT_NAME aries_base_${TYPE}_${NAMESPACE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAMESPACE_ALIAS}::${NAME})
set(ALIAS_GROUP ${NAMESPACE_ALIAS}::${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAMESPACE_DIR}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# Collect test sources (place your test .cpp files here, e.g. main.cpp, sender.cpp, receiver.cpp)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if(NOT TEST_SOURCES)
    message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
    # Create individual test executables (one per source) so CTest can run them
    aries_base_add_tests()
endif()
