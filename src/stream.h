#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace unv {
class FileStream {
public:
    FileStream() = delete;
    FileStream(std::ifstream& fstream);
    ~FileStream();

    auto read_line(std::string& line) -> bool;

private:
    std::ifstream* file_stream;
};

} // namespace unv