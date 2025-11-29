cmake_minimum_required(VERSION 3.10)

set(NAME "event")
set(NAMESPACE "process")
set(NAMESPACE_DIR "process")
set(NAMESPACE_ALIAS "process")
set(TYPE "test")
set(PROJECT_NAME aries_base_${TYPE}_${NAMESPACE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAMESPACE_ALIAS}::${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAMESPACE_DIR}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect test sources (place your test .cpp files here, e.g. main.cpp, sender.cpp, receiver.cpp)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT TEST_SOURCES)
    message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Create individual test executables (one per source) so CTest can run them
set(TEST_TARGETS)
foreach(src ${TEST_SOURCES})
    get_filename_component(test_name ${src} NAME_WE)

    set(target "${PROJECT_NAME}${test_name}")
    set(target_alias "${ALIAS_NAME}::${test_name}")

    add_executable(${target} ${src})
    add_executable(${target_alias} ALIAS ${target})

    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )

    if(TARGET aries_base)
        target_link_libraries(${target} PRIVATE aries_base)
    endif()

    target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${BIN_DIR}
        OUTPUT_NAME ${test_name}
    )

    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)

    message(STATUS "  Build ${TYPE} - ${NAMESPACE_ALIAS}::${NAME}::${test_name}")

    list(APPEND TEST_TARGETS ${target})
    # add this target to a global collection of all test targets
    set_property(GLOBAL APPEND PROPERTY ALL_TEST_TARGETS ${target})
endforeach()

add_custom_target(${PROJECT_NAME}_all ALL DEPENDS ${TEST_TARGETS})
