#pragma once

#include "shyam/meta/container_int.hpp"
#include <type_traits>

namespace shyam::meta::typelist {

template <typename InputNumbers, typename OutputNumbers = Numbers<>>
struct Unique {
  using type = OutputNumbers;
};

// specialization for cases, where 2 elements are the same.
// [1, 1], in this case, we skip the duplicate in the input, and add nothing in
// the output.
template <int Head, int... Rest, int... OutputNumbers>
struct Unique<Numbers<Head, Head, Rest...>, Numbers<OutputNumbers...>> {
  using type =
      typename Unique<Numbers<Head, Rest...>, Numbers<OutputNumbers...>>::type;
};

// specialization for cases, where the 2 elements are not the same, ie, all caes
// except the above
// Simply push back the head into the outputnumbers list
template <int Head, int... Rest, int... OutputNumbers>
struct Unique<Numbers<Head, Rest...>, Numbers<OutputNumbers...>> {
  using type =
      typename Unique<Numbers<Rest...>, Numbers<OutputNumbers..., Head>>::type;
};

} // namespace shyam::meta::typelist
