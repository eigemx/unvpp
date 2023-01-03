#include <iostream>
#include <stdexcept>
#include <unvpp/unvpp.h>

#include "common.h"
#include "reader.h"
#include "stream.h"

namespace unv {

auto is_crlf_and_not_windows(const std::filesystem::path& path) -> bool {
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
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Input UNV mesh file does not exist!");
    }

    if (is_crlf_and_not_windows(path)) {
        throw std::runtime_error(
            "Input UNV mesh file has Windows line endings, please convert to UNIX line endings.");
    }

    auto st = std::ifstream(path);
    auto stream = FileStream(st);
    auto reader = Reader(stream);
    reader.read_tags();

    Mesh mesh;
    mesh.units_system = reader.units().code > 0 ? std::optional(reader.units()) : std::nullopt;
    mesh.vertices = std::move(reader.vertices());
    mesh.elements =
        !reader.elements().empty() ? std::optional(std::move(reader.elements())) : std::nullopt;
    mesh.groups =
        !reader.groups().empty() ? std::optional(std::move(reader.groups())) : std::nullopt;

    return mesh;
}

} // namespace unv