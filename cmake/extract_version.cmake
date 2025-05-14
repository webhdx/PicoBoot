# CMake script to extract version information from Git and configure version.h

find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_DESCRIBE_RESULT
)

set(FW_VER_STRING "0.0.0-unknown")
set(FW_VER_MAJOR 0)
set(FW_VER_MINOR 0)
set(FW_VER_PATCH 0)

if(GIT_DESCRIBE_RESULT EQUAL 0 AND GIT_DESCRIBE_OUTPUT)
    set(FW_VER_STRING ${GIT_DESCRIBE_OUTPUT})
    if(FW_VER_STRING MATCHES "^v([0-9]+)\.([0-9]+)\.([0-9]+)")
        set(FW_VER_MAJOR ${CMAKE_MATCH_1})
        set(FW_VER_MINOR ${CMAKE_MATCH_2})
        set(FW_VER_PATCH ${CMAKE_MATCH_3})
    else()
        message(STATUS "Git version string '${FW_VER_STRING}' does not match vX.Y.Z format for MAJOR/MINOR/PATCH. Using 0.0.0 for these.")
    endif()
else()
    message(WARNING "git describe failed or returned empty. Using default version: ${FW_VER_STRING}, MAJOR: ${FW_VER_MAJOR}, MINOR: ${FW_VER_MINOR}, PATCH: ${FW_VER_PATCH}")
endif()

set(PROJECT_VERSION_STRING "${FW_VER_MAJOR}.${FW_VER_MINOR}.${FW_VER_PATCH}")

configure_file(
    "${CMAKE_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_SOURCE_DIR}/src/version.h"
)

message(STATUS "FW_VER_STRING: ${FW_VER_STRING}")
message(STATUS "FW_VER_MAJOR: ${FW_VER_MAJOR}")
message(STATUS "FW_VER_MINOR: ${FW_VER_MINOR}")
message(STATUS "FW_VER_PATCH: ${FW_VER_PATCH}")
message(STATUS "PROJECT_VERSION_STRING: ${PROJECT_VERSION_STRING}") 