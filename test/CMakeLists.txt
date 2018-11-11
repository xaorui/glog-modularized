set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../tools
    
    #${CMAKE_CURRENT_SOURCE_DIR}/../tools/cli
    #${CMAKE_CURRENT_SOURCE_DIR}/../tools/cli/cmd_exe
)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test_cli_srv test_cli_srv.cpp)
target_link_libraries(test_cli_srv tools pthread mlog)
add_dependencies(test_cli_srv tools mlog)
install(TARGETS test_cli_srv
ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)