cmake_minimum_required(VERSION 3.9)

project (unvpp VERSION 1.0.0 DESCRIPTION "Ideas-UNV Mesh Parsing Library")
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_HOME_DIRECTORY}/include)


add_executable(unv-report
    src/main.cpp
    src/reader.cpp
    src/stream.cpp
    src/unvpp.cpp
)

add_library(unvpp SHARED
    src/reader.cpp
    src/stream.cpp
    src/unvpp.cpp
)

set_target_properties(unvpp PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(unvpp PUBLIC ${CMAKE_HOME_DIRECTORY}/include)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set (CMAKE_CXX_FLAGS "-Wall -O2")
