cmake_minimum_required(VERSION 3.10)

include(FetchContent)

project (
  Unvpp VERSION 1.0.0 
  DESCRIPTION "Ideas-UNV Mesh Parsing Library"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set default build type to release if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# set optimization flags for release build for gcc, clang, and msvc
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    endif()
endif()

# check if unvpp is compiled as the main project
set(UNVPP_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(UNVPP_MAIN_PROJECT ON)
endif()


# turn off assertions in release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
endif()


FetchContent_Declare(
  fast_float
  GIT_REPOSITORY https://github.com/lemire/fast_float.git
  GIT_TAG tags/v1.1.2
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(fast_float)

add_subdirectory(src)

if (${UNVPP_MAIN_PROJECT})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    # set output directories
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    add_subdirectory(utilities)
endif()
