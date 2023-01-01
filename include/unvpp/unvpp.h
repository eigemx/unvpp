#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace unv {

struct UnitsSystem {
    std::size_t code {0};
    double length_scale {1.};
    std::string repr {"Unknown"};
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
    Element(std::vector<std::size_t>&& v, ElementType type)
        : _vertices_ids(std::move(v)), _type(type) {}

    auto inline vertices_ids() -> std::vector<std::size_t>& { return _vertices_ids; }
    auto inline type() -> ElementType { return _type; }

private:
    std::vector<std::size_t> _vertices_ids;
    ElementType _type;
};

enum class GroupType {
    Point,
    Cell,
};

struct Group {
    Group(std::string&& name, GroupType type, std::vector<std::size_t>&& elements_ids)
        : _name(std::move(name)), _type(type), _elements_ids(std::move(elements_ids)) {}

    auto inline name() -> std::string& { return _name; }
    auto inline type() -> GroupType { return _type; }
    auto inline elements_ids() -> std::vector<std::size_t>& { return _elements_ids; }

private:
    std::string _name;
    GroupType _type;
    std::vector<std::size_t> _elements_ids;
};

struct Mesh {
    std::optional<UnitsSystem> units_system;
    std::vector<std::array<double, 3>> vertices;
    std::optional<std::vector<Element>> elements;
    std::optional<std::vector<Group>> groups;
};

auto read(const std::filesystem::path& path) -> Mesh;

} // namespace unv