#pragma once

#include "popfront.hpp"
#include "shyam/meta/container_int.hpp"
#include "shyam/meta/reverse.hpp"

namespace shyam::meta::typelist {

// forward declaration, so we can specialize later
template <typename Container> struct PopBack;

// assume a list [1, 2, 3, 4]
// we have to popback meaning pop() should result in [1, 2, 3], with 4 removed.
//
// one way to do this, is reverse the list: [4, 3, 2, 1]
// once reversed, pop from front. [3, 2, 1]
// after popping, reverse the list back. [1, 2, 3]
//
// we do this, because popping from front is really simple.
//
template <int... head> struct PopBack<Numbers<head...>> {
  using ReversedList = typename Reverse<Numbers<head...>>::type;
  using PoppedList = typename PopFront<ReversedList>::type;
  using type = typename Reverse<PoppedList>::type;
};

} // namespace shyam::meta::typelist
