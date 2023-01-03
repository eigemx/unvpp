#include "reader.h"

#include <cmath>

namespace unv {

auto inline read_first_scalar(const std::string& line) -> std::size_t {
    return std::stoul(line);
}

auto inline read_n_scalars(const std::string& line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> scalars;
    scalars.reserve(n);

    auto views = split_to_views(std::string_view(line));

    for (std::size_t i = 0; i < n; ++i) {
        auto str = std::string(views[i].data(), views[i].size());
        scalars.push_back(std::stoul(str));
    }
    return scalars;
}

// overload for string_view line input
auto inline read_n_scalars(const std::string_view line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> scalars;
    scalars.reserve(n);

    auto views = split_to_views(line);

    for (std::size_t i = 0; i < n; ++i) {
        auto str = std::string(views[i].data(), views[i].size());
        scalars.push_back(std::stoul(str));
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

        case TagType::Vertices:
            read_vertices();
            break;

        case TagType::Elements:
            read_elements();

            // adjust unv vertices index ordering for each element
            adjust_vertices_ids();
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
        repr = unv_units_codes[unit_code];
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
        if (is_separator(temp_line)) {
            break;
        }

        auto point_unv_id = std::stoul(temp_line.substr(0, 10));

        if (!stream.read_line(temp_line)) {
            std::string error_msg {"Failed to read point coordinates at line "};
            error_msg += stream.line_number();
            throw std::runtime_error(error_msg);
        }

        _vertices.push_back(std::array<double, 3> {
            std::stod(temp_line.substr(0, 25)),
            std::stod(temp_line.substr(25, 25)),
            std::stod(temp_line.substr(50, 25)),
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


        _elements.emplace_back(std::move(vertices_ids), element_type);

        element_id_map[element_unv_id] = current_element_id++;
    }
}

void Reader::adjust_vertices_ids() {
    for (auto& element : _elements) {
        for (auto& v_id : element.vertices_ids()) {
            v_id = vertex_id_map[v_id];
        }
    }
}

void Reader::adjust_elements_ids() {
    for (auto& group : _groups) {
        for (auto& e_id : group.elements_ids()) {
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

        _groups.emplace_back(std::move(group_name), group_type, std::move(group_elements));
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

        _groups.emplace_back(std::move(group_name), GroupType::Point, std::move(group_vertices));
    }
}

auto Reader::units() -> UnitsSystem& {
    return units_system;
}

auto Reader::vertices() -> std::vector<std::array<double, 3>>& {
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
