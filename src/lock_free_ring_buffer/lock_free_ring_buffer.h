
#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <vector>

namespace shyam {

// Resource: https://rigtorp.se/

template <typename T, size_t Capacity> class RingBuffer {
  static_assert(Capacity >= 2);
  static_assert((Capacity & (Capacity - 1)) == 0);

  using Allocator = std::allocator<T>;
  using Traits = std::allocator_traits<Allocator>;

public:
  RingBuffer() { buffer_ = allocator_.allocate(Capacity); }

  ~RingBuffer() {
    T out;
    while (TryPop(out)) {
    }

    Traits::deallocate(allocator_, buffer_, Capacity);
  }

  RingBuffer(const RingBuffer &) = delete;
  RingBuffer &operator=(const RingBuffer &) = delete;

  RingBuffer(RingBuffer &&) = delete;
  RingBuffer &operator=(RingBuffer &&) = delete;

  bool TryPush(T &&item) {

    // when can we successfully push an item ?
    // when the queue is not full
    // how to tell when a circular buffer is full ?
    // the common mistake, that does not work is, once the writer_idx reaches
    // the end, it wraps around to index 0, assuming no reads have been made,
    // then now after the wrap-around, the reader_idx and writer_idx are the
    // same, meaning, the buffer is full, but at the start, when queue is empty,
    // both indices start at 0, and are the same, does that mean the queue is
    // full ? No!. So we need some kind of a flag for distingishing between when
    // the queue is empty and when it is full.
    //
    // So, we can use the last slot in the buffer as a placeholder/flag.
    // when the writer_idx reaches the last slot, it means that it has to now
    // wrap around. so we check if wrap_around(writer_idx + 1) == reader_idx, if
    // so, then the queue is full.

    auto wi = writer_idx_.load(std::memory_order::relaxed);

    auto next_write_idx = wi + 1;
    if (next_write_idx == Capacity) {
      // the writer_idx is currently at the placeholder slot
      // so it should wrap around to 0.
      next_write_idx = 0;
    }

    if (next_write_idx == reader_idx_.load(std::memory_order::acquire)) {
      // after wrapping, if the temporary write_idx(next_write_idx) falls on
      // reader_idx, then we have no space in the buffer.
      return false;
    }

    Traits::construct(allocator_, buffer_ + wi, std::move(item));
    writer_idx_.store(next_write_idx, std::memory_order::release);
    return true;
  }

  bool TryPop(T &out) {

    // when can we pop an item out of the buffer ?
    // when the buffer is not empty.
    // now how do we know if the buffer is not empty ?
    // using the solution above for push, we now can distinguish between empty
    // and full queue. when the writer_idx and reader_idx are equal, then the
    // queue is empty.

    auto ri = reader_idx_.load(std::memory_order::relaxed);
    if (ri == writer_idx_.load(std::memory_order::acquire)) {
      return false;
    }

    // take the item out
    T *slot = buffer_ + ri;
    out = std::move(*slot);
    Traits::destroy(allocator_, slot);

    // check if reader_idx needs to wrap around
    auto next_read_idx = ri + 1;

    if (next_read_idx == Capacity) {
      next_read_idx = 0;
    }
    reader_idx_.store(next_read_idx, std::memory_order::release);
    return true;
  }

private:
  Allocator allocator_{};
  T *buffer_{nullptr};

  // the index the next write will go to.
  // Optimization: aligning this writer_idx to a cacheline size, so it occupies
  // one full cacheline, which prevents false sharing(multiple cores constantly
  // invalidating the cacheline). So, writer_idx and reader_idx stay on
  // different cache lines. Same for reader_idx as well.
  alignas(std::hardware_destructive_interference_size)
      std::atomic<size_t> writer_idx_{};

  // the index the next read comes from.
  alignas(std::hardware_destructive_interference_size)
      std::atomic<size_t> reader_idx_{};

  static_assert(std::atomic<size_t>::is_always_lock_free,
                "atomic<size_t> is not atomic in the underlying hardware");
};

} // namespace shyam
