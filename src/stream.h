#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace unv {
class FileStream {
public:
    FileStream() = delete;
    FileStream(std::ifstream& fstream) : file_stream(&fstream) {}
    FileStream(FileStream& other) = delete;
    FileStream(FileStream&& other) = delete;
    auto operator=(FileStream& other) -> FileStream& = delete;
    auto operator=(FileStream&& other) -> FileStream& = delete;
    ~FileStream();

    auto read_line(std::string& line) -> bool;

private:
    std::ifstream* file_stream;
};

} // namespace unv