#pragma once

namespace shyam::meta {

template <int N> struct Fibonacci;

template <> struct Fibonacci<0> {
  constexpr static int value{0};
};

template <> struct Fibonacci<1> {
  constexpr static int value{1};
};

template <int N> struct Fibonacci {
  constexpr static int value =
      Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

} // namespace shyam::meta
