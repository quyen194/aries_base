macro(aries_base_add_to_alloptions _NEWNAME)
    list(APPEND ALLOPTIONS ${_NEWNAME})
endmacro()

macro(aries_base_set_option _NAME _DESC)
    aries_base_add_to_alloptions(${_NAME})
    if(${ARGC} EQUAL 3)
        set(_DEFLT ${ARGV2})
    else()
        set(_DEFLT OFF)
    endif()
    option(${_NAME} ${_DESC} ${_DEFLT})
endmacro()

macro(aries_base_dep_option _NAME _DESC _DEFLT _DEPTEST _FAILDFLT)
    aries_base_add_to_alloptions("${_NAME}")
    cmake_dependent_option("${_NAME}" "${_DESC}" "${_DEFLT}" "${_DEPTEST}" "${_FAILDFLT}")
endmacro()

macro(aries_base_option_string _NAME _DESC _VALUE)
    aries_base_add_to_alloptions(${_NAME})
    set(${_NAME} ${_VALUE} CACHE STRING "${_DESC}")
    set(HAVE_${_NAME} ${_VALUE})
ENDMACRO()

macro(aries_base_message_bool_option _NAME _VALUE)
    set(_PAD "\t")
    if(${ARGC} EQUAL 3)
        set(_PAD ${ARGV2})
    endif()
    if(${_VALUE})
        message(STATUS "  ${_NAME}:${_PAD}ON")
    else()
        message(STATUS "  ${_NAME}:${_PAD}OFF")
    endif()
endmacro()

macro(aries_base_message_tested_option _NAME)
    set(_REQVALUE ${${_NAME}})
    set(_PAD " ")
    if(${ARGC} EQUAL 2)
        set(_PAD ${ARGV1})
    endif()
    string(SUBSTRING "${_NAME}" 0 4 _NAMESTART)
    if(_NAMESTART STREQUAL "SDL_")
        string(SUBSTRING "${_NAME}" 4 -1 _STRIPPEDNAME)
    else()
        set(_STRIPPEDNAME "${_NAME}")
    endif()
    if(NOT HAVE_${_STRIPPEDNAME})
        set(HAVE_${_STRIPPEDNAME} OFF)
    elseif("${HAVE_${_STRIPPEDNAME}}" MATCHES "1|TRUE|YES|Y")
        set(HAVE_${_STRIPPEDNAME} ON)
    endif()
    message(STATUS "  ${_NAME}${_PAD}(Wanted: ${_REQVALUE}): ${HAVE_${_STRIPPEDNAME}}")
endmacro()

function(aries_base_find_stringlength_longest_item inList outLength)
    set(maxLength 0)
    foreach(item IN LISTS ${inList})
        string(LENGTH "${item}" slen)
        if(slen GREATER maxLength)
        set(maxLength ${slen})
        endif()
    endforeach()
    set("${outLength}" ${maxLength} PARENT_SCOPE)
endfunction()

function(aries_base_message_dictlist inList)
    aries_base_find_stringlength_longest_item(${inList} maxLength)
    foreach(name IN LISTS ${inList})
        # Get the padding
        string(LENGTH ${name} nameLength)
        math(EXPR padLength "(${maxLength} + 1) - ${nameLength}")
        string(RANDOM LENGTH ${padLength} ALPHABET " " padding)
        aries_base_message_tested_option(${name} ${padding})
    endforeach()
endfunction()

function(aries_base_print_summary)
    ##### Info output #####
    message(STATUS "")
    message(STATUS "aries_base was configured with the following options:")
    message(STATUS "")
    message(STATUS "Platform: ${CMAKE_SYSTEM}")
    message(STATUS "64-bit:   ${ARCH_64}")
    message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "")
    message(STATUS "Options:")
    list(SORT ALLOPTIONS)
    aries_base_message_dictlist(ALLOPTIONS)
    message("")
endfunction()

macro(aries_base_add_examples)
    message(STATUS "  [GROUP] ${ALIAS_GROUP}")

    set(EXAMPLE_TARGETS)
    foreach(src IN LISTS EXAMPLE_SOURCES)
        get_filename_component(example_name ${src} NAME_WE)
        string(REGEX REPLACE "^_+" "" example_name "${example_name}")

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

        message(STATUS "      [EXAM] ${example_name}")

        list(APPEND EXAMPLE_TARGETS ${target})
        # add this target to a global collection of all example targets
        set_property(GLOBAL APPEND PROPERTY ALL_EXAMPLE_TARGETS ${target})
    endforeach()

    add_custom_target(${PROJECT_NAME}__all ALL DEPENDS ${EXAMPLE_TARGETS})
endmacro()

macro(aries_base_add_tests)
    message(STATUS "  [GROUP] ${ALIAS_GROUP}")

    set(TEST_TARGETS)
    foreach(src ${TEST_SOURCES})
        get_filename_component(test_name ${src} NAME_WE)
        string(REGEX REPLACE "^_+" "" test_name "${test_name}")

        set(target "${PROJECT_NAME}_${test_name}")
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

        add_test(NAME ${ALIAS_NAME}::${test_name} COMMAND $<TARGET_FILE:${target}>)

        message(STATUS "      [TEST] ${test_name}")

        list(APPEND TEST_TARGETS ${target})
        # add this target to a global collection of all test targets
        set_property(GLOBAL APPEND PROPERTY ALL_TEST_TARGETS ${target})
    endforeach()

    add_custom_target(${PROJECT_NAME}_all ALL DEPENDS ${TEST_TARGETS})
endmacro()
