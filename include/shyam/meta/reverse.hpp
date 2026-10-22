#pragma once

#include "container_int.hpp"
#include "pushback.hpp"

namespace shyam::meta::typelist {

namespace v1 {
// forward declaration, so we can specialize later
template <typename Container> struct Reverse;

// if empty, then container will be empty
template <> struct Reverse<Numbers<>> {
  using type = Numbers<>;
};

// Walk-through for Reverse<Numbers<4, 3, 2, 1>>
//
// Each step peels the front element, reverses the remaining list, then
// appends the peeled element to the *end* of that already-reversed list.
// Because the append happens on the way back up, the original order is
// reversed.
//
// Start:
//
// head=4, rest=3,2,1
//   -> need Reverse<Numbers<3,2,1>> first
//
//   head=3, rest=2,1
//     -> need Reverse<Numbers<2,1>> first
//
//     head=2, rest=1
//       -> need Reverse<Numbers<1>> first
//
//       head=1, rest=(empty)
//         -> Reverse<Numbers<>> = Numbers<>
//         -> PushBack<1, Numbers<>> = Numbers<1>
//
//     PushBack<2, Numbers<1>> = Numbers<1, 2>
//
//   PushBack<3, Numbers<1, 2>> = Numbers<1, 2, 3>
//
// PushBack<4, Numbers<1, 2, 3>> = Numbers<1, 2, 3, 4>
//
// Final result: Numbers<1, 2, 3, 4>
template <int head, int... rest> struct Reverse<Numbers<head, rest...>> {
  using ReversedTail = typename Reverse<Numbers<rest...>>::type;
  using type = typename PushBack<head, ReversedTail>::type;
};

} // namespace v1

inline namespace v2 {

template <typename NumbersInput, typename NumbersOutput = Numbers<>>
struct Reverse {
  // note that this uses the accumulator style, so the final type points to the
  // accumulated type
  using type = NumbersOutput;
};

// Accumulator-style Reverse
//
// Walk-through for Reverse<Numbers<1, 2, 3>>
//
// The second template argument is an accumulator that starts empty
// and grows at the front while the input shrinks.
//
// Start: input = Numbers<1, 2, 3>, output = Numbers<>
//   Front=1, Rest=2,3, NumbersOutput=(empty)
//   -> Reverse<Numbers<2, 3>, Numbers<1>>
//
//   Front=2, Rest=3, NumbersOutput=1
//   -> Reverse<Numbers<3>, Numbers<2, 1>>
//
//   Front=3, Rest=(empty), NumbersOutput=2,1
//   -> Reverse<Numbers<>, Numbers<3, 2, 1>>
//
// Base case (input empty):
//   type = NumbersOutput  ->  Numbers<3, 2, 1>
//
// Final result: Numbers<3, 2, 1>

template <int Head, int... Rest, int... NumbersOutput>
struct Reverse<Numbers<Head, Rest...>, Numbers<NumbersOutput...>> {
  using type =
      typename Reverse<Numbers<Rest...>, Numbers<Head, NumbersOutput...>>::type;
};

} // namespace v2

} // namespace shyam::meta::typelist
