

#include <cmath>
#include <cstddef>
#include <limits>

// malloc on the stack
namespace shyam {

template <size_t Capacity> class MallocStack {
public:
  // TODO: Implement LIFO free(), by tracking the allocations made
  MallocStack() = default;
  ~MallocStack() = default;

  void *alloc(size_t size, size_t align = alignof(std::max_align_t)) {
    // Step 1: Round the requested size UP to the next multiple of `align`.
    //
    // We need the allocated block to end on an alignment boundary so that
    // the next allocation can start cleanly. The classic integer way to
    // compute the smallest multiple of `align` that is >= `size` is:
    //
    //     ((size + align - 1) / align) * align
    //
    // Example:
    //   size = 2, align = 8  →  ((2 + 8 - 1) / 8) * 8  = 8
    //   The extra 6 bytes become padding at the end of this block.
    auto aligned_size = ((size + align - 1) / align) * align;

    if (Capacity - offset_ < aligned_size) {
      return nullptr;
    }

    // Step 2: Align the current offset itself.
    //
    // The starting address of the new block must also be a multiple of `align`.
    // We apply the same rounding formula to the current offset:
    //
    //     ((offset_ + align - 1) / align) * align
    //
    // Example:
    //   After a previous alloc(1, 1) the offset is 1.
    //   A new request alloc(2, 4) must start at a multiple of 4.
    //   The formula moves offset_ from 1 to 4 (bytes 1, 2, 3 become padding).
    offset_ = ((offset_ + align - 1) / align) * align;

    auto ptr = static_cast<void *>(blk + offset_);

    offset_ += aligned_size;

    return ptr;
  }

  void reset() {
    // reset this virtual pointer to 0
    offset_ = 0;
  }

  size_t remaining() const {
    // the number of bytes available
    return Capacity - offset_;
  }

private:
  // tracking the current offset, which acts as a virtual pointer
  size_t offset_{};

  // the stack-allocated byte buffer with capacity
  std::byte blk[Capacity]{};
};
} // namespace shyam
