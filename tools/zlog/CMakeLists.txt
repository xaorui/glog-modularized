project(zlog)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_DEFAULT_SOURCE)


file(GLOB _TOP_INCS "./*.h")
file(GLOB _INCS "./core/*.h")
file(GLOB _SRC "./core/*.c")

#set(_SRC
#    ./*.cpp
#)
include_directories(
    ./
)
add_library(${PROJECT_NAME} STATIC ${_SRC})
target_link_libraries(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
install(FILES ${_INCS} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../include/base/zlog)
install(FILES ${_TOP_INCS} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../include/base/zlog)
if(BUILD_TEST)
    #add_subdirectory(ut)
endif()