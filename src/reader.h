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

#include "common.h"
#include "stream.h"
#include <sstream>
#include <unordered_map>
#include <utility>


namespace unv {

auto static read_first_scalar(const std::string& line) -> std::size_t;
auto static read_n_scalars(const std::string& line, std::size_t n) -> std::vector<std::size_t>;

class Reader {
public:
    Reader() = delete;
    Reader(FileStream& stream) : stream(stream) {};
    Reader(Reader& other) = delete;
    Reader(Reader&& other) = delete;
    auto operator=(Reader& other) -> Reader& = delete;
    auto operator=(Reader&& other) -> Reader& = delete;
    ~Reader() { stream.~FileStream(); }

    void read_tags();
    void read_units();
    void read_vertices();
    void read_elements();
    void read_groups();
    void read_dofs();

    auto units() -> UnitsSystem&;
    auto vertices() -> std::vector<std::array<double, 3>>&;
    auto elements() -> std::vector<Element>&;
    auto groups() -> std::vector<Group>&;

private:
    inline void skip_tag() {
        while (stream.read_line(temp_line) && !is_separator(temp_line)) {
        }
    }

    void adjust_vertices_ids();
    void adjust_elements_ids();
    void set_group_unique_elements(Group& group);

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto read_group_elements(std::size_t n_elements) -> T;

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto read_group_elements_two_columns(std::size_t n_elements) -> T;

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto read_group_elements_single_column() -> T;

    FileStream& stream;
    std::string temp_line;

    UnitsSystem units_system;
    std::vector<std::array<double, 3>> _vertices;
    std::vector<Element> _elements;
    std::vector<Group> _groups;

    std::unordered_map<std::size_t, std::size_t> vertex_id_map;
    std::unordered_map<std::size_t, std::size_t> element_id_map;
};
} // namespace unv
