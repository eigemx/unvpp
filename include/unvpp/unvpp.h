#pragma once

#include <optional>
#include <string>
#include <vector>

namespace unv {
struct UnitsSystem {
    std::size_t code{0};
    double lengthScale{1.};
    std::string repr{"Unknown"};
};

struct Vertex {
    double x;
    double y;
    double z;
};

enum class ElementType {
    Line,
    Triangle,
    Quad,
    Tetra,
    Wedge,
    Hex,
};

struct Element {
    std::vector<std::size_t> vIndices; 
    ElementType type;
};

enum class GroupType {
    Point,
    Cell,
};

struct Group {
    std::string name;
    GroupType type;
    std::vector<std::size_t> eIndices;
};

struct Mesh {
    std::optional<UnitsSystem> unitsSystem;
    std::vector<Vertex> vertices;
    std::optional<std::vector<Element>> elements;
    std::optional<std::vector<Group>> groups;
};

Mesh read(std::ifstream &stream);
} // namespace unv