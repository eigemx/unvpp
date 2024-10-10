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
#include <unordered_map>
#include <utility>
#include <filesystem>

namespace unvpp {

class Reader {
public:
  Reader() = delete;
  Reader(const std::filesystem::path &path);
  Reader(Reader &other) = delete;
  Reader(Reader &&other) = delete;
  auto operator=(Reader &other) -> Reader & = delete;
  auto operator=(Reader &&other) -> Reader & = delete;
  ~Reader() = default;

  void read_tags();

  auto inline units() const noexcept -> const UnitsSystem & {
    return units_system;
  }

  auto inline vertices() const noexcept
      -> const std::vector<std::array<double, 3>> & {
    return _vertices;
  }
  auto inline vertices() noexcept -> std::vector<std::array<double, 3>> & {
    return _vertices;
  }

  auto inline elements() const noexcept -> const std::vector<Element> & {
    return _elements;
  }
  auto inline elements() noexcept -> std::vector<Element> & {
    return _elements;
  }

  auto inline groups() const noexcept -> const std::vector<Group> & {
    return _groups;
  }
  auto inline groups() noexcept -> std::vector<Group> & { return _groups; }

private:
  inline void skip_tag() {
    while (_stream.read_line(_temp_line) && !is_separator(_temp_line)) {
    }
  }

  void read_units();
  void read_vertices();
  void read_elements();
  void read_groups();
  void read_dofs();
  void adjust_vertices_ids();
  void adjust_group_elements();

  using GroupDataPair = std::pair<std::vector<std::size_t>, GroupType>;
  auto read_group_elements(std::size_t n_elements) -> GroupDataPair;
  auto read_group_elements_two_columns(std::size_t n_elements) -> GroupDataPair;
  auto read_group_elements_single_column() -> GroupDataPair;

  FileStream _stream;

  std::string _temp_line;
  std::string_view _temp_line_view;

  UnitsSystem units_system;
  std::vector<std::array<double, 3>> _vertices;
  std::vector<Element> _elements;
  std::vector<Group> _groups;

  std::unordered_map<std::size_t, std::size_t> _unv_vertex_id_to_ordered_id_map;
  std::unordered_map<std::size_t, std::size_t>
      _unv_element_id_to_ordered_id_map;
};
} // namespace unvpp
