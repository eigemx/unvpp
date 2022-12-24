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

    auto readLine(std::string& line) -> bool;

private:
    std::ifstream* file_stream;
};

} // namespace unv