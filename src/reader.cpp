#include "reader.h"

#include <cmath>
#include <unordered_map>

namespace unv {
void Reader::readTags() {
    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            continue;
        }

        switch (tagTypeFromStr(_tempLine)) {
        case TagType::Units:
            readUnits();
            break;

        case TagType::Elements:
            readElements();
            break;

        case TagType::Vertices:
            readVertices();
            break;

        case TagType::Group:
            readGroups();
            break;

        case TagType::DOFs:
            readDOFs();
            break;

        default:
            skipTag();
        }
    }
}

void Reader::readUnits() {
    std::size_t unit_code = 0;

    _stream.readLine(_tempLine);
    unit_code = _stream.readScalars(_tempLine, 1)[0];

    _stream.readLine(_tempLine);
    auto lengthScale = std::stod(_tempLine.substr(0, 25));

    // Unit tag also include force scale, temperature scale and temperature
    // offset, but, since unvpp is mainly a mesh parser, data related to
    // post-processing are ignored.

    std::string repr;

    if (unit_code >= 0 && unit_code < unv_units_codes.size()) {
        repr = unv_units_codes.at(unit_code);
    }

    skipTag();

    unitsSystem = UnitsSystem{
        unit_code,
        lengthScale,
        repr,
    };
}

void Reader::readVertices() {
    std::size_t current_point_id = 0;
    std::size_t point_unv_id = 0;

    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        point_unv_id = std::stoul(_tempLine.substr(0, 10));

        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read point coordinates");
        }

        _vertices.push_back(Vertex{
            std::stod(_tempLine.substr(0, 25)),
            std::stod(_tempLine.substr(25, 25)),
            std::stod(_tempLine.substr(50, 25)),

        });
        _vertex_id_map[point_unv_id] = current_point_id++;
    }
}

void Reader::readElements() {
    std::size_t current_element_id{0}, element_unv_id{0}, vertex_count{0};
    ElementType element_type;

    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        auto records = _stream.readScalars(_tempLine, 6);
        element_unv_id = records[0];
        element_type = elementType(records[1]);
        vertex_count = records[5];

        _stream.readLine(_tempLine);

        if (isBeamType(element_type)) {
            _stream.readLine(_tempLine);
        }

        auto vIndices = _stream.readScalars(_tempLine, vertex_count);

        // map UNV vertex indices to ordered vertex indices
        for (std::size_t &vIndex : vIndices) {
            vIndex = _vertex_id_map[vIndex];
        }

        _elements.push_back(Element{
            std::move(vIndices),
            element_type,
        });

        _element_id_map[element_unv_id] = current_element_id++;
    }
}

void Reader::readGroups() {
    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        auto tokens = split(_tempLine);
        auto n_elements = parseNumber<std::size_t>(tokens.back());

        // get group name
        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read group name");
        }

        auto group_name = split(_tempLine).at(0);
        trim(group_name);

        auto group_elements = readGroupElements(n_elements);

        // TODO: handle point group type
        _groups.push_back(Group{
            group_name,
            GroupType::Cell,
            std::move(group_elements),
        });
    }
}

void Reader::readDOFs() {
    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        // get patch name
        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read group name in DOFs tag");
        }

        auto group_name = split(_tempLine).at(0);
        trim(group_name);

        std::vector<std::size_t> group_vertices;

        while (_stream.readLine(_tempLine)) {
            if (isSeparator(_tempLine)) {
                break;
            }
            group_vertices.push_back(
                _vertex_id_map[_stream.readScalars(_tempLine, 1)[0]]);
        }

        _groups.push_back(Group{
            group_name,
            GroupType::Point,
            std::move(group_vertices),
        });
    }
}

UnitsSystem &Reader::units() { return unitsSystem; }

std::vector<Vertex> &Reader::vertices() { return _vertices; }
std::vector<Element> &Reader::elements() { return _elements; }
std::vector<Group> &Reader::groups() { return _groups; }

std::vector<std::size_t> Reader::readGroupElements(std::size_t n_elements) {
    if (n_elements == 1) {
        auto element = readGroupElementsSingleColumn();
        return std::vector<std::size_t>{element};
    }

    else if (n_elements % 2 == 0) {
        return readGroupElementsTwoColumns(n_elements);
    } else {
        auto elements = readGroupElementsTwoColumns(n_elements - 1);
        elements.push_back(readGroupElementsSingleColumn());
        return elements;
    }

    // group has no elements
    return std::vector<std::size_t>();
}

std::size_t Reader::readGroupElementsSingleColumn() {
    if (!_stream.readLine(_tempLine)) {
        throw std::runtime_error("Failed to read group element");
    }
    return _stream.readScalars(_tempLine, 1)[0];
}

std::vector<std::size_t>
Reader::readGroupElementsTwoColumns(std::size_t n_elements) {
    auto n_rows = static_cast<std::size_t>(n_elements / 2.0);
    std::vector<size_t> elements;
    elements.resize(n_elements + 1);

    std::size_t current_element_number = 0;

    for (std::size_t i = 0; i < n_rows; ++i) {
        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read group element");
        }

        auto records = _stream.readScalars(_tempLine, 6);
        elements[current_element_number] = records[1];
        elements[current_element_number + 1] = records[5];
        current_element_number += 2;
    }

    return elements;
}

} // namespace unv
