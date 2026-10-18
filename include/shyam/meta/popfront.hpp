

#pragma once

#include "container_int.hpp"

namespace shyam::meta::typelist {

// forward declaration, so we can specialize later
template <typename Container> struct PopFront;

// iF empty container, then stay empty
template <> struct PopFront<Numbers<>> {
  using type = Numbers<>;
};

// take the elements as first element(Head) and the rest(from second element to
// last element, as a pack), and create a new Numbers with only the rest, and
// skip the first element, and make this the type.
template <int Head, int... Rest> struct PopFront<Numbers<Head, Rest...>> {
  using type = Numbers<Rest...>;
};

}; // namespace shyam::meta::typelist
