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
#include <iostream>
#include <stdexcept>
#include <unvpp/unvpp.h>

#include "common.h"
#include "reader.h"
#include "stream.h"

namespace unv {

auto is_crlf_and_not_windows(const std::filesystem::path& path) -> bool {
#if !defined(_WIN32) && !defined(_WIN64)
    auto st = std::ifstream {path, std::ios::in | std::ios::binary};
    auto stream = FileStream(st);
    auto line = std::string();
    stream.read_line(line);
    return line.back() == '\r';
#endif

    return false;
}

auto read(const std::filesystem::path& path) -> Mesh {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Input UNV mesh file does not exist!");
    }

    if (is_crlf_and_not_windows(path)) {
        throw std::runtime_error(
            "Input UNV mesh file has Windows line endings, please convert to UNIX line endings.");
    }

    auto st = std::ifstream(path);
    auto stream = FileStream(st);
    auto reader = Reader(stream);
    reader.read_tags();

    Mesh mesh;
    mesh.units_system = reader.units().code > 0 ? std::optional(reader.units()) : std::nullopt;
    mesh.vertices = std::move(reader.vertices());
    mesh.elements =
        !reader.elements().empty() ? std::optional(std::move(reader.elements())) : std::nullopt;
    mesh.groups =
        !reader.groups().empty() ? std::optional(std::move(reader.groups())) : std::nullopt;
    mesh.n_boundary_faces = reader.n_boundary_faces();

    return mesh;
}

} // namespace unv