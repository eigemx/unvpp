#include "reader.h"

#include <cmath>
#include <iostream>
#include <unordered_map>

namespace unv {

auto inline read_first_scalar(const std::string& line) -> std::size_t {
    return std::stoul(line);
}

auto inline read_n_scalars(const std::string& line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> scalars;
    scalars.reserve(n);

    auto views = split_to_views(std::string_view(line));

    for (std::size_t i = 0; i < n; ++i) {
        scalars.push_back(std::stoul(views[i].data()));
    }
    return scalars;
}

// overload for string_view line input
auto inline read_n_scalars(const std::string_view line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> scalars;
    scalars.reserve(n);

    auto views = split_to_views(line);

    for (std::size_t i = 0; i < n; ++i) {
        scalars.push_back(std::stoul(views[i].data()));
    }
    return scalars;
}


void Reader::read_tags() {
    while (stream.read_line(temp_line)) {
        if (is_separator(temp_line)) {
            continue;
        }

        switch (tag_type_from_string(temp_line)) {
        case TagType::Units:
            read_units();
            break;

        case TagType::Elements:
            read_elements();

            // adjust unv vertices index ordering for each element
            adjust_vertices_ids();
            break;

        case TagType::Vertices:
            read_vertices();
            break;

        case TagType::Group:
            read_groups();

            // adjust unv elements index ordering for each group
            adjust_elements_ids();
            break;

        case TagType::DOFs:
            read_dofs();
            break;

        default:
            // Unsupported tags are skipped.
            skip_tag();
        }
    }
}

void Reader::read_units() {
    std::size_t unit_code = 0;

    stream.read_line(temp_line);
    unit_code = read_first_scalar(temp_line);

    stream.read_line(temp_line);
    auto length_scale = std::stod(temp_line.substr(0, 25));

    // Unit tag also include force scale, temperature scale and temperature
    // offset, but, since unvpp is mainly a mesh parser, data related to
    // post-processing are ignored.

    std::string repr;

    if (unit_code >= 0 && unit_code < unv_units_codes.size()) {
        repr = unv_units_codes.at(unit_code);
    }

    skip_tag();

    units_system = UnitsSystem {
        unit_code,
        length_scale,
        repr,
    };
}

void Reader::read_vertices() {
    std::size_t current_point_id {0};

    while (stream.read_line(temp_line)) {
        auto line_str_view = std::string_view(temp_line);

        if (is_separator(line_str_view)) {
            break;
        }

        auto point_unv_id = std::stoul(line_str_view.substr(0, 10).data());

        if (!stream.read_line(temp_line)) {
            throw std::runtime_error("Failed to read point coordinates");
        }

        line_str_view = std::string_view(temp_line);

        _vertices.push_back(Vertex {
            std::stod(line_str_view.substr(0, 25).data()),
            std::stod(line_str_view.substr(25, 25).data()),
            std::stod(line_str_view.substr(50, 25).data()),

        });

        vertex_id_map[point_unv_id] = current_point_id++;
    }
}

void Reader::read_elements() {
    std::size_t current_element_id {0};

    while (stream.read_line(temp_line)) {
        auto line_str_view = std::string_view(temp_line);

        if (is_separator(line_str_view)) {
            break;
        }

        auto records = read_n_scalars(line_str_view, 6);

        auto element_unv_id = records[0];
        auto element_type = element_type_from_element_id(records[1]);
        auto vertex_count = records[5];

        stream.read_line(temp_line);

        if (is_beam_type(element_type)) {
            stream.read_line(temp_line);
        }

        line_str_view = std::string_view(temp_line);
        auto vertices_ids = read_n_scalars(line_str_view, vertex_count);


        _elements.push_back(Element {
            std::move(vertices_ids),
            element_type,
        });

        element_id_map[element_unv_id] = current_element_id++;
    }
}

void Reader::adjust_vertices_ids() {
    for (auto& element : _elements) {
        for (auto& v_id : element.vertices_ids) {
            v_id = vertex_id_map[v_id];
        }
    }
}

void Reader::adjust_elements_ids() {
    for (auto& group : _groups) {
        for (auto& e_id : group.elements_ids) {
            e_id = element_id_map[e_id];
        }
    }
}

void Reader::read_groups() {
    while (stream.read_line(temp_line)) {
        if (is_separator(temp_line)) {
            break;
        }

        auto tokens = split(temp_line);
        auto n_elements = std::stoi(tokens.back());

        // get group name
        if (!stream.read_line(temp_line)) {
            throw std::runtime_error("Failed to read group name");
        }

        auto group_name = split(temp_line).at(0);
        trim(group_name);

        auto [group_elements, group_type] = read_group_elements(n_elements);

        _groups.push_back(Group {
            group_name,
            group_type,
            std::move(group_elements),
        });
    }
}

void Reader::read_dofs() {
    while (stream.read_line(temp_line)) {
        if (is_separator(temp_line)) {
            break;
        }

        // get patch name
        if (!stream.read_line(temp_line)) {
            throw std::runtime_error("Failed to read group name in DOFs tag");
        }

        auto group_name = split(temp_line).at(0);
        trim(group_name);

        std::vector<std::size_t> group_vertices;

        while (stream.read_line(temp_line)) {
            if (is_separator(temp_line)) {
                break;
            }
            group_vertices.push_back(vertex_id_map[read_first_scalar(temp_line)]);
        }

        _groups.push_back(Group {
            group_name,
            GroupType::Point,
            std::move(group_vertices),
        });
    }
}

auto Reader::units() -> UnitsSystem& {
    return units_system;
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

template <typename T> auto Reader::read_group_elements(std::size_t n_elements) -> T {
    if (n_elements == 1) {
        return read_group_elements_single_column();
    }

    if (n_elements % 2 == 0) {
        return read_group_elements_two_columns(n_elements);
    }

    auto [elements, group_type] = read_group_elements_two_columns(n_elements - 1);
    elements.push_back(read_group_elements_single_column().first.at(0));

    return std::make_pair(elements, group_type);
}

template <typename T> auto Reader::read_group_elements_two_columns(std::size_t n_elements) -> T {
    // Warning: this causes a narrowing conversion from 'std::size_t' to 'double
    auto n_rows = static_cast<std::size_t>(n_elements / 2.0);
    std::vector<size_t> elements;
    elements.resize(n_elements + 1);

    std::size_t current_element_number = 0;

    std::vector<std::size_t> records;

    for (std::size_t i = 0; i < n_rows; ++i) {
        if (!stream.read_line(temp_line)) {
            throw std::runtime_error("Failed to read group element");
        }

        auto line_str_view = std::string_view(temp_line);
        records = read_n_scalars(line_str_view, 6);
        elements[current_element_number] = records[1];
        elements[current_element_number + 1] = records[5];
        current_element_number += 2;
    }

    auto group_type = records[0] == 8 ? GroupType::Cell : GroupType::Point;

    return std::make_pair(elements, group_type);
}

template <typename T> auto Reader::read_group_elements_single_column() -> T {
    if (!stream.read_line(temp_line)) {
        throw std::runtime_error("Failed to read group element");
    }

    auto records = read_n_scalars(temp_line, 2);
    auto element = std::vector<std::size_t>({records[1]});
    auto group_type = records[0] == 8 ? GroupType::Cell : GroupType::Point;

    return std::make_pair(element, group_type);
}
} // namespace unv
