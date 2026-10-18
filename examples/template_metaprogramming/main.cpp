
#include "shyam/meta/fibonacci.hpp"
#include "shyam/meta/index_access.hpp"
#include "shyam/meta/sum_accumulator.hpp"

#include <limits>

int main() {
  using namespace shyam::meta;

  // sum accumulator
  static_assert(Accumulate<Numbers<2, 3, 4>>::result == 9, "no match");
  static_assert(Accumulate<Numbers<>>::result == 0, "no match");

  // fibonacci
  static_assert(Fibonacci<0>::value == 0, "no match");
  static_assert(Fibonacci<1>::value == 1, "no match");
  static_assert(Fibonacci<2>::value == 1, "no match");
  static_assert(Fibonacci<3>::value == 2, "no match");
  static_assert(Fibonacci<4>::value == 3, "no match");
  static_assert(Fibonacci<5>::value == 5, "no match");
  static_assert(Fibonacci<6>::value == 8, "no match");
  static_assert(Fibonacci<7>::value == 13, "no match");
  static_assert(Fibonacci<8>::value == 21, "no match");

  // index access
  static_assert(ValueAt<0, Numbers<1>>::value == 1, "");
  static_assert(ValueAt<0, Numbers<1, 2, 3>>::value == 1, "");
  static_assert(ValueAt<1, Numbers<1, 2, 3>>::value == 2, "");
  static_assert(ValueAt<2, Numbers<1, 2, 3>>::value == 3, "");
  static_assert(ValueAt<3, Numbers<1, 2, 3>>::value ==
                    std::numeric_limits<int>::min(),
                "");
  static_assert(ValueAt<-4, Numbers<1, 2, 3>>::value ==
                    std::numeric_limits<int>::min(),
                "");
}
