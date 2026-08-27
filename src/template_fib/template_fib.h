#pragma once

#include <cstddef>
#include <cstring>

namespace shyam {

// =============================================================================
// Approach 1: constexpr function (iterative, C++14+)
// =============================================================================
// Computes Fibonacci at compile time by running a normal loop inside a
// constexpr function. The compiler evaluates the function when the result is
// needed as a constant (e.g. a template argument or static_assert).
//
// Advantages:
//   - Reads like ordinary runtime code.
//   - O(n) work, O(1) extra "instantiation" depth — no template recursion.
//   - Safe for fairly large n (limited mainly by the integer type).
//
// Requirements:
//   - C++14 or later. C++11 constexpr functions could not contain loops
//     or more than one return statement.
//   - n must be a compile-time constant (template argument or constexpr value).
//
// Note: Fibonacci grows quickly. int overflows after Fib(46) on typical
// 32-bit int. Use a wider type if you need larger indices.
template <int n> constexpr int fib() {
  // Base cases: F(0) = 0, F(1) = 1.
  if (n == 0 || n == 1) {
    return n;
  }

  // Iterative step: walk from F(2) up to F(n), keeping only the last two terms.
  int first{0};  // F(i - 2)
  int second{1}; // F(i - 1)
  for (int i{2}; i <= n; ++i) {
    int third = first + second; // F(i) = F(i - 1) + F(i - 2)
    first = second;
    second = third;
  }
  return second; // F(n)
}

// Thin wrapper so this approach can be used the same way as the TMP version:
//   shyam::Fibonacci<10>::value
template <int n> struct Fibonacci {
  static constexpr int value = fib<n>();
  Fibonacci() {}
};

// =============================================================================
// Approach 2: classic template metaprogramming (recursive, C++98+)
// =============================================================================
// Encodes the recurrence F(N) = F(N-1) + F(N-2) as a class template. Each
// instantiation of Fib<N> depends on Fib<N-1> and Fib<N-2>. The compiler
// "computes" the answer by instantiating the template chain down to the
// explicit specializations for 0 and 1.
//
// Advantages:
//   - Works in C++98 / C++11 (no constexpr loops required).
//   - The result is a compile-time constant: Fib<N>::value
//
// Disadvantages:
//   - Instantiation depth is O(N). Compilers cap this (often ~900–1024).
//     Large N will fail to compile.
//   - Exponential naive recursion is avoided here only because each Fib<K>
//     is instantiated once and reused; still, compile time grows with N.
//
// Pattern:
//   1. Forward-declare the primary template.
//   2. Specialize the base cases (N = 0 and N = 1).
//   3. Define the primary template as the recurrence.

// Primary template declaration. The definition comes after the base cases
// so those specializations are visible when the recurrence is instantiated.
template <size_t N> struct Fib;

// Base case: F(0) = 0
template <> struct Fib<0> {
  static constexpr size_t value = 0;
};

// Base case: F(1) = 1
template <> struct Fib<1> {
  static constexpr size_t value = 1;
};

// Recurrence: F(N) = F(N - 1) + F(N - 2) for N >= 2
template <size_t N> struct Fib {
  static constexpr size_t value = Fib<N - 1>::value + Fib<N - 2>::value;
};

} // namespace shyam
