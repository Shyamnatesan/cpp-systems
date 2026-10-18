#pragma once

#include "container_int.hpp"

namespace shyam::meta::typelist {

// forward declaration, so we can specialize later
template <int new_element, typename Container> struct PushFront;

// take the new element, and the container's elements as a pack, change the type
// to be a new Numbers with the new_element as the first, followed by the rest
template <int new_element, int... Head>
struct PushFront<new_element, Numbers<Head...>> {
  using type = Numbers<new_element, Head...>;
};

} // namespace shyam::meta::typelist
