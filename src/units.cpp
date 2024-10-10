#include "common.h"
#include "unvpp/unvpp.h"

namespace unvpp {
UnitsSystem::UnitsSystem(std::size_t code, double length_scale)
    : _code(code), _length_scale(length_scale) {
  if (code > 10) {
    code = 0; // unknown
  }
  _repr = unv_units_codes.at(code);
}

auto UnitsSystem::code() const noexcept -> std::size_t { return _code; }

auto UnitsSystem::length_scale() const noexcept -> double {
  return _length_scale;
}

auto UnitsSystem::repr() const noexcept -> std::string { return _repr; }

} // namespace unvpp
