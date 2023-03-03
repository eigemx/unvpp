cmake_minimum_required(VERSION 3.10)

project (
  Unvpp VERSION 1.0.0 
  DESCRIPTION "Ideas-UNV Mesh Parsing Library"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# check if unvpp is compiled as the main project
set(UNVPP_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(UNVPP_MAIN_PROJECT ON)
endif()

add_subdirectory(src)

if (${UNVPP_MAIN_PROJECT})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_subdirectory(utilities)
endif()
