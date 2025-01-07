# unvpp

[![cmake-linux](https://github.com/eigenemara/unvpp/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/eigenemara/unvpp/actions/workflows/cmake-linux.yml)
[![cmake-windows](https://github.com/eigenemara/unvpp/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/eigenemara/unvpp/actions/workflows/cmake-windows.yml)

A C++17 library for reading Ideas-UNV mesh format.

unvpp is designed as a parser for input UNV meshes for finite volume analysis. unvpp reads the following tags in `.unv` mesh files:

- System of units tag 164
- Nodes tag 2411 (vertices)
- Elements tag 2412 (lines, quads, triangles, hexagons, tetrahedrons and wedges)
- Groups tag 2452, 2467 and 757 (for boundary patches and cell zones)

Other tags are ignored.

# Get Started

## Using as a CMake dependency
Having CMake 3.11 or later, you can use FetchContent to add unvpp as a dependency to your project:

```cmake
include(FetchContent)

FetchContent_Declare(
    unvpp
    GIT_REPOSITORY https://github.com/eigemx/unvpp.git
    GIT_TAG main
)

FetchContent_MakeAvailable(unvpp)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE unvpp)

```

## Build `unv-report` tool:

Clone the repository:

```sh
git clone https://github.com/eigemx/unvpp.git
```

```sh
cd unvpp && mkdir build && cd build && cmake .. && make
```

This will compile unvpp library and unv-report tool in `build\bin` directory. unv-report tool is a simple tool for printing mesh information.


## Tutorial
```cpp
#include <unvpp/unvpp.h>
#include <iostream>

auto main() -> int {
    unvpp::Mesh mesh = unvpp::read("./my_mesh.unv");

    if (mesh.unit_system().has_value()) {
        std::cout << "Units system: " 
                  << mesh.unit_system().value().to_string()
                  << std::endl;
    }

    std::cout << "Vertices count = " 
              << mesh.vertices().size() 
              << std::endl;

    if (mesh.elements().has_value()) {
        std::cout << "Elements count = " 
                  << mesh.elements().value().size()
                  << std::endl;
    }

    if (mesh.groups().has_value()) {
        for (const auto& group : mesh.groups().value()) {
            std::cout << "Group name: " 
                      << group.name() 
                      << " - elements count = " 
                      << group.elements_ids().size()
                      << std::endl;
        }
    }

    return 0;
}
```

unvpp is designed to have a minimal interface, you can understand more about the various types included in `unvpp::Mesh` class by simply inspecting `<unvpp/unvpp.h>` file!

## Issues
unvpp is under active development, please feel free to open an issue for any bugs or wrong behaviour
