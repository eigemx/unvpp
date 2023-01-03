#include "stream.h"

namespace unv {
auto FileStream::read_line(std::string& line) -> bool {
    if (file_stream->good()) {
        std::getline(*file_stream, line);
        _line_number++;
        return true;
    }
    return false;
}

FileStream::~FileStream() {
    file_stream->close();
}

} // namespace unv