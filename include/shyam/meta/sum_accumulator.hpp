#pragma once

#include "container_int.hpp"

namespace shyam::meta::typelist {

template <typename T> struct Accumulate;

template <> struct Accumulate<Numbers<>> {
  static constexpr int result{0};
};

template <int Head, int... Rest> struct Accumulate<Numbers<Head, Rest...>> {
  static constexpr int result = Head + Accumulate<Numbers<Rest...>>::result;
};

} // namespace shyam::meta::typelist
