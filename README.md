# unvpp

[![cmake-linux](https://github.com/eigenemara/unvpp/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/eigenemara/unvpp/actions/workflows/cmake-linux.yml)
[![cmake-windows](https://github.com/eigenemara/unvpp/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/eigenemara/unvpp/actions/workflows/cmake-windows.yml)

A C++17 library for reading Ideas-UNV mesh format

## Usage
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
    std::cout << "Units system: " << mesh.unitsSystem.value_or(unv::UnitsSystem()).repr
            << std::endl;

    // print count of mesh vertices
    std::cout << "Vertices count = " << mesh.vertices.size() << std::endl;

    // print count of mesh elements (cells)
    std::cout << "Elements count = " << mesh.elements.value_or(std::vector<unv::Element>()).size()
            << std::endl;

    // print group names and element count (like bounday patches or cell zones)
    for (auto& group : mesh.groups.value_or(std::vector<unv::Group>())) {
        std::cout << "Group name: " << group.name << " - elements count = " << group.eIndices.size()
                << std::endl;
    }
}
```

unvpp is designed to have a minimal interface, you can understand more about the various types included in `unv::Mesh` struct by simply inspecting `<unvpp/unvpp.h>` file!
