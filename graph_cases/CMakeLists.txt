cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17 -O3")

project(graph_cases)

set(SOURCE_EXE graph_cases.cpp)

set(SOURCE_LIB multipartite_graphs.cpp)

#add_library(mg STATIC ${SOURCE_LIB})

INCLUDE_DIRECTORIES(lib)

ADD_SUBDIRECTORY(lib)

add_executable(graph_cases ${SOURCE_EXE})       # Создает исполняемый файл с именем main

target_link_libraries(graph_cases mg)
set_target_properties(graph_cases PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

