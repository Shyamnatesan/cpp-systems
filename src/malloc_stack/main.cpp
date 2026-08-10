
#include "assert.h"
#include "malloc_stack.h"
#include <cstdint>

int main() {
  using namespace shyam;

  MallocStack<128> allocator;

  // Fresh allocator should have full capacity
  assert(allocator.remaining() == 128);

  // normal allocation with 8 byte alignment boundary
  void *p1 = allocator.alloc(16, 8);
  assert(p1 != nullptr);
  assert(reinterpret_cast<std::uintptr_t>(p1) % 8 == 0);
  assert(allocator.remaining() == 128 - 16);

  // another allocation with 4 byte alignment boundary
  void *p2 = allocator.alloc(4, 4);
  assert(p2 != nullptr);
  assert(reinterpret_cast<std::uintptr_t>(p2) % 4 == 0);
  assert(allocator.remaining() < 128 - 16);

  // complete free()
  allocator.reset();

  // Force alignment padding(offset alignment)
  //    First take 1 byte (align 1), then request something that needs
  //    align 8. The second pointer must be 8-byte aligned even though
  //    offset was odd.
  void *p3 = allocator.alloc(1, 1);
  assert(p3 != nullptr);

  void *p4 = allocator.alloc(8, 8);
  assert(p4 != nullptr);
  assert(reinterpret_cast<std::uintptr_t>(p4) % 8 == 0);

  allocator.reset();

  // allocate beyond capacity
  void *p5 = allocator.alloc(200, 8);
  assert(p5 == nullptr);
  assert(allocator.remaining() == 128);

  allocator.alloc(32, 8);
  assert(allocator.remaining() < 128);
  allocator.reset();
  assert(allocator.remaining() == 128);
}
