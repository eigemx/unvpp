#include "reader.h"

#include <cmath>
#include <iostream>
#include <unordered_map>

namespace unv {

auto inline readFirstScalar(const std::string& line) -> std::size_t {
    return std::stol(line);
}

auto inline readScalars(const std::string& line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> scalars;
    std::size_t scalar;
    scalars.reserve(n);

    auto views = split_views(std::string_view(line));

    for (std::size_t i = 0; i < n; ++i) {
        scalars.push_back(std::stol(views[i].data()));
    }
    return scalars;
}

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
            // Unsupported tags are skipped.
            skipTag();
        }
    }
}

void Reader::readUnits() {
    std::size_t unit_code = 0;

    _stream.readLine(_tempLine);
    unit_code = readFirstScalar(_tempLine);

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

    _unitsSystem = UnitsSystem {
        unit_code,
        lengthScale,
        repr,
    };
}

void Reader::readVertices() {
    std::size_t current_point_id {0};
    std::size_t point_unv_id {0};

    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        auto view = std::string_view(_tempLine);

        point_unv_id = std::stoi(view.substr(0, 10).data());

        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read point coordinates");
        }

        view = std::string_view(_tempLine);

        _vertices.push_back(Vertex {
            std::stod(view.substr(0, 25).data()),
            std::stod(view.substr(25, 25).data()),
            std::stod(view.substr(50, 25).data()),

        });

        _vertex_id_map[point_unv_id] = current_point_id++;
    }
}

void Reader::readElements() {
    std::size_t current_element_id {0};
    std::size_t element_unv_id {0};
    std::size_t vertex_count {0};
    ElementType element_type;

    while (_stream.readLine(_tempLine)) {
        if (isSeparator(_tempLine)) {
            break;
        }

        //auto records = readScalars(_tempLine, 6);
        auto records = split_views(std::string_view(_tempLine));
        element_unv_id = std::stoi(records[0].data());
        element_type = elementType(std::stoi(records[1].data()));
        vertex_count = std::stoi(records[5].data());

        _stream.readLine(_tempLine);

        if (isBeamType(element_type)) {
            _stream.readLine(_tempLine);
        }

        auto vIndices = readScalars(_tempLine, vertex_count);

        // map UNV vertex indices to ordered vertex indices
        for (std::size_t& vIndex : vIndices) {
            vIndex = _vertex_id_map[vIndex];
        }

        _elements.push_back(Element {
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
        auto n_elements = std::stoi(tokens.back());

        // get group name
        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read group name");
        }

        auto group_name = split(_tempLine).at(0);
        trim(group_name);

        auto [group_elements, group_type] = readGroupElements(n_elements);

        _groups.push_back(Group {
            group_name,
            group_type,
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
            group_vertices.push_back(_vertex_id_map[readScalars(_tempLine, 1)[0]]);
        }

        _groups.push_back(Group {
            group_name,
            GroupType::Point,
            std::move(group_vertices),
        });
    }
}

auto Reader::units() -> UnitsSystem& {
    return _unitsSystem;
}

auto Reader::vertices() -> std::vector<Vertex>& {
    return _vertices;
}

auto Reader::elements() -> std::vector<Element>& {
    return _elements;
}

auto Reader::groups() -> std::vector<Group>& {
    return _groups;
}

template <typename T> auto Reader::readGroupElements(std::size_t n_elements) -> T {
    if (n_elements == 1) {
        return readGroupElementsSingleColumn();
    }

    if (n_elements % 2 == 0) {
        return readGroupElementsTwoColumns(n_elements);
    }

    auto [elements, group_type] = readGroupElementsTwoColumns(n_elements - 1);
    elements.push_back(readGroupElementsSingleColumn().first.at(0));

    return std::make_pair(elements, group_type);
}

template <typename T> auto Reader::readGroupElementsTwoColumns(std::size_t n_elements) -> T {
    auto n_rows = static_cast<std::size_t>(n_elements / 2.0);
    std::vector<size_t> elements;
    elements.resize(n_elements + 1);

    std::size_t current_element_number = 0;

    std::vector<std::size_t> records;
    for (std::size_t i = 0; i < n_rows; ++i) {
        if (!_stream.readLine(_tempLine)) {
            throw std::runtime_error("Failed to read group element");
        }

        records = readScalars(_tempLine, 6);
        elements[current_element_number] = records[1];
        elements[current_element_number + 1] = records[5];
        current_element_number += 2;
    }

    auto group_type = records[0] == 8 ? GroupType::Cell : GroupType::Point;

    return std::make_pair(std::move(elements), group_type);
}

template <typename T> auto Reader::readGroupElementsSingleColumn() -> T {
    if (!_stream.readLine(_tempLine)) {
        throw std::runtime_error("Failed to read group element");
    }

    auto records = readScalars(_tempLine, 2);
    auto element = std::vector<std::size_t>({records[1]});
    auto group_type = records[0] == 8 ? GroupType::Cell : GroupType::Point;

    return std::make_pair(element, group_type);
}
} // namespace unv
