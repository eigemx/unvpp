#pragma once

#include <optional>
#include <vector>

namespace unv {
struct UnitsSystem {
    std::size_t code{0};
    double lengthScale{1.};
    double forceScale{1.};
    double temperatureScale{1.};
    double temperatureOffset{273.};
    std::string repr;
};

struct Vertex {
    double x;
    double y;
    double z;
};

enum class ElementType {
    Point,
    Line,
    Triangle,
    Quad,
    Tetra,
    Wedge,
    Hex,
};

struct Element {
    std::vector<std::size_t>
        vIndices; // indices of vertices defining the elements
    ElementType type;
};

enum class GroupType {
    Point,
    Cell,
};

struct Group {
    std::string name;
    GroupType type;
    std::vector<std::size_t>
        eIndices; // indices of elements defining the elements
};

struct Mesh {
    std::optional<UnitsSystem> unitsSystem;
    std::vector<Vertex> vertices;
    std::optional<std::vector<Element>> elements;
    std::optional<std::vector<Group>> groups;
};

auto read(std::ifstream &stream) -> Mesh;
} // namespace unv