#include "stream.h"

namespace unv {
FileStream::FileStream(std::ifstream &fstream) { file_stream = &fstream; }

bool FileStream::readLine(std::string &line) {
    if (file_stream->good()) {
        std::getline(*file_stream, line);
        //++lineNumber;
        return true;
    }
    return false;
}

std::size_t FileStream::readFirstScalar(const std::string &line) {
    std::size_t scalar;
    std::stringstream ss(line);

    ss >> scalar;

    return scalar;
}

std::vector<std::size_t> FileStream::readScalars(const std::string &line,
                                                 std::size_t n) {
    std::vector<std::size_t> scalars;
    std::size_t scalar;
    scalars.reserve(n);
    std::stringstream ss(line);

    for (std::size_t i = 0; i < n; i++) {
        ss >> scalar;
        scalars.push_back(scalar);
    }
    return scalars;
}

std::vector<double> FileStream::readDoubles(const std::string &line,
                                            std::size_t n) {
    std::vector<double> doubles;
    double d;
    doubles.reserve(n);
    std::stringstream ss(line);

    for (std::size_t i = 0; i < n; i++) {
        ss >> d;
        doubles.push_back(d);
    }
    return doubles;
}

FileStream::~FileStream() { file_stream->close(); }

} // namespace unv