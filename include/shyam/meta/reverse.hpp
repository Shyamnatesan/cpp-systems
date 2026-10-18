#pragma once

#include "container_int.hpp"
#include "pushback.hpp"

namespace shyam::meta::typelist {

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

} // namespace shyam::meta::typelist
