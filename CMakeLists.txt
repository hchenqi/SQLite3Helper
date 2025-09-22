cmake_minimum_required(VERSION 3.20)

project(SQLite3Helper CXX)

add_subdirectory(SQLite3Helper)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE SQLite3Helper)
endif()
