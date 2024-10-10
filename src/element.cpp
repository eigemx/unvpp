#include "unvpp/unvpp.h"

namespace unvpp {

Element::Element(std::vector<std::size_t> vertices_ids,
                 ElementType type) noexcept
    : _vertices_ids(std::move(vertices_ids)), _type(type) {}

auto Element::vertices_ids() noexcept -> std::vector<std::size_t>& {
  return _vertices_ids;
}

auto Element::vertices_ids() const noexcept -> const std::vector<std::size_t>& {
  return _vertices_ids;
}

auto Element::type() const noexcept -> ElementType { return _type; }

} // namespace unvpp
