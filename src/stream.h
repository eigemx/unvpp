#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace unv {
class FileStream {
public:
    FileStream(std::ifstream& fstream) : file_stream(&fstream) {}
    [[nodiscard]] auto inline line_number() const -> std::size_t { return _line_number; }

    FileStream() = delete;
    FileStream(FileStream& other) = delete;
    FileStream(FileStream&& other) = delete;
    auto operator=(FileStream& other) -> FileStream& = delete;
    auto operator=(FileStream&& other) -> FileStream& = delete;

    ~FileStream();

    auto read_line(std::string& line) -> bool;

private:
    std::size_t _line_number {0};
    std::ifstream* file_stream;
};

} // namespace unv