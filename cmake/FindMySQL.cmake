# FindMySQL.cmake - Find MySQL client library
#
# This module defines:
#  MYSQL_FOUND - System has MySQL
#  MYSQL_INCLUDE_DIRS - MySQL include directories
#  MYSQL_LIBRARIES - MySQL libraries
#  MYSQL_VERSION - MySQL version

find_path(MYSQL_INCLUDE_DIR
    NAMES mysql.h
    PATHS
        /usr/include/mysql
        /usr/local/include/mysql
        /opt/local/include/mysql
        /opt/homebrew/include/mysql
        /usr/include/mariadb
        /usr/local/include/mariadb
        "C:/Program Files/MySQL/MySQL Server 8.0/include"
        "C:/Program Files/MySQL/MySQL Server 5.7/include"
)

find_library(MYSQL_LIBRARY
    NAMES mysqlclient libmysql mariadb
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /opt/homebrew/lib
        /usr/lib/x86_64-linux-gnu
        "C:/Program Files/MySQL/MySQL Server 8.0/lib"
        "C:/Program Files/MySQL/MySQL Server 5.7/lib"
)

# Try to get version
if(MYSQL_INCLUDE_DIR AND EXISTS "${MYSQL_INCLUDE_DIR}/mysql_version.h")
    file(STRINGS "${MYSQL_INCLUDE_DIR}/mysql_version.h" MYSQL_VERSION_LINE
         REGEX "^#define[ \t]+MYSQL_SERVER_VERSION[ \t]+\"[0-9.]+\"")
    string(REGEX REPLACE "^#define[ \t]+MYSQL_SERVER_VERSION[ \t]+\"([0-9.]+)\".*" "\\1"
           MYSQL_VERSION "${MYSQL_VERSION_LINE}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL
    REQUIRED_VARS MYSQL_LIBRARY MYSQL_INCLUDE_DIR
    VERSION_VAR MYSQL_VERSION
)

if(MYSQL_FOUND)
    set(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
    set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})

    if(NOT TARGET MySQL::MySQL)
        add_library(MySQL::MySQL UNKNOWN IMPORTED)
        set_target_properties(MySQL::MySQL PROPERTIES
            IMPORTED_LOCATION "${MYSQL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
