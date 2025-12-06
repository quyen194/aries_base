# FindODBC.cmake - Find ODBC library
#
# This module defines:
#  ODBC_FOUND - System has ODBC
#  ODBC_INCLUDE_DIRS - ODBC include directories
#  ODBC_LIBRARIES - ODBC libraries

if(WIN32)
    # On Windows, ODBC is usually available through system libraries
    set(ODBC_INCLUDE_DIR "")
    set(ODBC_LIBRARY odbc32)
    set(ODBC_FOUND TRUE)
else()
    find_path(ODBC_INCLUDE_DIR
        NAMES sql.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            /usr/include/x86_64-linux-gnu
    )

    find_library(ODBC_LIBRARY
        NAMES odbc odbcinst iodbc
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
            /usr/lib/x86_64-linux-gnu
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ODBC
        REQUIRED_VARS ODBC_LIBRARY ODBC_INCLUDE_DIR
    )
endif()

if(ODBC_FOUND)
    set(ODBC_LIBRARIES ${ODBC_LIBRARY})
    if(ODBC_INCLUDE_DIR)
        set(ODBC_INCLUDE_DIRS ${ODBC_INCLUDE_DIR})
    else()
        set(ODBC_INCLUDE_DIRS "")
    endif()

    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED)
        if(WIN32)
            set_target_properties(ODBC::ODBC PROPERTIES
                IMPORTED_LOCATION "${ODBC_LIBRARY}"
            )
        else()
            set_target_properties(ODBC::ODBC PROPERTIES
                IMPORTED_LOCATION "${ODBC_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
            )
        endif()
    endif()
endif()

if(NOT WIN32)
    mark_as_advanced(ODBC_INCLUDE_DIR ODBC_LIBRARY)
endif()
