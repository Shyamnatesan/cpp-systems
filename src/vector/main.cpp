#include "vector.h"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

int main() {

  struct Name {
    std::string first_name;
    std::string last_name;
  };

  using namespace shyam;

  Vector<Name> vec;

  vec.push_back(Name{
      .first_name = "one",
      .last_name = "two",
  });
  vec.push_back(Name{
      .first_name = "three",
      .last_name = "four",
  });
  vec.push_back(Name{
      .first_name = "five",
      .last_name = "six",
  });
  vec.push_back(Name{
      .first_name = "seven",
      .last_name = "eight",
  });

  assert(vec.size() == 4);
  assert(vec.capacity() == 27);

  vec.pop_back();
  assert(vec.size() == 3);
  assert(vec.capacity() == 27);

  vec.shrink_to_fit();
  assert(vec.capacity() == vec.size()); // 3
}
