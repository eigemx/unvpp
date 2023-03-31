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

#include <fstream>
#include <string>
#include <vector>

namespace unvpp {
class FileStream {
public:
    FileStream(std::ifstream& fstream) : file_stream(&fstream) {}
    auto inline line_number() const -> std::size_t { return _line_number; }

    FileStream() = delete;
    FileStream(FileStream& other) = delete;
    FileStream(FileStream&& other) = delete;
    auto operator=(FileStream& other) -> FileStream& = delete;
    auto operator=(FileStream&& other) -> FileStream& = delete;

    ~FileStream();

    auto read_line(std::string& line) -> bool;

private:
    std::size_t _line_number {0};
    std::ifstream* file_stream {nullptr};
};

} // namespace unvpp