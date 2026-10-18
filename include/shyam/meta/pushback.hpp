#pragma once

#include "container_int.hpp"

namespace shyam::meta::typelist {

// forward declaration, so we can specialize
template <int new_element, typename Container> struct PushBack;

// we take the new element, and the parameter pack(all the elements)
// and change the type to be a new Numbers container, with all the existing
// elements(from the pack), followed by the new_element
template <int new_element, int... Head>
struct PushBack<new_element, Numbers<Head...>> {
  using type = Numbers<Head..., new_element>;
};

} // namespace shyam::meta::typelist
