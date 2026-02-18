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
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace unvpp {

struct UnitsSystem {
  /**
   * @brief An object representing the system of units of the mesh.
   * UNV system of units, code and repr (representation) can be on of the
   * following:
   *
   *   Representation              Code
   *   -------------------------   ----
   *   "Unknown",                  0
   *   "SI: Meter (newton)",       1
   *   "BG: Foot (pound f)",       2
   *   "MG: Meter (kilogram f)",   3
   *   "BA: Foot (poundal)",       4
   *   "MM: mm (milli newton)",    5
   *   "CM: cm (centi newton)",    6
   *   "IN: Inch (pound f)",       7
   *   "GM: mm (kilogram f)",      8
   *   "US: USER_DEFINED",         9
   *   "MN: mm (newton)",          10
   *
   *   @param code The code of the system of units, as given in above table.
   *   @param length_scale The factor to scale length units (vertices).
   */
public:
  UnitsSystem() = default;
  UnitsSystem(std::size_t code, double length_scale);

  auto code() const noexcept -> std::size_t;
  auto length_scale() const noexcept -> double;
  auto to_string() const noexcept -> std::string;

private:
  std::size_t _code{0};
  double _length_scale{1.};
  std::string _repr{"Unknown"};
};

/* Type of UNV elements */
enum class ElementType : std::uint8_t {
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
  Element(std::vector<std::size_t> vertices_ids, ElementType type) noexcept;

  auto vertices_ids() noexcept -> std::vector<std::size_t> &;
  auto vertices_ids() const noexcept -> const std::vector<std::size_t> &;
  auto type() const noexcept -> ElementType;

private:
  std::vector<std::size_t> _vertices_ids;
  ElementType _type;
};

/*
 * UNV group type, can be either Vertex or Element.
 * This is used when parsing UNV mesh groups (or boundary patches and cell
 * zones) each group can be either a vertex group or an element (line, face,
 * cell) group.
 */
enum class GroupType : std::uint8_t {
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
  Group(std::string name, GroupType type,
        std::vector<std::size_t> elements_ids);

  auto name() const noexcept -> const std::string &;
  auto type() const noexcept -> GroupType;

  auto elements_ids() -> std::vector<std::size_t> &;
  auto elements_ids() const -> const std::vector<std::size_t> &;
  auto unique_element_types() const -> const std::unordered_set<ElementType> &;
  void add_element_type(ElementType type);

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
       std::optional<UnitsSystem> unit_system);

  auto vertices() const noexcept -> const std::vector<std::array<double, 3>> &;
  auto elements() const noexcept -> const std::optional<std::vector<Element>> &;
  auto groups() const noexcept -> const std::optional<std::vector<Group>> &;
  auto unit_system() const noexcept -> const std::optional<UnitsSystem> &;

private:
  std::vector<std::array<double, 3>> _vertices;
  std::optional<std::vector<Element>> _elements{std::nullopt};
  std::optional<std::vector<Group>> _groups{std::nullopt};
  std::optional<UnitsSystem> _unit_system{std::nullopt};
};

/**
 * @brief Read UNV mesh from file
 *
 * @param path path to the UNV file
 * @return Mesh
 */
auto read(const std::filesystem::path &path) -> Mesh;

} // namespace unvpp
