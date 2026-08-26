

#include <bit>
#include <cassert>
#include <cstddef>
#include <memory>

namespace shyam {

// Basic Arena allocator
// - allocate a fairly large buffer on the heap
// - use this buffer to construct new objects of same/different types
// - once a particular work is done, and when we want to reuse this buffer, we
// simply call reset, which resets the offset to 0, making it available for
// reuse.
// - de-allocates the block on destruction of arena

class Arena {

public:
  explicit Arena(const size_t capacity) : capacity_(capacity) {
    blkptr_ = new std::byte[capacity_];
  }

  ~Arena() { delete[] blkptr_; }

  Arena(const Arena &) = delete;
  Arena &operator=(const Arena &) = delete;

  Arena(Arena &&) = delete;
  Arena &operator=(Arena &&) = delete;

  void *Allocate(const size_t size,
                 const size_t align = alignof(std::max_align_t)) {

    // numbers that are a power of two have a property:
    // their binary representation contains only a single bit
    // So, we can use the <bit> to make sure the align is a power of two.
    assert(std::has_single_bit(align));

    // align the offset
    // 1. we have to push(->) the current offset past the align boundary
    // 2. and then once the offset is past the boundary, we want to push it
    // back(<-) to the next multiple of align
    // this is basically the optimized version of this expression
    //
    // naive version: using division and multiplication
    //  const size_t aligned_offset = ((offset + align - 1) / align) * align
    //
    // optimized verison: using bit manipulation
    const size_t aligned_offset = (offset_ + align - 1) & ~(align - 1);

    // check space
    if (aligned_offset + size > capacity_) {
      return nullptr;
    }

    // bump the offset and return the pointer
    void *result = blkptr_ + aligned_offset;
    offset_ = aligned_offset + size;
    return result;
  }

  void Reset() { offset_ = 0; }

private:
  size_t capacity_{};
  size_t offset_{};
  std::byte *blkptr_{nullptr};
};

} // namespace shyam
