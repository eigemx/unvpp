#include <unvpp/unvpp.h>

#include "common.h"
#include "reader.h"
#include "stream.h"

namespace unv {

auto read(std::ifstream &stream) -> Mesh {
    auto fileStream = FileStream(stream);
    auto reader = Reader(fileStream);

    reader.readTags();

    Mesh mesh;

    mesh.unitsSystem =
        reader.units().code > 0 ? std::optional(reader.units()) : std::nullopt;
    mesh.vertices = std::move(reader.vertices());
    mesh.elements = reader.elements().size() > 0
                        ? std::optional(std::move(reader.elements()))
                        : std::nullopt;
    mesh.groups = reader.groups().size() > 0
                      ? std::optional(std::move(reader.groups()))
                      : std::nullopt;

    return mesh;
}

} // namespace unv