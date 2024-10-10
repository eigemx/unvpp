#include "unvpp/unvpp.h"

namespace unvpp {
Mesh::Mesh(std::vector<std::array<double, 3>> vertices,
           std::optional<std::vector<Element>> elements,
           std::optional<std::vector<Group>> groups,
           std::optional<UnitsSystem> unit_system)
    : _vertices(std::move(vertices)), _elements(std::move(elements)),
      _groups(std::move(groups)), _unit_system(std::move(unit_system)) {}

auto Mesh::vertices() const noexcept
    -> const std::vector<std::array<double, 3>> & {
  return _vertices;
}

auto Mesh::elements() const noexcept
    -> const std::optional<std::vector<Element>> & {
  return _elements;
}

auto Mesh::groups() const noexcept
    -> const std::optional<std::vector<Group>> & {
  return _groups;
}

auto Mesh::unit_system() const noexcept -> const std::optional<UnitsSystem> & {
  return _unit_system;
}

} // namespace unvpp
