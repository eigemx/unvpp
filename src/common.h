#pragma once
#include <algorithm>
#include <cctype>
#include <charconv>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <unvpp/unvpp.h>

#include "stream.h"

namespace unv {
auto verticesCount(std::size_t unv_element_id) -> std::size_t;
auto elementType(std::size_t unv_element_id) -> ElementType;

const std::vector<std::string> unv_units_codes = {
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
const auto SEPARATOR = "    -1";
const auto UNITS_TAG = "   164";
const auto VERTICES_TAG = "  2411";
const auto ELEMENTS_TAG = "  2412";
const auto DOFS_TAG = "   757";
const std::array<std::string, 2> GROUP_TAGS = {"  2452", "  2467"};

enum class TagType {
    Separator,
    Units,
    Vertices,
    Elements,
    DOFs,
    Group,
    Unsupported,
};

inline auto tagTypeFromStr(const std::string& tag) -> TagType {
    if (tag == SEPARATOR) {
        return TagType::Separator;
    }
    if (tag == UNITS_TAG) {
        return TagType::Units;
    }

    if (tag == ELEMENTS_TAG) {
        return TagType::Elements;
    }

    if (tag == VERTICES_TAG) {
        return TagType::Vertices;
    }

    if (tag == DOFS_TAG) {
        return TagType::DOFs;
    }

    if (tag == GROUP_TAGS[0] || tag == GROUP_TAGS[1]) {
        return TagType::Group;
    }

    return TagType::Unsupported;
}

// group types
const auto POINT_GROUP = "7";
const auto ELEMENT_GROUP = "8";

inline auto verticesCount(std::size_t unv_element_id) -> std::size_t {
    switch (unv_element_id) {
    case 11: // rod
    case 21: // linear beam
        return 2;
    case 22: // tapered beam
    case 24: // parabolic beam
    case 41: // plane stress linear triangle
    case 91: // thin shell linear triangle
        return 3;
    case 42: // plane stress parabolic triangle
    case 92: // thin shell parabolic triangle
    case 112:
        return 6;
    case 44:  // plane stress linear quadrilateral
    case 94:  // thin shell linear quadrilateral
    case 122: // rigid element
    case 111:
        return 4;
    case 45:  // plane stress parabolic quadrilateral
    case 95:  // thin shell parabolic quadrilateral
    case 115: // solid Linear Brick
        return 8;
    case 118: // solid Parabolic Tetrahedron
        return 10;
    case 116: // solid Parabolic Brick
        return 20;
    default:
        return 0;
    }
}

inline auto elementType(std::size_t unv_element_id) -> ElementType {
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
        // throw exception for unknown element_type
        throw std::runtime_error("Unknown element type id");
    }
}

inline auto isSeparator(const std::string& line) -> bool {
    return line.substr(0, 6) == SEPARATOR;
}

inline auto isBeamType(ElementType element_type) -> bool {
    return element_type == ElementType::Line;
}

inline auto split(const std::string& line) -> std::vector<std::string> {
    std::vector<std::string> result;
    result.reserve(10);

    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ' ')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

template <typename T> inline auto parseNumber(const std::string& str) -> T {
    T x;
    auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), x);
    if (p == str.data()) {
        throw std::runtime_error("error while parsing numbers");
    }
    return x;
}

} // namespace unv

// https://stackoverflow.com/a/217605/2839539
// trim from start (in place)
inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !static_cast<bool>(std::isspace(ch));
            }));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !static_cast<bool>(std::isspace(ch)); })
                .base(),
            s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}
