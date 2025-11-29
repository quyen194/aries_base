cmake_minimum_required(VERSION 3.10)

set(NAME "event")
set(NAMESPACE "process")
set(NAMESPACE_DIR "process")
set(NAMESPACE_ALIAS "process")
set(TYPE "example")
set(PROJECT_NAME aries_base_${TYPE}_${NAMESPACE}_${NAME})
set(ALIAS_NAME aries_base::${TYPE}::${NAMESPACE_ALIAS}::${NAME})
set(BIN_DIR aries_base/${TYPE}/${NAMESPACE_DIR}/${NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect example sources (place your example .cpp files here, e.g. main.cpp, demo.cpp)
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT EXAMPLE_SOURCES)
    message(WARNING "No example sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Create one executable per source file
set(EXAMPLE_TARGETS)
foreach(src IN LISTS EXAMPLE_SOURCES)
    get_filename_component(example_name ${src} NAME_WE)

    set(target ${PROJECT_NAME}_${example_name})
    set(target_alias "${ALIAS_NAME}::${example_name}")

    add_executable(${target} ${src})
    add_executable(${ALIAS_NAME}_${example_name} ALIAS ${target})

    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )

    if(TARGET aries_base)
        target_link_libraries(${target} PRIVATE aries_base)
    endif()

    target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${BIN_DIR}
        OUTPUT_NAME ${example_name}
    )

    install(TARGETS ${target} RUNTIME DESTINATION bin/${BIN_DIR})

    message(STATUS "  Build ${TYPE} - ${NAMESPACE_ALIAS}::${NAME}::${example_name}")

    list(APPEND EXAMPLE_TARGETS ${target})
    # add this target to a global collection of all example targets
    set_property(GLOBAL APPEND PROPERTY ALL_EXAMPLE_TARGETS ${target})
endforeach()

add_custom_target(${PROJECT_NAME}_all ALL DEPENDS ${EXAMPLE_TARGETS})
