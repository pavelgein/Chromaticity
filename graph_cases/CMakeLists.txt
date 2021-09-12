CMAKE_MINIMUM_REQUIRED(VERSION 3.1) # Проверка версии CMake.

PROJECT(graph_cases)
#SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
ENDIF()
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -pthread")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -funroll-loops -Ofast")
SET(CMAKE_CXX_FLAGS_TS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g")

SET(CMAKE_CXX_FLAGS_ADDRESS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
SET(CMAKE_LINKER_FLAGS_ADDRESS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
SET(CMAKE_CXX_FLAGS_MEMORY "${CMAKE_CXX_FLAGS} -g -fsanitize=memory -fno-omit-frame-pointer")

INCLUDE_DIRECTORIES(lib)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(invariant_explorer)
ADD_SUBDIRECTORY(acyclic_orientations_calculator)
