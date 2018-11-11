#add_subdirectory(cli)

if(BUILD_TEST)
    #add_subdirectory(ut)
endif()


project(tools)

#cmake_minimum_required(VERSION 2.8)
#set(CMAKE_CXX_STANDARD 11)

file(GLOB _SRC ./*.cpp
    ./cli/cli_command.cpp
    ./cli/cli_server.cpp
    ./cli/cmd_exe/cmd_center.cpp
)

set(CLIENT_SRC
    ./cli/cli_client.cpp
)
include_directories(
    ./
    ./cli
    ./cli/cmd_exe
    ${CMAKE_CURRENT_BINARY_DIR}/../
)
message(STATUS "${CMAKE_CURRENT_BINARY_DIR}")

add_library(${PROJECT_NAME} SHARED ${_SRC})
target_link_libraries(${PROJECT_NAME} mlog)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_executable(cli_client ${CLIENT_SRC})
target_link_libraries(cli_client pthread readline)
install(TARGETS cli_client
    ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)


