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
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#include <unvpp/unvpp.h>

using std::string_view_literals::operator""sv;

namespace unvpp {

const std::array<std::string, 11> unv_units_codes = {
    "Unknown",                // 0
    "SI: Meter (newton)",     // 1
    "BG: Foot (pound f)",     // 2
    "MG: Meter (kilogram f)", // 3
    "BA: Foot (poundal)",     // 4
    "MM: mm (milli newton)",  // 5
    "CM: cm (centi newton)",  // 6
    "IN: Inch (pound f)",     // 7
    "GM: mm (kilogram f)",    // 8
    "US: USER_DEFINED",       // 9
    "MN: mm (newton)",        // 10
};

// supported unv tags strings
constexpr auto SEPARATOR{"    -1"sv};
constexpr auto UNITS_TAG{"   164"sv};
constexpr auto VERTICES_TAG{"  2411"sv};
constexpr auto ELEMENTS_TAG{"  2412"sv};
constexpr auto DOFS_TAG{"   757"sv};

constexpr std::array<std::string_view, 3> GROUP_TAGS{
    "  2452"sv, "  2467"sv,
    // gmsh exports physical groups using 2477 tag
    "  2477"sv};

// group types
constexpr auto POINT_GROUP{"7"sv};
constexpr auto ELEMENT_GROUP{"8"sv};

enum class TagKind : std::uint8_t {
  Separator,
  Units,
  Vertices,
  Elements,
  DOFs,
  Group,
  Unsupported,
};

inline auto tag_kind_from_str(std::string_view tag) -> TagKind {
  static const std::unordered_map<std::string_view, TagKind> tag_kind_map = {
      {SEPARATOR, TagKind::Separator},
      {UNITS_TAG, TagKind::Units},
      {ELEMENTS_TAG, TagKind::Elements},
      {VERTICES_TAG, TagKind::Vertices},
      {DOFS_TAG, TagKind::DOFs},
      {GROUP_TAGS[0], TagKind::Group},
      {GROUP_TAGS[1], TagKind::Group},
      {GROUP_TAGS[2], TagKind::Group},
  };

  auto iter = tag_kind_map.find(tag);

  if (iter != tag_kind_map.end()) {
    return iter->second;
  }

  return TagKind::Unsupported;
}

inline auto element_type_from_element_id(std::size_t unv_element_id)
    -> ElementType {
  switch (unv_element_id) {
  case 11:
  case 21:
  case 22:
  case 24:
    return ElementType::Line;
  case 41:
  case 42:
  case 91:
  case 92:
    return ElementType::Triangle;
  case 44:
  case 45:
  case 94:
  case 95:
  case 122:
    return ElementType::Quad;
  case 111:
  case 118:
    return ElementType::Tetra;
  case 112:
    return ElementType::Wedge;
  case 115:
  case 116:
    return ElementType::Hex;
  default:
    throw std::runtime_error(
        std::string("unvpp::element_type_from_element_id(): ") +
        "Unknown element type id " + std::to_string(unv_element_id));
  }
}

inline auto is_separator(const std::string_view line) -> bool {
  return line.substr(0, 6) == SEPARATOR;
}

inline auto is_beam_type(ElementType element_type) -> bool {
  return element_type == ElementType::Line;
}

} // namespace unvpp
