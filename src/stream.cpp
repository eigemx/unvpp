#include "stream.h"

namespace unv {
FileStream::FileStream(std::ifstream& fstream) {
    file_stream = &fstream;
}

auto FileStream::read_line(std::string& line) -> bool {
    if (file_stream->good()) {
        std::getline(*file_stream, line);
        return true;
    }
    return false;
}

FileStream::~FileStream() {
    file_stream->close();
}

} // namespace unv