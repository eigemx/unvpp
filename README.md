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
It's planned to make a header-only version of unvpp in the future to make it easier to use and avoid the need for FetchContent.

## Build `unv-report` tool:

Clone the repository:

```sh
git clone https://github.com/eigemx/unvpp.git
```

```sh
cd unvpp && mkdir build && cd build && cmake .. && make
```

This will compile unvpp library and unv-report tool in `build\bin` directory. unv-report tool is a simple tool for printing mesh information.


### Tutorial
```cpp
#include <unvpp/unvpp.h>
#include <iostream>

auto main() -> int {
    // read mesh, this will return a unv::Mesh struct:
    // struct Mesh {
    //      std::optional<UnitsSystem> unitsSystem;
    //      std::vector<Vertex> vertices;
    //      std::optional<std::vector<Element>> elements;
    //      std::optional<std::vector<Group>> groups;
    // };
    auto mesh = unv::read("./my_mesh.unv");


    // print string representation of the mesh system of units
    // if the mesh does not have a system of units, 
    // print the default "repr" representation string "Unknown".
    // Check definition of UnitsSystem in unvpp/unvpp.h file
    std::cout << "Units system: " 
              << mesh.unitsSystem.value_or(unv::UnitsSystem()).repr
              << std::endl;

    // print count of mesh vertices
    std::cout << "Vertices count = " 
              << mesh.vertices.size() 
              << std::endl;

    // print count of mesh elements (cells)
    std::cout << "Elements count = " 
              << mesh.elements.value_or(std::vector<unv::Element>()).size()
              << std::endl;

    // print group names and element count (like boundary patches or cell zones)
    for (auto& group : mesh.groups.value_or(std::vector<unv::Group>())) {
        std::cout << "Group name: " 
                  << group.name 
                  << " - elements count = " 
                  << group.eIndices.size()
                  << std::endl;
    }

    return 0;
}
```

unvpp is designed to have a minimal interface, you can understand more about the various types included in `unv::Mesh` struct by simply inspecting `<unvpp/unvpp.h>` file!

## Issues
unvpp is under active development, please feel free to open an issue for any bugs or wrong behaviour
