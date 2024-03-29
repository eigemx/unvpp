/*
MIT License

Copyright (c) 2022 Mohamed Emara <mae.emara@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace unvpp {

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
    Element(std::vector<std::size_t> vertices_ids, ElementType type) noexcept
        : _vertices_ids(std::move(vertices_ids)), _type(type) {}

    auto inline vertices_ids() noexcept -> std::vector<std::size_t>& { return _vertices_ids; }
    auto inline vertices_ids() const noexcept -> const std::vector<std::size_t>& {
        return _vertices_ids;
    }
    auto inline type() const noexcept -> ElementType { return _type; }

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
    Group(std::string name, GroupType type, std::vector<std::size_t> elements_ids)
        : _name(std::move(name)), _type(type), _elements_ids(std::move(elements_ids)) {}

    auto inline name() const -> std::string { return _name; }
    auto inline type() const -> GroupType { return _type; }

    auto inline elements_ids() -> std::vector<std::size_t>& { return _elements_ids; }
    auto inline elements_ids() const -> const std::vector<std::size_t>& { return _elements_ids; }

    auto inline unique_element_types() const -> const std::unordered_set<ElementType>& {
        return _unique_element_types;
    }
    void inline add_element_type(ElementType type) { _unique_element_types.insert(type); }


  private:
    std::string _name;
    GroupType _type;
    std::vector<std::size_t> _elements_ids;
    std::unordered_set<ElementType> _unique_element_types;
};

/* UNV mesh data */
class Mesh {
    /**
     * @brief UNV mesh object
     * 
     * @param units_system an optional units system of the mesh
     * @param vertices a vector of vertices coordinates
     * @param elements an optional vector of elements (if any)
     * @param groups an optional vector of groups (if any)
     */
  public:
    Mesh(std::vector<std::array<double, 3>> vertices,
         std::optional<std::vector<Element>> elements,
         std::optional<std::vector<Group>> groups,
         std::optional<UnitsSystem> unit_system)
        : _vertices(std::move(vertices)),
          _elements(std::move(elements)),
          _groups(std::move(groups)),
          _unit_system(std::move(unit_system)) {}

    auto vertices() const -> const std::vector<std::array<double, 3>>& { return _vertices; }
    auto elements() const -> const std::optional<std::vector<Element>>& { return _elements; }
    auto groups() const -> const std::optional<std::vector<Group>>& { return _groups; }
    auto unit_system() const -> const std::optional<UnitsSystem>& { return _unit_system; }

  private:
    std::vector<std::array<double, 3>> _vertices;
    std::optional<std::vector<Element>> _elements {std::nullopt};
    std::optional<std::vector<Group>> _groups {std::nullopt};
    std::optional<UnitsSystem> _unit_system {std::nullopt};
};


/**
 * @brief Read UNV mesh from file
 * 
 * @param path path to the UNV file
 * @return Mesh 
 */
auto read(const std::filesystem::path& path) -> Mesh;

} // namespace unvpp