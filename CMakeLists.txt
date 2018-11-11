project(project)

cmake_minimum_required(VERSION 2.12)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 11)


option(BUILD_TEST " build test" ON)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set (CMAKE_CXX_FLAGS "--std=c++11 -g -Wall -o3")
else ()
    set (CMAKE_CXX_FLAGS "--std=c++11 -Wall -o3")
endif()
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#set (CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "BUILD TYPE : " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
    FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.")
endif()

set(INSTALL_RPATH ${CMAKE_BINARY_DIR}/install)
message (STATUS "-install destination ${INSTALL_RPATH}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${INSTALL_RPATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INSTALL_RPATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${INSTALL_RPATH}/bin)

add_subdirectory(mlog)
add_subdirectory(tools)

if(BUILD_TEST)
    add_subdirectory(test)
endif()