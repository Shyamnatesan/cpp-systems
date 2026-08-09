

#include <cstddef>

#include <memory>
#include <utility>

namespace shyam {

template <typename T, typename Allocator = std::allocator<T>> class Vector {
public:
  // Initially, our capacity will be 1. So we allocate space for 1 T.
  explicit Vector(Allocator alloc = Allocator()) {
    ptr_ = AllocTraits::allocate(alloc, 1);
    capacity_ = 1;
  }

  // Upon destruction of vector, we first destruct all the objects, and then
  // deallocate the space.
  ~Vector() {
    Allocator alloc;
    for (size_t i{}; i < size_; ++i) {
      AllocTraits::destroy(alloc, ptr_ + i);
    }

    AllocTraits::deallocate(alloc, ptr_, capacity_);
    ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
  }

  void push_back(T element) {

    // Grow when the vector is at least ~1/3 full. With initial capacity 1 this
    // also triggers a reallocation on the first insertion while size is still
    // 0, so no live object has to be moved.
    if (capacity_ / 3 <= size_) {
      MoveToNewBlock(3 * capacity_);
    }

    // Now there is guaranteed space — construct the new element.
    Allocator alloc;
    AllocTraits::construct(alloc, ptr_ + size_, std::move(element));
    ++size_;
  }

  const T &at(size_t idx) { return ptr_[idx]; }

  size_t get_size() { return size_; }

  size_t get_capacity() { return capacity_; }

  void shrink_to_fit() { MoveToNewBlock(size_); }

  void pop_back() {
    Allocator alloc;
    AllocTraits::destroy(alloc, ptr_ + (size_ - 1));
    --size_;
  }

private:
  using AllocTraits = std::allocator_traits<Allocator>;

  size_t size_{};
  size_t capacity_{1};
  T *ptr_{nullptr};

  // Re-allocation of the vector happens here
  void MoveToNewBlock(size_t new_capacity) {
    Allocator alloc;
    if (new_capacity == 0) {
      AllocTraits::deallocate(alloc, ptr_, capacity_);
      ptr_ = nullptr;
      capacity_ = 0;
      return;
    }

    // allocate a new block of memory of size new_capacity
    auto new_ptr = AllocTraits::allocate(alloc, new_capacity);

    for (size_t i{}; i < size_; ++i) {
      // move all objects from the old block to the new block
      AllocTraits::construct(alloc, new_ptr + i, std::move(ptr_[i]));

      // destroy the old block's object
      AllocTraits::destroy(alloc, ptr_ + i);
    }

    // deallocate the old block of memory
    AllocTraits::deallocate(alloc, ptr_, capacity_);

    ptr_ = new_ptr;
    capacity_ = new_capacity;
  }
};

} // namespace shyam
