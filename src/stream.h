#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace unv {
class FileStream {
  public:
    FileStream() = delete;
    FileStream(std::ifstream &fstream);
    ~FileStream();

    bool readLine(std::string &line);
    std::size_t readFirstScalar(const std::string &line);
    std::vector<std::size_t> readScalars(const std::string &line,
                                         std::size_t n);
    std::vector<double> readDoubles(const std::string &line, std::size_t n);

  private:
    std::ifstream *file_stream;
    // std::size_t lineNumber {0};
};

} // namespace unv