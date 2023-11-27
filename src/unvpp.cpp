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
#include <unvpp/unvpp.h>

#include <iostream>
#include <stdexcept>

#include "common.h"
#include "reader.h"
#include "stream.h"

namespace unvpp {

auto is_crlf_and_not_windows(const std::filesystem::path& path) -> bool {
    /**
     * @brief Check if the input file has Windows line endings and is not a Windows machine.
     * 
     * @param path path to the input file
     * @return true if the input file has Windows line endings and is not a Windows machine
     */
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
    /**
     * @brief Read an input UNV mesh file.
     * 
     * @param path path to the input UNV mesh file
     * @return Mesh
     */
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Input UNV mesh file does not exist!");
    }

    // check if input is not a file
    if (!std::filesystem::is_regular_file(path)) {
        throw std::runtime_error("Input UNV mesh file is not a regular file!");
    }

    if (is_crlf_and_not_windows(path)) {
        throw std::runtime_error(
            "Input UNV mesh file has Windows line endings, please convert to UNIX line endings.");
    }

    auto st = std::ifstream(path);
    auto stream = FileStream(st);
    auto reader = Reader(stream);
    reader.read_tags();

    return Mesh {reader.vertices(), reader.elements(), reader.groups(), reader.units()};
}

} // namespace unvpp