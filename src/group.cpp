#include "unvpp/unvpp.h"

namespace unvpp {
Group::Group(std::string name, GroupType type,
             std::vector<std::size_t> elements_ids)
    : _name(std::move(name)), _type(type),
      _elements_ids(std::move(elements_ids)) {}

auto Group::name() const noexcept -> const std::string & { return _name; }

auto Group::type() const noexcept -> GroupType { return _type; }

auto Group::elements_ids() -> std::vector<std::size_t> & {
  return _elements_ids;
}

auto Group::elements_ids() const -> const std::vector<std::size_t> & {
  return _elements_ids;
}

auto Group::unique_element_types() const
    -> const std::unordered_set<ElementType> & {
  return _unique_element_types;
}

void Group::add_element_type(ElementType type) {
  _unique_element_types.insert(type);
}

} // namespace unvpp
