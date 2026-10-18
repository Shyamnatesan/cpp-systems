#pragma once

#include "shyam/meta/numbers.hpp"

namespace shyam::meta {

template <typename T> struct Accumulate;

template <> struct Accumulate<Numbers<>> {
  static constexpr int result{0};
};

template <int head, int... rest> struct Accumulate<Numbers<head, rest...>> {
  static constexpr int result = head + Accumulate<Numbers<rest...>>::result;
};

} // namespace shyam::meta
