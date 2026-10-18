#pragma once

#include "container_int.hpp"

#include <limits>
#include <type_traits>

namespace shyam::meta::typelist {

template <int Idx, typename Container> struct ValueAt;

template <int Idx> struct ValueAt<Idx, Numbers<>> {
  constexpr static int value = std::numeric_limits<int>::min();
};

// assume an array [1 2 3 4 5]; the idx is 2; so arr[2] = 3.
// intially we have the head pointing at [1], and rest has [2, 3, 4, 5]
// now, the intuition is to, simply shrink the array from the left, until our
// idx is at 0th pos.
//
// So, head[1]; rest[2, 3, 4, 5], idx = 2
// 1. shrink-left-by-1 -> head[2]; rest[3, 4, 5], idx = 1 (Since we shrunk the
// array, on the left, our idx shrinks by 1.)
// 2. shrink-left-by-1 -> head[3]; rest[4, 5], idx = 0
// Now, the base case idx==0 specialization takes over, and the value is the
// head which is 3.
template <int Idx, int Head, int... Rest>
struct ValueAt<Idx, Numbers<Head, Rest...>> {
  constexpr static int value = (Idx < 0) ? std::numeric_limits<int>::min()
                               : (Idx == 0)
                                   ? Head
                                   : ValueAt<Idx - 1, Numbers<Rest...>>::value;
};

} // namespace shyam::meta::typelist
