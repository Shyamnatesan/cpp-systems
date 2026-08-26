

#include "arena.h"

int main() {
  using namespace shyam;

  Arena arena{512};

  // basic allocation
  void *p1 = arena.Allocate(16, 8);
  assert(p1 != nullptr);
  assert(reinterpret_cast<std::uintptr_t>(p1) % 8 == 0);

  // alignment: 1-byte object, then an 8-byte-aligned object
  void *tiny = arena.Allocate(1, 1);
  assert(tiny != nullptr);

  void *p8 = arena.Allocate(8, 8);
  assert(p8 != nullptr);
  assert(reinterpret_cast<std::uintptr_t>(p8) % 8 == 0);
  assert(static_cast<std::byte *>(p8) > static_cast<std::byte *>(tiny));

  // Write through the memory to prove it is usable
  auto *n = static_cast<int *>(arena.Allocate(sizeof(int), alignof(int)));
  assert(n != nullptr);
  *n = 42;
  assert(*n == 42);

  // Reset reuses from the start
  void *before_reset = p1;
  arena.Reset();
  void *after_reset = arena.Allocate(16, 8);
  assert(after_reset != nullptr);
  assert(after_reset == before_reset);

  // Exhaust the arena
  Arena small{32};
  void *a = small.Allocate(24, 8);
  assert(a != nullptr);
  void *b = small.Allocate(24, 8);
  assert(b == nullptr);

  // After reset, the small arena works again
  small.Reset();
  void *c = small.Allocate(24, 8);
  assert(c != nullptr);
  assert(c == a);

  return 0;
}
