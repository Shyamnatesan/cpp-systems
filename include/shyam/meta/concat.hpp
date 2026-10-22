#pragma once

#include "shyam/meta/container_int.hpp"

namespace shyam::meta::typelist {

template <typename... Container> struct Concat;

template <int... head1, int... head2>
struct Concat<Numbers<head1...>, Numbers<head2...>> {
  using type = Numbers<head1..., head2...>;
};

} // namespace shyam::meta::typelist

// namespace shyam::meta::typelist
