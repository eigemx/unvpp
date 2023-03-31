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

#include "reader.h"

#include <charconv>
#include <cmath>
#include <fast_float/fast_float.h>

namespace unv {

auto inline read_double_triplet(std::string_view line) -> std::array<double, 3> {
    std::array<double, 3> numbers {};

    auto start = line.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        throw std::runtime_error("read_double_triplet(): No number found in line");
    }

    auto pos = start;
    std::size_t count = 0;

    while (pos < line.size() && count < 3) {
        auto end = line.find(' ', pos);

        if (end == std::string_view::npos) {
            double number {};
            auto [p, ec] =
                fast_float::from_chars(line.data() + pos, line.data() + line.size(), number);
            if (ec == std::errc()) {
                numbers[count] = number;
            }
            break;
        }

        double number {};
        auto [p, ec] = fast_float::from_chars(line.data() + pos, line.data() + end, number);
        if (ec == std::errc()) {
            numbers[count] = number;
        } else {
            throw std::runtime_error("Error parsing number");
        }

        pos = line.find_first_not_of(' ', end);
        if (pos == std::string_view::npos) {
            break;
        }

        ++count;
    }

    return numbers;
}

auto inline read_n_scalars(std::string_view line, std::size_t n) -> std::vector<std::size_t> {
    std::vector<std::size_t> numbers;
    numbers.reserve(n);

    auto start = line.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        return numbers;
    }

    auto pos = start;
    std::size_t count = 0;

    while (pos < line.size() && count < n) {
        auto end = line.find(' ', pos);

        if (end == std::string_view::npos) {
            std::size_t number {};
            auto [p, ec] = std::from_chars(line.data() + pos, line.data() + line.size(), number);
            if (ec == std::errc()) {
                numbers.push_back(number);
            }
            break;
        }

        std::size_t number {};
        auto [p, ec] = std::from_chars(line.data() + pos, line.data() + end, number);
        if (ec == std::errc()) {
            numbers.push_back(number);
        } else {
            throw std::runtime_error("Error parsing number");
        }

        pos = line.find_first_not_of(' ', end);
        if (pos == std::string_view::npos) {
            break;
        }

        ++count;
    }

    return numbers;
}

auto inline read_first_scalar(std::string_view line) -> std::size_t {
    auto start = line.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        throw std::runtime_error("read_first_scalar(): No number found in line");
    }

    auto pos = start;
    auto end = line.find(' ', pos);

    std::size_t number {};
    auto [p, ec] = std::from_chars(line.data() + pos, line.data() + end, number);
    if (ec == std::errc()) {
        return number;
    }

    throw std::runtime_error("read_first_scalar(): Error parsing number");
}

auto inline read_first_double(std::string_view line) -> double {
    auto start = line.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        throw std::runtime_error("read_first_double(): No number found in line");
    }

    auto pos = start;
    auto end = line.find(' ', pos);

    double number {};
    auto [p, ec] = fast_float::from_chars(line.data() + pos, line.data() + end, number);
    if (ec == std::errc()) {
        return number;
    }

    throw std::runtime_error("read_first_double(): Error parsing number");
}

auto inline read_nth_scalar(std::string_view line, std::size_t n) -> std::size_t {
    auto start = line.find_first_not_of(' ');

    if (start == std::string_view::npos) {
        throw std::runtime_error("read_nth_scalar(): No number found in line");
    }

    auto pos = start;

    for (std::size_t i = 0; i < n; ++i) {
        auto end = line.find(' ', pos);

        if (end == std::string_view::npos) {
            throw std::runtime_error("read_nth_scalar(): Not enough numbers in line");
        }

        pos = line.find_first_not_of(' ', end);
        if (pos == std::string_view::npos) {
            throw std::runtime_error("read_nth_scalar(): Not enough numbers in line");
        }
    }

    auto end = line.find(' ', pos);

    if (end == std::string_view::npos) {
        std::size_t number {};
        auto [p, ec] = std::from_chars(line.data() + pos, line.data() + line.size(), number);
        if (ec == std::errc()) {
            return number;
        }
    }

    std::size_t number {};
    auto [p, ec] = std::from_chars(line.data() + pos, line.data() + end, number);
    if (ec == std::errc()) {
        return number;
    }

    throw std::runtime_error("read_nth_scalar(): Error parsing number");
}


void Reader::read_tags() {
    while (_stream.read_line(_temp_line)) {
        _temp_line_view = _temp_line;

        if (is_separator(_temp_line_view)) {
            continue;
        }

        switch (tag_kind_from_str(_temp_line_view)) {
        case TagKind::Units:
            read_units();
            break;

        case TagKind::Vertices:
            read_vertices();
            break;

        case TagKind::Elements:
            read_elements();

            // adjust unv vertices index ordering for each element
            adjust_vertices_ids();
            break;

        case TagKind::Group:
            read_groups();

            // adjust unv elements index ordering for each group
            adjust_elements_ids();
            break;

        case TagKind::DOFs:
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

    _stream.read_line(_temp_line);
    _temp_line_view = _temp_line;
    unit_code = read_first_scalar(_temp_line_view);

    _stream.read_line(_temp_line);
    _temp_line_view = _temp_line;
    auto length_scale = read_first_double(_temp_line_view);

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

    while (_stream.read_line(_temp_line)) {
        _temp_line_view = _temp_line;

        if (is_separator(_temp_line_view)) {
            break;
        }

        auto point_unv_id = read_first_scalar(_temp_line_view);

        if (!_stream.read_line(_temp_line)) {
            std::string error_msg {"Failed to read point coordinates at line "};
            error_msg += _stream.line_number();
            throw std::runtime_error(error_msg);
        }

        _vertices.emplace_back(read_double_triplet(_temp_line_view));

        _unv_vertex_id_to_ordered_id_map[point_unv_id] = current_point_id++;
    }
}

void Reader::read_elements() {
    std::size_t current_element_id {0};

    while (_stream.read_line(_temp_line)) {
        _temp_line_view = _temp_line;

        if (is_separator(_temp_line_view)) {
            break;
        }

        auto records = read_n_scalars(_temp_line_view, 6);

        auto element_unv_id = records[0];
        auto element_type = element_type_from_element_id(records[1]);
        auto vertex_count = records[5];

        _stream.read_line(_temp_line);

        if (is_beam_type(element_type)) {
            _stream.read_line(_temp_line);
        }

        _temp_line_view = _temp_line;
        auto vertices_ids = read_n_scalars(_temp_line_view, vertex_count);
        _elements.emplace_back(std::move(vertices_ids), element_type);

        _unv_element_id_to_ordered_id_map[element_unv_id] = current_element_id++;
    }
}

void Reader::adjust_vertices_ids() {
    for (auto& element : _elements) {
        for (auto& v_id : element.vertices_ids()) {
            v_id = _unv_vertex_id_to_ordered_id_map[v_id];
        }
    }
}

void Reader::adjust_elements_ids() {
    for (auto& group : _groups) {
        for (auto& e_id : group.elements_ids()) {
            e_id = _unv_element_id_to_ordered_id_map[e_id];
        }

        set_group_unique_elements_types(group);
    }
}

void Reader::read_groups() {
    // Group number of element is stored at index 7 in group definition
    constexpr std::size_t n_element_pos = 7;

    while (_stream.read_line(_temp_line)) {
        _temp_line_view = _temp_line;
        if (is_separator(_temp_line_view)) {
            break;
        }

        auto n_elements = read_nth_scalar(_temp_line_view, n_element_pos);

        // get group name
        if (!_stream.read_line(_temp_line)) {
            throw std::runtime_error("Failed to read group name");
        }

        // read group name
        auto group_name_start = _temp_line.find_first_not_of(' ');
        auto group_name_end = _temp_line.find_last_not_of(' ');
        auto group_name =
            _temp_line.substr(group_name_start, group_name_end - group_name_start + 1);

        auto [group_elements, group_type] = read_group_elements(n_elements);

        _groups.emplace_back(std::move(group_name), group_type, std::move(group_elements));
    }
}

void Reader::read_dofs() {
    while (_stream.read_line(_temp_line)) {
        _temp_line_view = _temp_line;

        if (is_separator(_temp_line_view)) {
            break;
        }

        // get patch name
        if (!_stream.read_line(_temp_line)) {
            throw std::runtime_error("Failed to read group name in DOFs tag");
        }

        // read group name
        auto group_name_start = _temp_line.find_first_not_of(' ');
        auto group_name_end = _temp_line.find_last_not_of(' ');
        auto group_name =
            _temp_line.substr(group_name_start, group_name_end - group_name_start + 1);

        std::vector<std::size_t> group_vertices;

        while (_stream.read_line(_temp_line)) {
            if (is_separator(_temp_line)) {
                break;
            }
            group_vertices.push_back(
                _unv_vertex_id_to_ordered_id_map[read_first_scalar(_temp_line)]);
        }

        _groups.emplace_back(std::move(group_name), GroupType::Vertex, std::move(group_vertices));
    }
}

void Reader::set_group_unique_elements_types(Group& group) {
    for (auto& element_id : group.elements_ids()) {
        group.add_element_type(_elements[element_id].type());
    }
}

auto Reader::read_group_elements(std::size_t n_elements) -> Reader::GroupDataPair {
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

auto Reader::read_group_elements_two_columns(std::size_t n_elements) -> Reader::GroupDataPair {
    // Warning: this causes a narrowing conversion from 'std::size_t' to 'double
    auto n_rows = static_cast<std::size_t>(n_elements / 2.0);
    std::vector<size_t> elements;
    elements.reserve(n_elements);

    auto group_type = GroupType::Element;

    for (std::size_t i = 0; i < n_rows; ++i) {
        if (!_stream.read_line(_temp_line)) {
            throw std::runtime_error("Failed to read group element");
        }
        _temp_line_view = _temp_line;

        auto records = read_n_scalars(_temp_line_view, 6);
        elements.push_back(records[1]);
        elements.push_back(records[5]);

        group_type = records[0] == 8 ? GroupType::Element : GroupType::Vertex;
    }

    return std::make_pair(elements, group_type);
}

auto Reader::read_group_elements_single_column() -> Reader::GroupDataPair {
    if (!_stream.read_line(_temp_line)) {
        throw std::runtime_error("Failed to read group element");
    }

    _temp_line_view = _temp_line;

    auto records = read_n_scalars(_temp_line, 2);
    auto element = std::vector<std::size_t>({records[1]});
    auto group_type = records[0] == 8 ? GroupType::Element : GroupType::Vertex;

    return std::make_pair(element, group_type);
}
} // namespace unv
