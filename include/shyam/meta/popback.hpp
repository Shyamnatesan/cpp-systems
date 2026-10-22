#pragma once

#include "popfront.hpp"
#include "shyam/meta/container_int.hpp"
#include "shyam/meta/pushfront.hpp"
#include "shyam/meta/reverse.hpp"

namespace shyam::meta::typelist {

namespace v1 {

// forward declaration, so we can specialize later
template <typename Container> struct PopBack;

// assume a list [1, 2, 3, 4]
// we have to popback meaning pop() should result in [1, 2, 3], with 4
// removed.
//
// one way to do this, is reverse the list: [4, 3, 2, 1]
// once reversed, pop from front. [3, 2, 1]
// after popping, reverse the list back. [1, 2, 3]
//
// we do this, because popping from front is really simple.
//
// another way, is recursively calling popback, until we have only one
// element, which will pop the last element. and then, push front the
// remaining in the new list.
//
template <int... head> struct PopBack<Numbers<head...>> {
  using ReversedList = typename Reverse<Numbers<head...>>::type;
  using PoppedList = typename PopFront<ReversedList>::type;
  using type = typename Reverse<PoppedList>::type;
};

} // namespace v1

// ********************************************************************************
//
//          ANOTHER SOLUTION USING PUSHFRONT
//
// ********************************************************************************

inline namespace v2 {

// forward declaration, so we can specialize later
template <typename Container> struct PopBack;

// empty list, stays empty list
template <> struct PopBack<Numbers<>> {
  using type = Numbers<>;
};

// one element in list, after popping, will be empty.
template <int head> struct PopBack<Numbers<head>> {
  using type = Numbers<>;
};

// Start: assume list = [1, 2, 3, 4]
//
// head: 1; rest: [2, 3, 4]
//   -> need PopBack<Numbers<2, 3, 4>>
//
//   head: 2; rest: [3, 4]
//     -> need PopBack<Numbers<3, 4>>
//
//     head: 3; rest: [4]
//       -> need PopBack<Numbers<4>>
//          single-element specialization -> Numbers<>
//
//       PoppedList = Numbers<>
//       PushFront<3, Numbers<>> -> Numbers<3>
//
//   PoppedList = Numbers<3>
//   PushFront<2, Numbers<3>> -> Numbers<2, 3>
//
// PoppedList = Numbers<2, 3>
// PushFront<1, Numbers<2, 3>> -> Numbers<1, 2, 3>
//
// Final result: Numbers<1, 2, 3>
template <int head, int... rest> struct PopBack<Numbers<head, rest...>> {
  using PoppedList = typename PopBack<Numbers<rest...>>::type;
  using type = typename PushFront<head, PoppedList>::type;
};

} // namespace v2

} // namespace shyam::meta::typelist
