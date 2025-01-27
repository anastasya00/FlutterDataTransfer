cmake_minimum_required(VERSION 3.22)

project(
    Program1_CPP
    VERSION 1.0
    LANGUAGES CXX
)

add_library(client STATIC
  libclient/buffer.cpp
  libclient/socket.cpp
  libclient/thread.cpp)

target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libclient)

add_executable(project src/main.cpp)

target_include_directories(project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(project PRIVATE client)

target_compile_options(project PRIVATE -g -Wall -Wextra -Wpedantic)
