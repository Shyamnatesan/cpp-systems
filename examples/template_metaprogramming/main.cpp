
#include "shyam/meta/concat.hpp"
#include "shyam/meta/container_int.hpp"
#include "shyam/meta/fibonacci.hpp"
#include "shyam/meta/index_access.hpp"
#include "shyam/meta/popback.hpp"
#include "shyam/meta/popfront.hpp"
#include "shyam/meta/pushback.hpp"
#include "shyam/meta/pushfront.hpp"
#include "shyam/meta/reverse.hpp"
#include "shyam/meta/sum_accumulator.hpp"
#include "shyam/meta/unique.hpp"

#include <limits>

int main() {
  using namespace shyam::meta;

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

  using namespace shyam::meta::typelist;

  // sum accumulator
  static_assert(Accumulate<Numbers<2, 3, 4>>::result == 9, "no match");
  static_assert(Accumulate<Numbers<>>::result == 0, "no match");

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

  using namespace shyam::meta::typelist;

  // popfront

  static_assert(
      std::is_same_v<Numbers<2, 3>, PopFront<Numbers<1, 2, 3>>::type>);

  static_assert(std::is_same_v<Numbers<>, PopFront<Numbers<1>>::type>);
  static_assert(std::is_same_v<Numbers<>, PopFront<Numbers<>>::type>);

  // pushfront
  static_assert(
      std::is_same_v<Numbers<1, 2, 3>, PushFront<1, Numbers<2, 3>>::type>);
  static_assert(std::is_same_v<Numbers<1>, PushFront<1, Numbers<>>::type>);

  // pushback
  static_assert(
      std::is_same_v<Numbers<2, 3, 4>, PushBack<4, Numbers<2, 3>>::type>);
  static_assert(std::is_same_v<Numbers<1>, PushBack<1, Numbers<>>::type>);

  // reverse
  static_assert(
      std::is_same_v<Numbers<1, 2, 3, 4>, Reverse<Numbers<4, 3, 2, 1>>::type>);

  static_assert(std::is_same_v<Numbers<1>, Reverse<Numbers<1>>::type>);

  static_assert(std::is_same_v<Numbers<>, Reverse<Numbers<>>::type>);

  // popback
  static_assert(
      std::is_same_v<Numbers<1, 2>, typename PopBack<Numbers<1, 2, 3>>::type>);

  // concat
  static_assert(std::is_same_v<Numbers<1, 2, 3, 4>,
                               Concat<Numbers<1, 2>, Numbers<3, 4>>::type>);
  static_assert(
      std::is_same_v<Numbers<1>, Concat<Numbers<1>, Numbers<>>::type>);
  static_assert(std::is_same_v<Numbers<>, Concat<Numbers<>, Numbers<>>::type>);

  // unique
  static_assert(
      std::is_same_v<Numbers<1, 3, 4, 5>,
                     typename Unique<Numbers<1, 3, 3, 3, 4, 5, 5>>::type>);
}
