#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace unv {

/*
* UNV system of units, code and repr (representation) can be on of the following:
*   Representation              Code
*   -------------------------   ----
*   "Unknown",                // 0
*   "SI: Meter (newton)",     // 1
*   "BG: Foot (pound f)",     // 2
*   "MG: Meter (kilogram f)", // 3
*   "BA: Foot (poundal)",     // 4
*   "MM: mm (milli newton)",  // 5
*   "CM: cm (centi newton)",  // 6
*   "IN: Inch (pound f)",     // 7
*   "GM: mm (kilogram f)",    // 8
*   "US: USER_DEFINED",       // 9
*   "MN: mm (newton)",        // 10
*
*   length_scale is a factor to scale length units (vertices)
*/
struct UnitsSystem {
    std::size_t code {0};
    double length_scale {1.};
    std::string repr {"Unknown"};
};


/* Type of UNV elements */
enum class ElementType {
    Line,
    Triangle,
    Quad,
    Tetra,
    Wedge,
    Hex,
};

/* UNV element data */
struct Element {
    /**
     * @brief Construct a new Element object
     * 
     * @param v a vector of vertices ids defining the element
     * @param type type of the element
     */
    Element(std::vector<std::size_t>&& v, ElementType type)
        : _vertices_ids(std::move(v)), _type(type) {}

    auto inline vertices_ids() -> std::vector<std::size_t>& { return _vertices_ids; }
    [[nodiscard]] auto inline vertices_ids() const -> const std::vector<std::size_t>& {
        return _vertices_ids;
    }
    auto inline type() -> ElementType { return _type; }

private:
    std::vector<std::size_t> _vertices_ids;
    ElementType _type;
};

/* 
* UNV group type, can be either Vertex or Element.
* This is used when parsing UNV mesh groups (or boundary patches and cell zones)
* each group can be either a vertex group or an element (line, face, cell) group.
*/
enum class GroupType {
    Vertex,
    Element,
};

/* UNV group data */
struct Group {
    /**
     * @brief Construct a new Group object
     * 
     * @param name name of the group
     * @param type type of the group
     * @param elements_ids a vector of elements ids defining the group
     */
    Group(std::string&& name, GroupType type, std::vector<std::size_t>&& elements_ids)
        : _name(std::move(name)), _type(type), _elements_ids(std::move(elements_ids)) {}

    auto inline name() -> std::string& { return _name; }
    auto inline type() -> GroupType { return _type; }
    auto inline elements_ids() -> std::vector<std::size_t>& { return _elements_ids; }
    [[nodiscard]] auto inline elements_ids() const -> const std::vector<std::size_t>& {
        return _elements_ids;
    }

private:
    std::string _name;
    GroupType _type;
    std::vector<std::size_t> _elements_ids;
};

/* UNV mesh data */
struct Mesh {
    /**
     * @brief UNV Mesh object
     * 
     * @param units_system an optional units system of the mesh
     * @param vertices a vector of vertices coordinates
     * @param elements an optional vector of elements (if any)
     * @param groups an optional vector of groups (if any)
     */
    std::optional<UnitsSystem> units_system;
    std::vector<std::array<double, 3>> vertices;
    std::optional<std::vector<Element>> elements;
    std::optional<std::vector<Group>> groups;
};

/**
 * @brief Read UNV mesh from file
 * 
 * @param path path to the UNV file
 * @return Mesh 
 */
auto read(const std::filesystem::path& path) -> Mesh;

} // namespace unv